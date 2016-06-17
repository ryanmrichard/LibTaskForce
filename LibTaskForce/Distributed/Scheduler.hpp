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

/** \file Scheduler.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 17, 2016
 */

#ifndef LIBTASKFORCE_GUARD_SCHEDULER_HPP
#define LIBTASKFORCE_GUARD_SCHEDULER_HPP

namespace LibTaskForce {
class ProcessComm;

/** \brief Little wrapper to abstract away who gets what task
 * 
 *  This class started as just a wrapper around the scheduling algorithm,
 *  but then became essential to breaking the cyclic dependency of the
 *  ProcessComm, ProcessFuture, and ProcessQueue.  This is why it forwards
 *  many of the Comm's members.  Admittedly this is indicative of a poor
 *  code design, but I'm having trouble seeing a better solution.
 */
struct Scheduler{
    ProcessComm& Comm_;
    Scheduler(ProcessComm& Comm);
    MPI_Comm mpi_comm()const;
    size_t me()const;
    ///True if this is my task to run
    bool my_task(size_t TaskNum)const;
    
    ///Should be made to return the rank of the process who runs task i
    virtual size_t who_runs_task(size_t i)const=0;
    virtual ~Scheduler()=default;///<No clean-up
};

///Scheduler that assigns tasks in round-robin like style
struct RoundRobin:public Scheduler{
    RoundRobin(ProcessComm& Comm);
    size_t who_runs_task(size_t i)const;
};

}//End namespace LibTaskForce
#endif /* LIBTASKFORCE_GHUARD_SCHEDULER_HPP */

