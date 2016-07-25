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

/** \file ThreadQueue.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 10, 2016
 */

#ifndef LIBTASKFORCE_GUARD_THREADQUEUE_HPP
#define LIBTASKFORCE_GUARD_THREADQUEUE_HPP

#include "LibTaskForce/Util/pragma.h"

PRAGMA_WARNING_PUSH
PRAGMA_WARNING_IGNORE_CONVERT
PRAGMA_WARNING_IGNORE_FP_EQUALITY
#include <tbb/tbb.h>
PRAGMA_WARNING_POP

#include<type_traits>

namespace LibTaskForce {
template<typename T> class ThreadFuture;

//TBB doesn't support move semantics yet, so fake them...
template<typename TaskType>
struct ThreadWrapper{
    const TaskType& Task_;
    
    ThreadWrapper(const TaskType& Task):
        Task_(Task){}
    
    void operator()()const
    {
        Task_();
    }
                
};

///Abstracts away the actual queue implementation
class ThreadQueue{
private:
    tbb::task_group Queue_;///< The actual queue
public:    
    ThreadQueue(size_t)
    {}
    
    template<typename TaskType>
    ThreadFuture<typename TaskType::return_type> add_task(const TaskType& Task)
    {           
        ThreadFuture<typename TaskType::return_type> 
            Fut(std::move(Task.P_->get_future()),*this);

        Queue_.run(Task);
        return Fut;
    }
    
    template<typename TaskType,typename const_iterator>
    typename TaskType::return_type reduce(TaskType& Task,
                                          const_iterator Begin,
                                          const_iterator End)
    {
        tbb::blocked_range<const_iterator> r(Begin,End);
        tbb::parallel_reduce(r,Task);
        return Task.MySum_;
    }
    
    void wait()
    {
        Queue_.wait();
    }
};

}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_THREADQUEUE_HPP */

