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

      ///Adds a task to the queue and then gives you a future to the result
      template<typename T,typename T1, typename T2>
      Future<typename std::result_of<T(T1,T2)>::type>&&
      AddTask(T YourTask,T1 arg1, T2 arg2){
         Future<typename std::result_of<T(T1,T2)>::type>
                (World_.taskq.add(YourTask,arg1,arg2));
      }

   private:
      friend class Environment;
      ///Can only be made by Environment or other Communicators
      Communicator(madness::World& world);
      madness::World& World_;

};


}}//End namespaces



#endif /* PARALLEL_COMMUNICATOR_HPP_ */
