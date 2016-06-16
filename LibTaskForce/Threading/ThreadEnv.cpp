/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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