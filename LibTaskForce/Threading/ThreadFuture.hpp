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

