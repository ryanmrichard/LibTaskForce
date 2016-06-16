/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "LibTaskForce/Distributed/ProcessComm.hpp"
#include "LibTaskForce/Distributed/ProcessQueue.hpp"


namespace LibTaskForce{

bool Scheduler::my_task(size_t TaskNum, const ProcessComm& Comm) const
{
            return who_runs_task(TaskNum,Comm)==Comm.rank();
}

size_t RoundRobin::who_runs_task(size_t i, const ProcessComm& Comm) const
{
    return i%Comm.size();
}

ProcessQueue::ProcessQueue(ProcessComm& Comm):
        NTasks_(0),Comm_(Comm)
{}

}//end namespace
