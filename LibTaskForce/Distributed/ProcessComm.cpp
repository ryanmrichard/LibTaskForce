/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "LibTaskForce/Distributed/ProcessComm.hpp"
#include "LibTaskForce/Distributed/ProcessEnv.hpp"
#include "LibTaskForce/Util/ParallelAssert.hpp"
#include "ProcessQueue.hpp"

namespace LibTaskForce {

ProcessComm::ProcessComm(MPI_Comm Comm,ProcessEnv* Env) :
    Comm_(Comm),base_type(Env,new ProcessQueue(*this))
{   
}

ProcessComm::~ProcessComm()
{
  if(Registered_)Env_->release_comm(*this);  
}

void ProcessComm::barrier()const
{
    MPI_Barrier(Comm_);
}

size_t ProcessComm::size()const
{
    int DaSize;
    MPI_Comm_size(Comm_,&DaSize);
    return static_cast<size_t>(DaSize);
}

size_t ProcessComm::rank()const
{
    int DaRank;
    MPI_Comm_rank(Comm_,&DaRank);
    return static_cast<size_t>(DaRank);
}

bool ProcessComm::active()const
{
    return Active_;
}

std::unique_ptr<ProcessComm> ProcessComm::split(size_t NProcs) const
{
    if(NProcs==0)NProcs=size();
    bool Active=(rank()<NProcs);
    MPI_Comm NewMPIComm;
    MPI_Comm_split(Comm_,(int)Active,(int)rank(),&NewMPIComm);
    std::unique_ptr<ProcessComm> NewComm(new ProcessComm(NewMPIComm,Env_));
    NewComm->Active_=Active;
    Env_->register_comm(NewComm.get());
    NewComm->Registered_=true;
    return NewComm;
}


}//End namespace
