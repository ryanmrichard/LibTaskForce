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

#include "LibTaskForce/Hybrid/HybridComm.hpp"
#include "LibTaskForce/Hybrid/HybridEnv.hpp"
#include "LibTaskForce/Threading/ThreadEnv.hpp"
#include "LibTaskForce/Distributed/ProcessEnv.hpp"
#include "LibTaskForce/Hybrid/HybridQueue.hpp"
#include "LibTaskForce/General/GeneralEnv.hpp"

namespace LibTaskForce {

const ProcessComm& HybridComm::ActiveProcess()const
{
    return ProcessComm_?*ProcessComm_:Env_->ProcessEnv_->comm();
}

const ThreadComm& HybridComm::ActiveThread()const
{
    return ThreadComm_?*ThreadComm_:Env_->ThreadEnv_->comm();
}

HybridComm::HybridComm(HybridEnv* Env):
base_type(Env,new HybridQueue())
{    
}

HybridComm::~HybridComm()
{
    if(Registered_)Env_->release_comm(*this);
}

bool HybridComm::UseThreads()const
{
    return nprocs()==1;
}

MPI_Comm HybridComm::mpi_comm() const
{
    return ActiveProcess().mpi_comm();
}

size_t HybridComm::rank() const
{
    return ActiveProcess().rank();
}

size_t HybridComm::size() const
{
    return UseThreads()? nthreads():nprocs();
}

size_t HybridComm::nthreads()const
{
    return ActiveThread().size();
}

size_t HybridComm::nprocs()const
{
    return ActiveProcess().size();
}

void HybridComm::barrier()const
{
    ProcessComm_->barrier();
}


std::unique_ptr<HybridComm> HybridComm::split(size_t NProcs, size_t NThreads) const
{
    if(!NProcs)NProcs=nprocs();
    if(!NThreads)NThreads=nthreads();
    
    std::unique_ptr<HybridComm> NewComm(new HybridComm(Env_));
    NewComm->ProcessComm_=ActiveProcess().split(NProcs);
    NewComm->ThreadComm_=ActiveThread().split(NThreads);
    Env_->register_comm(NewComm.get());
    NewComm->Registered_=true;
    return NewComm;
}

std::ostream& operator<<(std::ostream& os, const HybridComm& comm){
    os<<"Process "<<comm.rank()<<"/"<<comm.nprocs()<<std::endl;
    os<<"Threads "<<comm.nthreads()<<std::endl;
    return os;
}

}//End namespace
