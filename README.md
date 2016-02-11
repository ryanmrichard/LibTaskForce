# LibTaskForce

# Summary
LibTaskForce is a wrapper around the Madness library that makes it easier to 
use.  In particular, I find Madness's documentation insufficient, especially
for people who are not terribly C++ savvy.  Consequentially, a large goal of
the current project is just to provide a user friendly library complete with
user friendly documentation.

# Task vs. Data Parallelism
For the most part any parallel task can be categorized as either task-based or
data-based.  The distinction is whether we are applying the same operation to
lots of little data sets (task-based) or one operation to one large data set
(data-based).  In electronic structure theory a finite-difference computation of
the geometric gradient is an example of the task-based (we have lots of 
geometries for which we need to evaluate the energy) and tensor contraction is
an example of data-based (we have two large tensors and want to form a third).
We note in passing that data-based parallelism is still often tackled by
task-based methods (for example in tensor contraction, rather than thinking of
your resulting large tensor as one data set, you can think of it as 
many small elements, each of which needs to be computed by the same operation).

Within electronic structure theory, opportunities for data-parallelism exist
almost entirely in the form of tensors and our other side-project is a wrapper
around Tiled Array (insert github link here for wrapper).  The current project,
LibTaskForce, focuses entirely on task-based parallelism.  By combining both
libraries it should be possible to parallelize any routine with minimal effort.

# Concepts
These are the fundamental concepts we need you to know:

- Task: Some operation and one associated input data.  For the
        finite-difference example above, the actual task would be the function
        that computes the energy and one geometry for which the energy will be
        computed.

- Callback: Formally a piece of executable code that is passed as an argument to
            another piece of code.  More simply, it is a function that is passed
            into another function and then called at a pre-defined location.  As
            you likely have pieced together, the operation part of the task is
            a callback.  In C++ this is usually handled via functors...

- Functor: A callable object, i.e. a class that has  `operator()` defined.  
           There really is not too much more to it, despite the rather
           complicated definitions available.  The exact signature of 
           `operator()` will also vary from function to function.  Unlike a
           function pointer in C, functors allow you store member data
           making it convenient to access that data during the actual call.

- Iterator: An object that knows how to traverse an associated container.  The
            STL uses iterators all over the place so that one can iterate over
            the elements of different containers in a uniform way.  For example
            consider looping over an `std::vector` and an `std::set`.  The
            `std::vector` stores its elements in a fashion that iterating like:
            `for(i=0;i<MyVector.size();++i)MyVector[i]` works; however, the
            same code would not work for an `std::set`.  Iterators work by
            asking a container for an iterator to the beginning of the container
            and the end of the container; usually these functions are called
            `begin()` and `end()` respectively.  Some functions of this
            library will need iterators as input, which is why I listed them
            here.

- Distributed vs. Shared Parallelism: I have tried my best to insulate you from
            the low-level aspects of these two types of parallelism, but to get
            your code screaming fast you'll have to tweak some small details.
            For all intents and purposes, distributed parallelism is MPI and
            shared is threading.  Slightly more in detail, distributed deals
            with passing data between nodes and running different operations
            on different nodes.  Shared is about running on different cores on
            the same node.  The caveat here is that a lot of codes have some
            threading support already so you may have to adjust the number of
            threads this library uses so that you are not oversubscribing.

# Usage

```C++
//This is our functor operation
class MyOperation{
   public:
      double operator()(const double& lhs,const double& rhs)const{
          //Do stuff that combines lhs and rhs
          double result;
          return result;
      }
};

//This is our environment, i.e. all the resources and their current allocations
//Your program should have one sitting around, use it
//(how you actually get this is not shown here)
Environment Env;

//Get a communicator, which is our liason to the environment
//These are unique instances because they represent resources
//Copy, Assignment, and Moving are prohibited
const Communicator& CurrentComm=Env.Comm();

//Now we need tasks to run, our function, in this example it
//takes pairs of doubles, so we put every pair we care about in a vector
//(the actual filling is not shown here)
std::vector<std::pair<double,double>> DataSets;

//Results are always returned as futures.  In this example we assume that we
//we want to store the output of each call to our function.  For example,
//DataSets[0]'s result will be stored in Results[0] and so on...
std::vector<Future<double>> Results;

//Now we request our parallel resources.  Split actually takes up to three
//arguments, but by default it will schedule 
//(number of processes)*(threads per process) tasks, which we assume is what
//we presently want.  Like "CurrentComm", "NewComm" can not be copied,
//moved, or assigned.  We return it to you in a const unique_ptr so that when
//you are done with the resources ("NewComm" goes out of scope), the
//resources will be added back into the pool
const std::unique_ptr<Communicator> NewComm=CurrentComm.Split();

//Now we just add our tasks (using the C++11 range-based for-loop)
for(const std::pair<double,double>& Data : DataSets)
   Results.push_back(NewComm.AddTask(MyOperation,Data.first,Data.second));

//From this point on, tasks are running "backgrounded" if you like, so you
//can continue to do things until you need the results

//Now we are ready for our results and decide we are going to print them
for(size_t i=0;i<Results.size();i++)
   std::cout<<*Results[i]<<std::endl;//The futures behave like pointers

//That's it, we ran our tasks in a hybrid MPI/threaded nature!!!!
```

We can get a lot more fancy.  Only using parts of the resources available, 
performing "reductions" on our data set, adding tasks in a "foreach" fashion.


# In Depth Strategy

This section focuses on how we secured the same interface for both MPI and 
threading.  As far as I can tell Madness does not do this by default.
The general idea is all calls go through one of the three ways of registering
tasks: manually via AddTask, via Reduce, or via ForEach.  In Madness, these
functions are tied to a thread queue.  We therefore need to implement the
correct MPI logic over the threading.  By default we assume each task requires
one thread, this means we simply assign the tasks, via RoundRobin to each node.
That node then adds the task to its task queue.  

If each task requires more than one thread, we make a new Madness world that has
floor(N/n) threads where N is the total number of threads per node and n is the 
number of threads each task wants.  We then proceed as above. How/why this works
is best shown by example.  Consider N=6 and n=2.  Normally, Madness thinks it
has 6 threads to work with, but now we throttle that down to 3.  Each task is
then scheduled on one of those three threads.  When a task runs it may then
spawn 1 additional thread (it itself is already a valid thread that shouldn't
be wasted).  Thus we have three groups of two threads running, in agreement
with the user specification.

If each task requires more than one MPI process we then split the current MPI
communicator into ceil(M/m) communicators, where M is the total number of
processes we have and m is the number each task wants.  How this breaks down at
the thread level is a bit tricky.  We take the stance that requiring more
than one MPI process means you actually need m*N threads per task.  If it
required less threads then there was no point to require m processes.  For
example consider m=2 and n=1.  What you are saying is that each task requires
one thread, or that it requires 1/N of the resources on each node, but then you 
say each task requires two processes, meaning it needs all the resources 
available to both nodes.  These are inconsistent statements.  Admittedly there
is an implicit assumption here: process maps to node.  If process maps to
thread then the logic appears to break down; however, in such a scenario N
actually equals 1 and thus you are requesting 2*1 threads, which is consistent
with our original assumption.  It is straightforward to show that this is also
true for intermittent process to thread mappings.
