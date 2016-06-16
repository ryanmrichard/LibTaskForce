/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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