#ifndef PARALLEL_COMMUNICATOR_HPP_
#define PARALLEL_COMMUNICATOR_HPP_

#include <type_traits> //For std::result_of
#include <mpi.h> //For MPI_Comm
#include "Future.hpp"
#include "madness/world/MADworld.h"
#include "CommStats.hpp"
#include "Serialization.hpp"
namespace LibTaskForce{
    
class Environment;
class CommStats;

/** \brief Class that handles interactions with the parallel environment
 *
 *
 *   At least at the moment, this is just a wrapper around Madness's World.
 *   Madness's Worlds appear to be made up of four main components:
 *   1. An MPI interface
 *   2. An active message interface
 *   3. A task queue
 *   4. A global operations interface
 *
 *   Of these, the task queue is the most straightforward.  The task queue
 *   is a thread pool, that when given a task and input, returns a future
 *   to the result of the task.  We wrap the future with our own Future class.
 *   The task queue is also capable of for each and reduction.  Together these
 *   three operations allow you to
 *   add arbitrary tasks, a series of related tasks, and a series of related
 *   tasks that require simple post processing.  All of the Madness related
 *   operations are wrapped by this interface.
 * 
 *   Note Madness doesn't seem to handle serial in the way I would like, it
 *   still spawns a thread, which means we now have two threads.so we take care
 *   of that here.
 * 
 *   Madness lets us have multiple worlds.  My current thought is: what if we
 *   use this to throttle threads?  Best I can tell this is working...
 * 
 *   The last trick is how to do the MPI behind the guise of the future.  
 *   There's two real scenarios: every task is running on one MPI process or
 *   every task involves multiple MPI processes.  Runs not using MPI fall under
 *   the former.  In the former, we need to determine if we are running a
 *   given task, if we aren't than we make a future that knows who is, otherwise
 *   we add the task to our pool.
 * 
 *   
 *
 *
 */
class Communicator{
   public:
      ///Causes all members of the current communicator to wait at this call
      ///until all other members arrive
      void Barrier()const;

      ///Returns the number of tasks this comm has scheduled, a unique ID per
      ///comm
      size_t NTasks()const{return TasksAdded_;}
      /** \brief Adds a task to the queue and then gives you a future to the
       *  result.
       * 
       *  Let's say you have some function, creatively named "MyFxn" that
       *  returns an object of type "ReturnType".  Furthermore, assume your
       *  function takes two arguments of types "Type1" and "Type2".  Now
       *  assume you have a whole bunch of pairs of objects of "Type1" and
       *  "Type2" for which you want to evaluate "MyFxn".  You ask this
       *  communicator to run  those tasks via something like:
       *  \code
       *  Communicator AComm;
       *  Type1 Input1,Input2,Input3;
       *  Type2 Input4,Input5,Input6;
       *  Future<ReturnType> Result1=AComm.AddTask(MyFxn,Input1,Input4);
       *  Future<ReturnType> Result2=AComm.AddTask(MyFxn,Input2,Input5);
       *  Future<ReturnType> Result3=AComm.AddTask(MyFxn,Input3,Input6);
       *  \endcode
       *  Each time you add a task, you are given a Future back.  See the Future
       *  documentation for more details on using it.
       * 
       *  \params[in] YourTask a function or functor that is callable with the
       *              next N arguments to AddTask
       *  \params[in] args N arguments that will be passed to the callable
       *                     object you gave us as YourTask
       *  \return A future to an object of type ReturnType, where ReturnType is
       *          the return type of YourTask.
       * 
       *  Note: This ultimately calls Madness, which has a 9 argument limit.
       *  If you need more than 9 arguments consider
       *  using a tuple as one of the arguments, or some other container.
       * 
       *  Also note that if you run with MPI the result may be communicated
       *  among processes
       */
      template<typename Fxn_t,typename...Args>
      Future<
        typename RemoveFuture<
            typename std::result_of<Fxn_t(Args...)>::type
                 >::type
      >
      AddTask(Fxn_t YourTask,Args... args);
      
