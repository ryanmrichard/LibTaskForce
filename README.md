# LibTaskForce

# Summary
LibTaskForce is a wrapper around the Madness library that makes it easier to 
use.  In particular, I find Madness's documentation insufficient, especially
for people who are not terribly C++ savvy.  Consequentially, a large goal of
the current project is just to provide a user friendly library complete with
user friendly documentation.  

The second goal is to extend Madness's task based, threaded parallelism to
distributed parallelism.  In my experience, one often wants one's coarse-grained
parallelism to be distributed parallelism, if it is present, or threaded
parallelism otherwise.  Rather than have to write two codes, the decision is
offloaded to this library which tracks the available resources for each type of
parallelism.

# Task vs. Data Parallelism
For the most part any parallel task can be categorized as either task-based or
data-based.  The distinction is whether we are applying the operations to
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

- Task: Some operation and one associated input data set.  For the
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

# What This Library Does

Say we have to run a function with a lot of different inputs.  What this library
does is take all those inputs and run them in a hybrid MPI/threaded fashion, 
with the exact process/thread breakdown being (somewhat) tunable. 

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
Environment Env;

//Get a communicator, which is our liason to the environment
//These are unique instances because they represent resources
//Copy, Assignment, and Moving are prohibited
const Communicator& CurrentComm=Env.Comm();

//Now we need tasks to run, our function in this example will
//take pairs of doubles. We put every pair we care about in a vector
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
//moved, or assigned.  When you are done with the resources ("NewComm" goes out
// of scope), the resources will be added back into the pool
Communicator NewComm=CurrentComm.Split();

//Now we just add our tasks (using the C++11 range-based for-loop)
//Note that we are not doing anything special for the MPI, specifically we are
//not blocking some processes from reaching this function.  In fact, AddTask(),
//assumes that all MPI processes that exist on "CurrentComm" will call AddTask
//with each task.
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

Disclaimer: reductions are not working at the moment (and since foreach is a
wrapper around reduction it's also not working).

\TODO: Make reductions work

# Building the Library
Now that you know how awesome the library is you probably want to build it.
That should be pretty easy (in the top-level directory):

```bash
cmake -H. -Bbuild
cd build && make
make install
```

Of course if your compilers etc. are not in the typical places you'll need to
set the appropriate variables.

```bash
#Useful environment variables:

CXX=<Path to your C++ compiler>
CC=<Path to your C compiler>

#Useful CMake variables (pass to cmake with -D<name of variable>:

MPI_C_COMPILER=<Path to the MPI wrapper around the compiler set to CC>
MPI_CXX_COMPILER=<Path to the MPI wrapper around the compiler set to CXX>
PYTHON_EXECUTABLE=<Path to a python executable> #Required for Madness dependency
CMAKE_BUILD_TYPE=Debug #or Release
```



# Advanced Usage/Topics

## TaskResult Object
The above example should work, but it's going to be very ineffiecent in terms of
communication because each MPI process is going to send its data to every other
MPI process for every element in the `Results`.  Instead if you replace:

```C++
std::vector<Future<double>> Results;
```

with:

```C++
TaskResults<double> Results;
```

The communication will be far more reasonable (two gathers and a broadcast.  The
first gather is for the number of elements each process ran, the next grabs the
elements via gatherv, and then the broadcast gives them to you).

## Controlling Resources

`Split()` allows you to set the maximum number of process/threads each task 
gets.  For example, if you want to give each task 2 MPI processes the syntax is:

```C++
Communicator NewComm=CurrentComm.Split(CurrentComm.NThreads(),2);
```

The first argument is the number of threads each process gets.  In this case
we are giving each process all of the threads on the node.  The second argument
is the number of processes for each task.  

Disclaimer: at the moment, the first argument is ignored because the second is
greater than 1.  I have decided that since your task is using multiple MPI
processes you have to finish divvying them up before you can start divvying up
threads.  This means I will always give you all the threads if you request more
than one MPI process per task.

If you wanted more than one thread per task the syntax would be:

```C++
Communicator NewComm=CurrentComm.Split(2);
//Could also explicitly fill in the 1 MPI process
Communicator NewComm=CurrentComm.Split(2,1);//Exactly the same as above
```

Disclaimer:  Madness keeps the threads in a singleton threadpool.  So there is 
no way for us to pull threads back from Madness.  If you want to run threads in
your task I recommend you request `NThreads()` threads per task.

Finally, if you don't want to use all of the MPI processes to run tasks (say
you have 100 MPI processes available, but 20 tasks to run), you can request that
only a subset of the processes be used by:

```C++
Communicator NewComm=CurrentComm.Split(1,1,10);
```

This call requests that at most 10 of the available processes run your tasks,
with one task per thread on each of the 10 processes.  Of course you can combine
these three options to get pretty much any distribution of parallel resources
you want.

