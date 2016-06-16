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