      /** \brief Applies a given operation to every piece of data in the range
       * 
       *   Say you have a large vector of doubles that you want to add up.
       *   If you pass the iterator to the first and last elements in the
       *   range [first,last) then this function will add up the elements
       *   in parallel.  The result is returned as a Future<double> so that it
       *   can be done asynchronously.  We of course don't want to be restricted
       *   to only doubles, so this function is templated on the type of the
       *   objects we are reducing.  In general we will call that type: Result_t
       *   Of course you don't always want to add things up.  Sometimes you want
       *   to subtract, etc.  In general you need to provide me a visitor (a
       *   functor with multiple methods) with the following functions:
       *   \code
          class Operation{
               public:
                //Must be copyable
                 Operation(const Operation&)=default;
                 //Must be default constructable
                OpWrapper()=default;
                //Allows you to do something given your iterator's current
                //value
                 Result_t operator()(const Itr_t& Itr)const;
                //The operation that will combine two Result_t's
                 Result_t operator()(const Result_t& lhs,
                                    const Result_t& rhs)const;
            };
       *   \endcode
       * 
       *   To add the sum of the squares of a series of values:
       * 
       *   \code
       *   //Get a communicator somehow
       *   Communicator Comm;
       *  
       *   //Make a vector filled with the numbers 1 to 100
       *   std::vector<double> Values;
       * 
       *   //You can declare a class inline, if you didn't know
       *   class MySumOfSquares{
       *      public:
       *         MySumOfSquares(const MySumOfSquares&)=default;
       *         MySumOfSquares()=default;
       *         double operator()(const std::vector<double>::iterator& lhs)
       *           const{return (*lhs)*(*lhs);}
       *         double operator()(double lhs,double rhs)const{
       *             return lhs+rhs;
       *         }
       *   };
       * 
       *   //Now perform the operation
       *   Future<double>=Comm.Reduce<double>(Values.begin(),
       *                                      Values.end(),
       *                                       MySumOfSquares());
       *   \endcode
       *
       *   \param[in] BeginItr An iterator to the beginning of the range of
       *                       objects we are iterating over
       *   \param[in] EndItr   An iterator to the end of the range we are
       *                       iterating over
       *   \param[in] Op       The reduction operation
       *   \param[in] ChunkSize How many reductions each thread does at a time.
       *                         (See expert note below).
       *   
       *   Expert note. The last argument allows for finer control of the
       *   parallelism by allowing the user to specify how many reductions a
       *   task entails.  For example, in the code above first 0+1 is a task,
       *   then (0+1)+2, is a task, then (0+1+2)+3 is a task etc.  That is each
       *   reduction, each of which is a task, reduces one element (a chunk size
       *   of 1).  There is a small amount of overhead associated with adding 
       *   each task to the queue, which in this case will be more
       *   overhead than the actual addition.  For this reason you may want to
       *   specify a larger chunk size.  For example if you set the chunk size
       *   to 4, the first task would be 0+1+2+3+4, the second task would be:
       *   (0+1+2+3+4)+5+6+7+8, etc.  Once you have your parallel code running
       *   it's a simple task to find a chunk size that is optimal for your
       *   code.
       * 
       * 
       *   Implementation note.  We call Madness's reduce, which works by
       *   recursion.  It splits the range in two (the left and right), each of
       *   these ranges is then passed to reduce and this continues until the
       *   input range is smaller than the maximum range size.  Once we have
       *   a sufficient range the operation is called like:
       *   \code
       *   result=op(result,op(itr))
       *   \endcode
       *   where result is  of type Result_t and itr is an iterator to a range
       *   of elements of type Result_t.  Presently we build a small wrapper
       *   class inside this function that handles these semantics.  The user
       *   should not have to worry about this. We mirror this behavior at
       *   the MPI level
       */
      template<typename Result_t,typename Itr_t,typename Op_t>
      Future<typename RemoveFuture<Result_t>::type> 
        Reduce(Itr_t BeginItr, Itr_t EndItr,const Op_t& Op,size_t ChunkSize=1);
      
      
      /** \brief This function performs the same function on each instance in
       *  a container
       * 
       *  Given two iterators for the range [BeginItr,EndItr) this function
       *  will call Op on each instance in the range.  Op must have
       *  the signature:
       *  \code
       *  bool Op(const Type&);
       *  \endcode
       *  where Type is the type of each object in the range.  Your function 
       *  should return a bool (true
       *  for success, false otherwise).  The resulting future holds the logical
       *  and of each returned bool (so true if they all pass, false if even one
       *  failed).
       * 
       *  Typical usage of this function:
       *  \code
       *  //Get a communicator
       *  Communicator Comm;
       * 
       *  //Get a vector of values
       *  std::vector<double> SomeValues;
       * 
       *  //Define a function that takes an iterator
       *  bool MyFxn(const std::vector<double>::iterator& value){
       *     //do something with value
       *     return true; 
       *  }
       * 
       *  //Here we use std::function to turn a function into a functor
       *  Comm.ForEach(SomeValues.begin(),SomeValues.end(),
       *      std::function<bool(double)>(MyFxn));
       *  \endcode
       * 
       *  \param[in] BeginItr An iterator pointing to the beginning of the range
       *  \param[in] EndItr An iterator pointing to the end of the range
       *  \param[in] Op The operation to be applied to each element in the range
       *  \param[in] ChunkSize How many operations each task should entail
       * 
       *  Expert note: like Reduce we allow you to put more than one operation
       *  in each task.  If your operations are short you likely will want to
       *  put more than one operation into each task.  Once you have your code
       *  working it is straightforward to run it with various chunk sizes and
       *  see what the optimal value is.  This is actually just a wrapper
       *  to reduce where Result_t is a bool.
       *
       */
      template<typename Itr_t,typename Op_t>
      Future<bool> ForEach(Itr_t BeginItr,Itr_t EndItr,const Op_t& Op,
                           size_t ChunkSize=1);
      
