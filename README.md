# LibTaskForce

## Summary
Opportunities for task-based algorithms usually appear in the form of loops over
tasks.  A task for our purposes is a section of code that can run independently
of any other section of code.  For example the body of a for-loop that depends
only on the iteration number is a task, as is a call to a function that only
depends on its input and local variables.  Task based parallelism is often
referred to as "embarassing parallelism" or more modernly "pleasent parallelism"
because parallelizing such regions of code is embarassingly easy.  Owing
 to typical coding paradigms, loops over tasks usually occur early on in a 
routine and tend to represent the coarsest oppurtunity for parallelism in the 
code/algorithm.  With the popularity of using MPI for distributed memory
parallelism and one of several threading libraries for shared memory this
tends to lead to a situation where such loops have to be written
twice: once for the scenario that we are using process based parallelism and
once for the case where only threading is available.  That is semantically the
code looks something like:

~~~{.cpp}
if(MPI && NProcs>1){
    //MPI related scheduling leading to a list of tasks
    for(auto Task: TaskList){
       //Run Task, use threads within task
    }
    //MPI synchronization
}
else if(!MPI || NProcs==1){
    //Thread related scheduling leading to a list of tasks
    for(auto Task: TaskList){
       //Run tasks, not using threads to avoid contention
    }
    //Shared memory so no synchronization
}
~~~

Unfortunately one needs to write two codes as the interfaces are fundamentally
 different for the two scenarios.  Conceptually, however, we are doing the
same thing, running a bunch of independent things and possibly collecting the
results.

With LibTaskForce the entire code for the two loops can be written as:
~~~{.cpp}
HybridEnv Env;//<------Holds details of parallel environment (usually made at
                       program startup)
HybridComm Comm=Env.comm().split();//Gives us an object for running tasks
std::vector<HybridFuture<return_type>> Results;//This is where the results go

//Loops over a set of tasks pushing futures back into our results vector
for(auto Task: TaskPool)Results.push_back(Comm.add_task<return_type>(Task));

//Some time later grab the result
return_type AResult=Results[0].get();
~~~
This code takes care of scheduling, synchronization, task dependencies etc. for
you.  Furthermore it is asynchronous (relying on a generalization of the
future/promise model of C++11 to distributed memory).

In summary LibTaskForce's features are:
- Uniform interface for launching tasks that use shared, distributed, 
  and/or hybrid memory parallelism
- Uniform interface for retrieving the results of these tasks
- Support for asynchronous workflows
- Automatic scheduling and load-balancing

## Status
The section above details LibTaskForce in its idealized state.  It admittedly
isn't quite there yet.  The current TODO list:
- Expand asynchronous operations for distributed memory
- Expand load-balancing strategies for distributed memory
- Enable distributed reduce
- Expand hybrid support
- Better unify API

### Obtaining and Building
After getting the source code from this repo, go ahead and gather 
#### Dependencies
LibTaskForce depends on:
- MPI for distributed memory manipulations 
- TBB for shared memory manipulations (add link to Intel's website)
- Cereal (add link) for serialization (will be made if you do no provide)
- C++11 because it is awesome (i.e. you need a compiler from the last 3 years)
#### Building
LibTaskForce follows the typical CMake pattern for a superbuild.  Under this
pattern everything is build as external projects, which allows all parts of the
build to experience the same interface.  This also ensures that all parts
of the build interact with one another via the usual CMake variables and channels
such as find_package.

Basically, this amounts to you having to run:

~~~{.sh}
#The -Bbuild flag tells CMake to make a build directory called "build"
cmake -Bbuild (other options go here)
cd build && make
make install
~~~

