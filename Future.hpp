#ifndef PARALLEL_FUTURE_HPP_
#define PARALLEL_FUTURE_HPP_

#include "madness.h"

namespace bpmodule{
namespace LibParallel{

/** \brief A class that holds a future result
 *
 *  When you put a task into a queue you are given a Future to that task's
 *  return type.  So if you have a task, that when given a bunch of doubles
 *  returns their sum you would get a Future<double> instance back.  The double
 *  wrapped in that instance would be the sum.  Of course, if your task hasn't
 *  run yet, then the sum is not there.  It's this class's job to determine
 *  if the result of your task is available, and if it's not, to wait for it
 *  to be available.
 *
 *  For
 *  all intents and purposes a Future<T> behaves like a unique pointer to an
 *  object of type T.  Specifically, you can get the value by using the
 *  dereference operator [operator*()] or if T is a class, you can use
 *  the class dereference operator [operator->()] to access it's members.
 *
 *  It's important to realize that once you have called either operator*()
 *  or operator->() you are stuck in that call until it can return, i.e. your
 *  task has finished.  If the task hasn't already finished, you're waiting,
 *  otherwise, if the task has already completed,
 *  then operator*() and operator->() immediately return.  Thus you should
 *  delay trying to use the wrapped object as long as possible to increase
 *  the likely hood that the object is ready for use.
 *
 *  Implementation note:
 *  Madness's Futures are not implemented like std::future.  Specifically,
 *  a future does not transfer ownership of the object upon calling get.
 *  What confuses me about this is how their copy constructor works then.
 *  So in std::future, copying is not allowed because it would be unclear
 *  who gets the result, instead you can move the return spot to another
 *  future.  Rather than try to decipher what they do, I have disabled
 *  copying/assignment, except by moving.
 *
 *
 */
template<typename T>
class Future{
   public:
      T& operator*(){return DaFuture_.get();}
      const T& operator*()const{return DaFuture_.get();}
      const T* operator->()const{return &(DaFuture_.get());}
      T* operator->(){return &(DaFuture_.get());}
      Future(Future<T>&& Other)=default;
      Future<T>& operator=(Future<T>&& Other)=default;
      Future<T>& operator=(Future<T>& Other)=delete;
      Future(Future<T>& Other)=delete;
   private:
      ///Can only be made by communicators
      friend class Communicator;
      Future(madness::Future<T>&& future):DaFuture_(future){}
      madness::Future<T>& DaFuture_;
};


}}



#endif /* PARALLEL_FUTURE_HPP_ */
