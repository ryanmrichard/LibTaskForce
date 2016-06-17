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
#include <cstdlib>
#include "LibTaskForce/Distributed/ProcessEnv.hpp"
#include "LibTaskForce/Distributed/ProcessComm.hpp"
#include "LibTaskForce/Util/ParallelAssert.hpp"

namespace LibTaskForce{  
    
ProcessEnv::ProcessEnv(MPI_Comm Comm):
    WeStartedMPI_(false){
    int Temp;
    MPI_Initialized(&Temp);
    WeStartedMPI_=!(static_cast<bool>(Temp));
    if(WeStartedMPI_)MPI_Init(nullptr,nullptr);
    FirstComm_=std::unique_ptr<ProcessComm>(new ProcessComm(Comm,this));
}


ProcessEnv::~ProcessEnv(){
    if(!WeStartedMPI_)return;
    int AlreadyFinalized;
    MPI_Finalized(&AlreadyFinalized);
    if(!AlreadyFinalized)MPI_Finalize();
}

std::string ProcessEnv::print()const
{
    std::stringstream ss;
    ss<<"Environment has "<<Comms_.size()<<" nested comms at the moment"
      <<std::endl;
    return ss.str();
}

}//End namespaces

