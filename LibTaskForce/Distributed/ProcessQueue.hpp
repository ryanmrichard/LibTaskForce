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

/** \file ProcessQueue.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 13, 2016
 */

#ifndef LIBTASKFORCE_GUARD_PROCESSQUEUE_HPP
#define LIBTASKFORCE_GUARD_PROCESSQUEUE_HPP

#include "LibTaskForce/Distributed/Scheduler.hpp"


namespace LibTaskForce {
class ProcessComm;

///The class in charge of storing tasks
class ProcessQueue{
private:
    size_t NTasks_;///< How many tasks have passed through me
    RoundRobin Scheduler_;
public:
    ProcessQueue(ProcessComm& Comm);
    
    template<typename return_type,typename task_type>
    ProcessFuture<return_type> add_task(const task_type& Task)
    {
        using FutureType=ProcessFuture<return_type>;
        FutureType Temp;
        if(Scheduler_.my_task(NTasks_))
            Temp=FutureType(Task(),Scheduler_.me(),&Scheduler_);
        else 
            Temp=FutureType(Scheduler_.who_runs_task(NTasks_),&Scheduler_);
        ++NTasks_;
        return Temp;
    }
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_PROCESSQUEUE_HPP */