      /** \brief Splits the communicator
       * 
       *  I'll try to be explanative here because this can be somewhat confusing
       *  do to the various ways of thinking about how the split occurs.  This
       *  is a very powerful function.  That can do a lot of things with just
       *  three arguments.  For everything below, assume we have \f$M\f$
       *  processes and \f$N\f$ threads per process, available in this 
       *  communicator.  The result of this call will be a communicator that 
       *  when you call it's AddTask, Reduce, or ForEach methods will schedule
       *  your task on the requested resources.  We allow tasks themselves to
       *  contain parallelism, although we assume it is the same for each task.
       *  In particular we assume that each task requires \f$m\f$ processes and
       *  \f$n\f$ threads.
       * 
       *  First assume you only have one process (i.e. \f$M=1\f$), then \f$m=1\$
       *  necessarily.  If \f$N=1\f$ as well, you can only run serially and the
       *  only option available to you will be (there is a hidden third
       *  argument that will be described later):
       *  \code
       *  NewComm=OldComm.Split(1,1);
       * 
       *  //which in this case is equivalent to:
       *  NewComm=OldComm.Split();
       *  \endcode
       *  and your tasks will run serially, and life is boring.  Now we instead
       *  assume \f$N\f$ is not 1.  This means we can run up to \f$N\f$
       *  tasks on our one process, or we can run \f$N/2\f$ tasks that use two
       *  threads, \f$N/3\f$ tasks that use three threads each, or in general
       *  \f$1\le n\le N\f$  tasks.  Code:
       *  \code
       *  NewComm=OldComm.Split(n);
       *  //or if you want to explicitly insert the m=1
       *  NewComm=OldComm.Split(n,1);
       * \endcode
       * When you add tasks they will be put into a scheduler that will schedule
       * at most \f$\left\lfloor{N/n}\right\rfloor\f$ tasks.  So if \f$N=12\f$
       * and \f$n=5\f$, 2 tasks will run simultaneously.  Within each task you 
       * are then free to spawn \f$n-1\f$ more threads (you after all, are a 
       * thread yourself which counts towards the \f$n\f$ you want) or \f$n\f$ 
       * more threads if you want to maintain an asynchronous work flow.
       * 
       * The above covers the case of \f$M=1\f$.  Now we generalize it and
       * things get fun.  The simplest scenario now is \f$M>1\f$, but \f$m=1\f$,
       * that is each task only requires 1 process.  This scenario is exactly
       * the same as the previously discussed one except that instead of running
       * \f$N/n\f$ tasks, \f$M(N/n)\f$ tasks will be run.  That is we assume we
       * are using all of our available MPI processes.  For \f$m>1\f$ it only
       * makes sense to use \f$n=N\f$ because we have nested parallelism and
       * we are still at the stage of assigning MPI processes.
       * \todo come up with a better explanation why  
       * 
       * Next we have sort of fringe scenarios.  Imagine we are running
       * say a finite-difference computation of the geometric gradient of a
       * non-linear trimer.  There are 3 unique displacements and we need 6
       * geometries for the minimum central-difference computation.  If we are
       * calling an energy function that is only threaded it makes no sense
       * to use all \f$M\f$ processes (assuming \f$M>6\f$).  We thus introduce
       * our last variable: MaxProcs, which is the maximum number of processes
       * to use for scheduling tasks:
       * \code
       * NewComm=OldComm.Split(n,1,MaxProcs);
       * \endcode
       * This example starts \f$N/n\f$ tasks, per process, on at most MaxProcs
       * processors.
       * 
       * Finally, combining everything.  Say each of the six tasks can run in
       * MPI on at most 2 processes.  This can be requested by:
       * \code
       * NewComm=OldComm.Split(N,2,12);
       * \endcode
       * This will spawn 6 tasks, of 2 processes each where each task has
       * access to all of the threads on that process (n=N is required for
       * exactly the same reason as above).
       * 
       * For completeness.  If you want to force a serial section:
       * \code
       * NewComm=OldComm.Split(N,1,1);
       * \endcode
       * 
       * Got it?
       * 
       * \param[in] n The number of threads per task.  Defaults to 1, must be
       *              in range [1,N]
       * \param[in] m The number of processes per task. Defaults to 1, must be
       *               in the range [1,M].  If m>1, then n will be ignored and
       *               used as N.  You should still set n to N to future proof
       *               your call though.
       * \param[in] Maxm The maximum number of processes one can use.  Defaults
       *                 to 0, which is a special value that means use all the
       *                 processes. Must be in range [0,M]
       * 
       */
      Communicator Split(size_t n=1, 
                                          size_t m=1,
                                          size_t Maxm=0)const;

