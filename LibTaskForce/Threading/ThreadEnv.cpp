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

#include <tbb/task_scheduler_init.h>
#include "LibTaskForce/Threading/ThreadEnv.hpp"
#include "LibTaskForce/Threading/ThreadComm.hpp"

inline size_t GetNThreads(size_t NThreads)
{
    return !NThreads?tbb::task_scheduler_init::default_num_threads():NThreads;
}

namespace LibTaskForce{

ThreadEnv::ThreadEnv(size_t NThreads):
    NThreads_(GetNThreads(NThreads)),
    TaskScheduler_(new tbb::task_scheduler_init((int)NThreads_))
{
    FirstComm_=std::unique_ptr<ThreadComm>(new ThreadComm(this));
}

//Need to 
ThreadEnv::~ThreadEnv(){
        while(!Comms_.empty())Comms_.pop();
        FirstComm_.reset();
        TaskScheduler_.reset();
    }


}//end namespace