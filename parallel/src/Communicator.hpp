#ifndef PARALLEL_COMMUNICATOR_HPP_
#define PARALLEL_COMMUNICATOR_HPP_

#include <type_traits>
#include "Future.hpp"

namespace bpmodule{
namespace LibParallel{

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
 *   The task queue is also capable of for each and reduction.  To do the
 *   latter two we need to create a range object.  A range object is just
 *   a data set to be iterated over.  The reduce function
 *   takes a functor that is default constructable, copyable, and contains
 *   three members, one that takes a single iterator (to dereference
 *   the iterator and return an appropriate value), one that takes
 *   input values of the intended reduction (a lhs and a rhs),
 *   and a serialize function (this only needs to work for task stealing).
 *   The foreach construct is similar except it only has to be copyable
 *   and take a single iterator (the value it should work on) and should
 *   return true or false, the foreach call will return the logical and
 *   of all of these calls.  Together these three operations allow you to
 *   add arbitrary tasks, a series of related tasks, and a series of related
 *   tasks that require simple post processing.
 *
 *
 */
class Communicator{
   public:
      ///Causes all members of the current communicator to wait at this call
      ///until all other members arrive
      void Barrier()const;

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
       */
      template<typename Fxn_t,typename...Args>
      Future<typename std::result_of<Fxn_t(Args...)>::type>
      AddTask(Fxn_t YourTask,Args... args){
         return Future<typename std::result_of<Fxn_t(Args...)>::type>
                (World_.taskq.add(YourTask,args...));
      }

   private:
      friend class Environment;
      ///Can only be made by Environment or other Communicators
      Communicator(madness::World& world);
      madness::World& World_;

};


}}//End namespaces



#endif /* PARALLEL_COMMUNICATOR_HPP_ */
