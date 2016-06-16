/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file ProcessQueue.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 13, 2016
 */

#ifndef LIBTASKFORCE_GUARD_PROCESSQUEUE_HPP
#define LIBTASKFORCE_GUARD_PROCESSQUEUE_HPP

#include "LibTaskForce/Distributed/ProcessFuture.hpp"

namespace LibTaskForce {
class ProcessComm;

///Little wrapper to abstract away who gets what task
struct Scheduler{
    ///True if this is my task to run
    bool my_task(size_t TaskNum,const ProcessComm& Comm)const;
    
    ///Should be made to return the rank of the process who runs task i
    virtual size_t who_runs_task(size_t i,const ProcessComm& Comm)const=0;
    virtual ~Scheduler()=default;///<No clean-up
};

///Scheduler that assigns tasks in round-robin like style
struct RoundRobin:public Scheduler{
    size_t who_runs_task(size_t i,const ProcessComm& Comm)const;
};

///The class in charge of storing tasks
class ProcessQueue{
private:
    size_t NTasks_;///< How many tasks have passed through me
    ProcessComm& Comm_;
    RoundRobin Scheduler_;
public:
    ProcessQueue(ProcessComm& Comm);
    
    template<typename return_type,typename functor_type>
    ProcessFuture<return_type> add_task(const functor_type& Fxn)
    {
        using FutureType=ProcessFuture<return_type>;
        FutureType Temp;
        if(Scheduler_.my_task(NTasks_,Comm_))
            Temp=FutureType(Fxn(Comm_),Comm_.rank(),&Comm_);
        else 
            Temp=FutureType(Scheduler_.who_runs_task(NTasks_,Comm_),&Comm_);
        ++NTasks_;
        return Temp;
    }
    
    /*template<typename return_type,typename iterator_type>
    ProcessArray<return_type> add_tasks(iterator_type Begin,iterator_type End)
    {
        ProcessArray<return_type> Array(Comm_);
        while(Begin!=End){
            if(Scheduler_->my_task(NTasks_,Comm_))
                Array.push_back(Fxn(),NTasks_);
            ++Begin;
        }
    }*/
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_PROCESSQUEUE_HPP */

