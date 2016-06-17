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

#include "LibTaskForce/Distributed/ProcessComm.hpp"
 

namespace LibTaskForce {

Scheduler::Scheduler(ProcessComm& Comm) :
Comm_(Comm)
{}

MPI_Comm Scheduler::mpi_comm()const
{
    return Comm_.mpi_comm();
}

size_t Scheduler::me()const
{
    return Comm_.rank();
}

bool Scheduler::my_task(size_t TaskNum) const
{
            return who_runs_task(TaskNum)==me();
}

RoundRobin::RoundRobin(ProcessComm& Comm) :
Scheduler(Comm)
{}

size_t RoundRobin::who_runs_task(size_t i) const
{
    return i%Comm_.size();
}

}//End namespace LibTaskForce
