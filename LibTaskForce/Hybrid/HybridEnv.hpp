/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file HybridEnv.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 14, 2016
 */

#ifndef LIBTASKFORCE_GUARD_HYBRIDENV_HPP
#define LIBTASKFORCE_GUARD_HYBRIDENV_HPP

#include<mpi.h>
#include<memory>
#include "LibTaskForce/General/GeneralEnv.hpp"
#include "LibTaskForce/Hybrid/HybridComm.hpp"

namespace LibTaskForce {
class ThreadEnv;
class ProcessEnv;
class HybridComm;

///Basically a wrapper around a thread and a process environment
class HybridEnv : public GeneralEnv<HybridComm> {
private:
    friend HybridComm;///<Allows HybridComms to register themselves
    std::unique_ptr<ThreadEnv> ThreadEnv_;///<The threads we own
    std::unique_ptr<ProcessEnv> ProcessEnv_;///<The processes we own
public:
    ///For creating a new MPI environment
    HybridEnv(size_t NThreads=0);
        
    ///Latches onto existing MPI environment
    HybridEnv(MPI_Comm Comm,size_t NThreads=0);
    
};

}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_HYBRIDENV_HPP */

