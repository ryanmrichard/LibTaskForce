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
