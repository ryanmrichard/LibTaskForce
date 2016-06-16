/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "LibTaskForce/Hybrid/HybridEnv.hpp"
#include "LibTaskForce/Threading/ThreadEnv.hpp"
#include "LibTaskForce/Distributed/ProcessEnv.hpp"

namespace LibTaskForce{

HybridEnv::HybridEnv(size_t NThreads):
    HybridEnv(MPI_COMM_WORLD,NThreads)
    {}


HybridEnv::HybridEnv(MPI_Comm Comm,size_t NThreads) :
    ThreadEnv_(new ThreadEnv(NThreads)), 
    ProcessEnv_(new ProcessEnv(Comm))
{
    FirstComm_=std::unique_ptr<HybridComm>(new HybridComm(this));
}



}//End namespace
