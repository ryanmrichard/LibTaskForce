/*  
 *   LibTaskForce: An open-source library for task-based parallelism
 * 
 *   Copyright (C) 2016 Ryan M. Richard
 * 
 *   This file is part of LibTaskForce.
 *
 *   LibTaskForce is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   LibTaskForce is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with LibTaskForce.  If not, see <http://www.gnu.org/licenses/>.
 */ 

/** \file ThreadComm.hpp
 *  \brief A communicator that handles the threading semantics
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 9, 2016
 */

#ifndef LIBTASKFORCE_GUARD_THREADCOMM_HPP
#define LIBTASKFORCE_GUARD_THREADCOMM_HPP

#include <memory>
#include "LibTaskForce/Threading/ThreadFuture.hpp"
#include "LibTaskForce/Threading/ThreadQueue.hpp"
#include "LibTaskForce/Threading/ThreadTask.hpp"
#include "LibTaskForce/General/GeneralComm.hpp"

namespace LibTaskForce {
class ThreadEnv;
class ThreadQueue;

/** \brief The user interface to thread parallelism
 * 
 *  This is built on top of TBB and we defer much of this comm's operations to
 *  that library.  In particular we do not actually register/release comms 
 *  because TBB does that for us.
 */ 
class ThreadComm:public GeneralComm<ThreadEnv,ThreadQueue>{
private:
    using base_type=GeneralComm<ThreadEnv,ThreadQueue>;
    friend ThreadEnv;///< Only Env can make comms
    ThreadComm(ThreadEnv* Env);///Makes comm with \p NThreads nthreads
public:
    ~ThreadComm();
    ThreadComm(ThreadComm&&)=default;
    ThreadComm& operator=(ThreadComm&&)=default;
    
    /** \brief Splits off a sub communicator for launching sub tasks.  
     * 
     * At the moment \p n is ignored, but is eventually envisioned as allowing
     * finer control over how many threads actually split off.  The reason it
     * is ignored is that TBB actually will play nicely with other threading
     * libraries (according to their documentation at least).  That being said
     * you still should split the comm because the split is more for task
     * dependencies.
     */
    std::unique_ptr<ThreadComm> split(size_t n=0)const;
    
    size_t size()const;///< Returns the number of threads on this Comm   
    
    /** \brief The main call for adding a task to a communicator
     * 
     *  The essence of task-based paralellism is well begin able to run tasks in
     *  parallel.  This function is the key to that.  Any callable entity given
     *  to it will be scheduled to run in parallel asynchronously.
     * 
     *  Your function/functor/lambda needs to be callable via the signature:
     *  \code
     *  //Const is only needed if this is a member function
     *  return_type operator()(ThreadComm)const;
     *  \endcode
     * 
     *  \param[in] Fxn The function that will be called to run a task.
     *  \param[in] return_type The type of the value your function returns
     *  \return A future to the result of your task
     */
    template<typename return_type,typename functor_type>
    ThreadFuture<return_type> add_task(functor_type&& Fxn)
    {
        ThreadTask<return_type,functor_type,ThreadComm> 
                Task(std::forward<functor_type>(Fxn),*this);
        return Queue_->add_task(Task);
    }
    
    /** \brief The main call for doing a reduce
     * 
     *  Note, this is not actually asynchronous at the moment because tbb does
     *  not do asynchronous reduce.
     * 
     *  Given a set of iterators to a range of the form [Begin,End) this 
     *  function will apply a functor in a manner consistent with accumulation.
     *  The result will be of type result_type.
     *
     *  Your functor must be default constructable and define:
     *  \code
     *  //Dereferences the iterator and from the resulting value return an 
     *  //object of type return_value
     *  return_value operator()(itr_type)const;//Dereference function
     *  
     *  //Combine the two input return_values and return the result
     *  return_value operator()(return_value,return_value)const;//Operation
     *  \endcode
     * 
     *  \note Your iterator does not have to dereference to return_type, but
     *        the dereference function must take whatever value the iterator
     *        does dereference to and turn it into an object of type
     *        return_type.
     * 
     *  A typical usage of this call is something like:
     *  
     *  \code
     *  const_iterator Begin,End;//Get iterators to a range from somewhere
     *  
     *  ThreadComm Comm;//Get a thread comm from somewhere
     * 
     *  Functor MyOperation;//Make a functor with above interface
     * 
     *  result_type Result=Comm.reduce<result_type>(MyOperation,Begin,End);
     *  \endcode
     * 
     * 
     *  \param[in] Fxn The operation that will be called on the range
     *  \param[in] Begin An iterator to the start of the range
     *  \param[in] End   An iterator just past the end of the range
     *  \param[in] return_type The values of objects you are returning.  Must be
     *                         default initialized with default constructor and
     *                         assignable.
     *                         Note normal types (int,double, etc.) satisfy
     *                         this criteria when invoked like double a().  In
     *                         this case a will be initialized to 0.0.
     *  \return The reduced value
     *  
     *
     */
    template<typename return_type,typename fxn_type,typename itr_type>
    return_type reduce(const fxn_type& Fxn,itr_type Begin,itr_type End){
        ReduceTask<return_type,fxn_type> Task(Fxn);
        return Queue_->reduce(Task,Begin,End);
    }
};


}//End namespace LibTaskForce
#endif /* LIBTASKFORCE_GUARD_THREADCOMM_HPP */

