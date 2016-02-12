#ifndef PARALLEL_FUTURE_HPP_
#define PARALLEL_FUTURE_HPP_

#include <iostream>
#include "madness/world/MADworld.h"
#include "ParallelAssert.hpp"

namespace LibTaskForce{

/** \brief A class that holds a future result
 *
 *  When you put a task into a queue you are given a Future to that task's
 *  return instance.  So if you have a task (a functor),
 *  that when called with a bunch of doubles as arguments, returns their sum you
 *  would get a Future<double> instance back.  The double
 *  wrapped in that instance would be the sum.  Of course, if your task hasn't
 *  run then we don't know the sum yet.  It's this class's job to determine
 *  if the result of your task is available, and if it's not, to wait for it
 *  to be available.
 *
 *  For
 *  all intents and purposes a Future<T> behaves like a shared pointer to an
 *  object of type T.  Specifically, you can get the value by using the
 *  dereference operator [operator*()] or if T is a class, you can use
 *  the class dereference operator [operator->()] to access it's members.  You
 *  can make multiple Futures point to the same result using assignment or
 *  copying.  It is recommended that your returned object be light weight or
 *  a smart pointer because the future owns the memory and you'll have to copy
 *  the result out (this is a necessary complication from the nature of the
 *  underlying parallelism).
 *
 *  It's important to realize that once you have called either operator*()
 *  or operator->() you are stuck in that call until it can return, i.e. your
 *  task has finished.  If the task hasn't already finished, you're waiting,
 *  otherwise, if the task has already completed,
 *  then operator*() and operator->() immediately return.  Thus you should
 *  delay trying to use the wrapped object as long as possible to increase
 *  the likely hood that the object is ready for use.
 * 
 *  Like a shared_ptr, if you use the default constructor a valid object is
 *  created, but it's a null state and derefencing it will produce a segfault.
 *
 *  Implementation note:
 *  Madness's Futures are not implemented like std::future.  Specifically,
 *  their futures do not transfer ownership of the object upon calling get.
 *  Instead they behave like a pointer and allow the user to access the data
 *  multiple times or to set multiple Madness future instances to point to the
 *  same data.  Note that if A and B are Madness futures and B already points to
 *  a result (or the spot where a result will be), setting A=B will make A
 *  point to a result with the same value (or the spot where the result will 
 *  be).  However, if A also points to a result (or future spot) then an 
 *  exception is thrown.  I find this odd behavior and instead have made
 *  futures behave like shared_ptrs.
 * 
 *
 *
 */
template<typename T>
class Future{
   private:
       ///Typedef of Madness's future
       typedef madness::Future<T> mFuture_t;
       ///Typedef of a shared_ptr to a Madness future
       typedef std::shared_ptr<mFuture_t> SharedFuture_t;
   public:
      ///Returns a reference to the computed value
      T& operator*(){
          PARALLEL_ASSERT(DaFuture_,"This future was never set");
          return DaFuture_->get();
      }
      ///Returns a const reference to the computed value
      const T& operator*()const{
          PARALLEL_ASSERT(DaFuture_,"This future was never set");
          return DaFuture_->get();
      }
      
      ///Returns a pointer to a const version of your object
      const T* operator->()const{
          PARALLEL_ASSERT(DaFuture_,"This future was never set");
          return &(DaFuture_->get());
      }
      
      ///Returns a pointer to your object
      T* operator->(){return &(DaFuture_->get());}
      
      ///Makes a NULL future
      Future()=default;
      
      ///Shallow copy of other
      Future<T>& operator=(const Future<T>&)=default;
      
      ///Shallow copy of other
      Future(const Future<T>&)=default;
      
      ///Prints out some basic status information for this Future
      std::string ToString()const{
          std::stringstream ss;
          ss<<"Is set: "<<(DaFuture_->probe()? "TRUE" : "FALSE")<<std::endl;
          ss<<"Is local: "<<(DaFuture_->is_local()? "TRUE" : "FALSE");
          return ss.str();
      }
   private:
      ///Valid futures can only be made by communicators
      friend class Communicator;
      ///The constructor makes a Future that actually does stuff
      Future(const mFuture_t& future):DaFuture_(new mFuture_t(future)){}
      ///The constructor for setting the value immediatly
      Future(const T& Value):DaFuture_(new mFuture_t(Value)){}
      ///The implementation of this future
      SharedFuture_t DaFuture_;
};

///Allows futures to be printed out
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Future<T>& Ftr){
    return os<<Ftr.ToString();
    
}


}//End namespace
#endif /* PARALLEL_FUTURE_HPP_ */
