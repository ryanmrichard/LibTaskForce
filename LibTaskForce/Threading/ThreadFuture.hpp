/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file ThreadFuture.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 9, 2016
 */

#ifndef LIBTASKFORCE_GUARD_THREADFUTURE_HPP
#define LIBTASKFORCE_GUARD_THREADFUTURE_HPP

#include <future>
#include "LibTaskForce/Threading/ThreadQueue.hpp"

namespace LibTaskForce {
class ThreadQueue;


///Abstracts away the mechanism for getting a value computed with threads
template<typename ReturnT>
class ThreadFuture{
    private:
        using future_type=std::future<ReturnT>;///< Type of future 2 result
        using queue_type=ThreadQueue;///< Type of the queue
        using my_type=ThreadFuture<ReturnT>;///< The type of this class
        future_type DaFuture_;///< The result we are going to return
        queue_type& Parent_;///< The task that is waiting for this future
    public:
        
        ThreadFuture(future_type&& Future,queue_type& Parent):
            DaFuture_(std::move(Future)),Parent_(Parent)
            {}
        ~ThreadFuture()=default;
        
        ///Copy/Assignment Constructors
        /**@{*/
        ThreadFuture(my_type&&)=default;
        my_type& operator=(my_type&&)=default;
        ThreadFuture(const my_type&)=delete;
        my_type& operator=(const my_type&)=delete;
        /**@}*/
        
        ///Returns the value this future is in charge of
        ReturnT get()
        {
            Parent_.wait();
            return DaFuture_.get();
        }
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_THREADFUTURE_HPP */

