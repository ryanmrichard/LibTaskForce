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
#include "LibTaskForce/Threading/ThreadComm.hpp"
#include "LibTaskForce/Threading/ThreadEnv.hpp"
#include "LibTaskForce/Util/ParallelAssert.hpp"

namespace LibTaskForce{

ThreadComm::ThreadComm(ThreadEnv* Env):
    base_type(Env,new ThreadQueue(Env->size()))
{
}

ThreadComm::~ThreadComm()
{
    if(Registered_)Env_->release_comm(*this);
}

size_t ThreadComm::size()const
{
    return Env_->size();
}

std::unique_ptr<ThreadComm> ThreadComm::split(size_t)const{
    
    std::unique_ptr<ThreadComm> NewComm(new ThreadComm(Env_));
    //Env_->register_comm(NewComm.get());
    //NewComm->Registered_=true;
    return NewComm;
}

}//End namespace