      ///The process rank (I'm letting you use this, but you probably don't
      ///need it)
      size_t Rank()const;
      
      ///The number of processes
      size_t NProcs()const;
      
      ///The number of threads
      size_t NThreads()const{return NThreads_;}
      
      ///True if the communicator is active, i.e. if you should give it tasks
      bool IsActive()const;
      
      ///Returns the details of the distrubeted parallel setup
      std::shared_ptr<const CommStats> GetStats()const{return MyStats_;}
      
      ///This function should be used sparingly, but if you need it this is the
      ///the current MPI_COMM
      MPI_Comm MPIComm()const;
      
      ///@{ MPI Wrappers
      template<typename T>
      void Send(const T& Data,size_t Recv, size_t MsgTag=0)const;
      
      template<typename T>
      void Recv(T& Data,size_t Sender,size_t MsgTag=0)const;
      
      ///Broadcasts Data from Root, Data must be serializable
      template<typename T>
      void Bcast(T& Data,size_t Root=0)const;
      
      ///Splits the MPI_Comm inside this communicator (I strongly recommend
      ///seeing if Split() works for you)
      void MPISplit(size_t Color,MPI_Comm& NewComm)const;
      ///@}
 
      ///You may not assign a communicator
      const Communicator& operator=(const Communicator&)=delete;
      ///You may not copy a communicator
      Communicator(const Communicator&)=delete;
      ///May not be default constructed
      Communicator()=delete;
      ///Comms may be moved, but please don't...
      Communicator(Communicator&&)=default;
      ///Releases the resources owned by this communicator
      ~Communicator();
      
      ///Prints the communicator as a string
      std::string ToString()const;
      
      ///Allows (mainly DVector) to grab the world
      const madness::World& World()const{return *World_;}
      
   private:
      ///The Madness world we are wrapping, represents the resources at our
      ///disposal
      std::shared_ptr<madness::World> World_;
      ///The environment we belong to, we don't own it
      Environment* Env_;
      ///The number of threads we own
      const size_t NThreads_;
      
      ///The distributed parallel nature of this communicator
      std::shared_ptr<CommStats> MyStats_;
      
      ///Was this communicator registered (needed b/c of recursion see Env)
      bool Registered_;
            
      /** \brief Keeps track of the tasks that have been added to the queue
       *
       *  This value starts at 0 and is incremented every time a task is
       *  scheduled to run.  It functions like a universal ID for a task
       *  because tasks need to be added in the same order on all processes.
       *  As far as worrying about its uniqueness, the maximum unsigned 64-bit
       *  integer is: 1.845E19.  If we submitted that many nano-second long
       *  tasks it would take 584 years to run them, so I think we are ok...
       */
      size_t TasksAdded_;     
      
      ///What number communicator am I?
      size_t MyNum_;
      
      ///True if the barrier in AddTask and Reduce is on
      bool BarrierOn_=true;

      friend Environment;

      
      /** \brief A constructor for making communicators from MPI_Comm's
       *         This is private to prevent people from going around our
       *         resource management.
       * 
       *  \param[in] NThreads The number of threads this comm owns
       *  \param[in] Comm The MPI_Comm we are building this on
       *  \param[in] Env  The environment we are part of
       *  \param[in] Control True if we are taking ownership of the MPI_Comm
       *  \param[in] Stats How processes are divided up.  Null makes it so that
       *             only threading is used.
       *  \param[in] Register Should we register this communicator with the
       *                       Env.  (Needed to prevent infinite recursion)
       */
      Communicator(size_t NThreads,const MPI_Comm& Comm, Environment* Env,
                   bool Control=false,CommStats* Stats=nullptr,
                   bool Register=false);
      ///Registers the communicator with Env_ (only call once)
      void Register();
      ///Releases the communicator from Env_ (only call once)
      void Release();
};



inline std::ostream& operator<<(std::ostream& os, const Communicator& Comm){
    return os<<Comm.ToString();
}

/* In-depth implementations*/
#include "MPIWrappers.hpp"
#include "AddTask.hpp"
#include "Reduce.hpp"



}//End namespaces



#endif /* PARALLEL_COMMUNICATOR_HPP_ */
