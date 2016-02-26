#ifndef PARALLEL_FUTURE_HPP_
#define PARALLEL_FUTURE_HPP_

#include <iostream>
#include <memory>
#include "madness/world/MADworld.h"
#include "ParallelAssert.hpp"
#include "DVector.hpp"

namespace LibTaskForce{
    
    ///Forward declare for the following helper classes
    template<typename T> class Future;
    
    /* Following trick stolen from Madness*/
    
    ///Removes the future from type T    
    template<typename T>
    struct RemoveFuture{
        typedef T type;
    };
    
    ///Partial specialization to a Future, this actually removes the Future
    template<typename T>
    struct RemoveFuture<Future<T>>{
        typedef T type;
    };
    
    template<typename T>
    struct IsFuture: public std::false_type{};
    
    template<typename T>
    struct IsFuture<Future<T>>: public std::true_type{};
    
    template<typename T,typename U>
    T GetValue(U& value){return value;}
    
    template<typename T>
    T GetValue(Future<T>& value){return *value;}
    
    template<typename T>
    T GetValue(madness::Future<Future<T>>& value){return *(value.get());}
    
    //These let us make madness's futures
    template<typename T,typename U>
    madness::Future<T> ToMadFuture(U& value){return madness::Future<T>(value);}
    
    //This one should never be called because it breaks asynch, but it is 
    //needed for compilation to happen (see AddTask e.g.)
    template<typename T>
    madness::Future<T> ToMadFuture(madness::Future<Future<T>>& value){
        return madness::Future<T>(GetValue<T>(value.get()));
    }
    
    //The null scenario
    template<typename T>
    madness::Future<T> ToMadFuture(madness::Future<T>& value){return value;}

/** \brief A class that holds a future result
 *
 *  When you put a task into a queue you are given a Future to that task's
 *  return instance.  So if you have a task (a functor),
 *  that when called with a bunch of doubles as arguments, returns their sum you
 *  would get a Future<double> instance back.  The double
 *  wrapped in that instance would be the sum.  Of course, if your task hasn't
 *  run then we don't know the sum yet.  It's this class's job to determine
 *  if the result of your task are available, and if it's not, to wait for it
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
 *  created, but it's a null state and derefencing it would produce a segfault,
 *  but I throw instead.  Note, it really doesn't make sense for you the user
 *  to be making non-NULL futures because they are so intimately tied to their
 *  corresponding communicator, this is why the constructors that make valid
 *  Future's are private.
 */
template<typename T>
class Future{
   public:
      ///Returns a copy of the computed value, communicating if necessary
      T operator*(){
          PARALLEL_ASSERT(static_cast<bool>(DaFuture_),
           "This is not a valid future possibly because it was"
           " default constructed");
          return DaFuture_->Get(TaskNum_);
      }
      
      ///Makes a NULL future, can't be used aside from a place-holder
      Future()=default;
      
      ///Shallow copy of other 
      Future<T>& operator=(const Future<T>& RHS)=default;
      
      ///Shallow copy of other
      Future(const Future<T>&)=default;
      
      ///Prints out some basic status information for this Future
      std::string ToString()const{
          std::stringstream ss;
          ss<<"I am responsible for the result of task: "<<TaskNum_
            <<" the result of which is"<<(IsLocal()?"":" not")
            <<" stored locally";
          return ss.str();
      }
      
      ///Returns true if this future's data is stored locally
      bool IsLocal()const{return IsLocal_;}
      
      ///Returns the task number
      size_t TaskNum()const{return TaskNum_;}
      
      ///For compatability with Madness (it does nothing)
      template<typename Archive_t>
      void serialize(const Archive_t&)const{}
      
   private:
      
      ///Typedef of Madness's future, which is the work horse of this class
      typedef madness::Future<T> mFuture_t;
      
      ///Valid futures can only be made by communicators
      friend class Communicator;
      
      ///The task number that generated us (a sort of universal ID)
      size_t TaskNum_=0;
      
      ///Basically we abuse the remote element access
      std::shared_ptr<DVector<T>> DaFuture_;
     
      ///True if the data is local
      bool IsLocal_=false;
      
      ///The constructor for making a future to a remote task
      Future(std::shared_ptr<const CommStats> Stats,size_t TaskNum):
        TaskNum_(TaskNum),DaFuture_(new DVector<T>(Stats->World(),Stats)),
        IsLocal_(Stats->MyTask(TaskNum)){}
      
      ///The constructor makes a Future that actually does stuff
      Future(std::shared_ptr<const CommStats> Stats, size_t TaskNum,
            const mFuture_t& future):Future(Stats,TaskNum){
        DaFuture_->Set(TaskNum_,future);
      }

      ///The constructor for setting the value immediatly
      Future(std::shared_ptr<const CommStats> Stats, size_t TaskNum,
              const T& Value):Future(Stats,TaskNum){
          mFuture_t Temp(Value);
          DaFuture_->Set(TaskNum_,Temp);
      }

};

///Allows futures to be printed out
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Future<T>& Ftr){
    return os<<Ftr.ToString();
    
}


}//End namespace
#endif /* PARALLEL_FUTURE_HPP_ */
