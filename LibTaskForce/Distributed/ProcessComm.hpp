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

/** \file ProcessComm.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 13, 2016
 */

#ifndef LIBTASKFORCE_GUARD_PROCESSCOMM_HPP
#define LIBTASKFORCE_GUARD_PROCESSCOMM_HPP

#include <mpi.h>
#include "LibTaskForce/Util/ParallelAssert.hpp"
#include "LibTaskForce/Util/Serialization.hpp"
#include "LibTaskForce/Distributed/ProcessFuture.hpp"
#include "LibTaskForce/Distributed/ProcessQueue.hpp"
#include "LibTaskForce/Distributed/ProcessTask.hpp"
#include "LibTaskForce/General/GeneralComm.hpp"

namespace LibTaskForce {
class ProcessEnv;

/** \brief The main interface to distributed parallelism
 *
 *   Where TBB took care of much of our threading details we have no such
 *   luxury with MPI.  Hence, we try to make this interface parallel to that
 *   of ThreadComm, but it has some additional features related to MPI functions
 *   we'll need (send/recv, broadcast, gather).  This is pretty similar to 
 *   Boost MPI's wrapping, but without the Boost dependency.  OpenMPI provides
 *   a similar C++ syntax as well, but I'm under the impression this is only
 *   for OpenMPI and not for other MPI flavors.
 * 
 *   Again I'm trying to keep asynchronous parallelism on the table.  To do this
 *   with MPI it's a little bit different.  Processes don't really run in the
 *   background the way threads do.  We thus split the processes into two groups
 *   the ones working on a task and those who aren't.  The ones who are active
 *   start working while those who aren't just return futures.
 * 
 *   \code
 *   ProcessComm NewComm=OldComm.split(2);
 *   //Two processes sit here running ATask
 *   ProcessFuture Result1=NewComm.add_task(ATask);
 *   ProcessFuture Result2=NewComm.add_task(ATask);
 * 
 *   //Others have fallen through
 *   if(!NewComm.active()){
 *      //TODO: How do other processes do stuff?
 *   }
 *   \endcode
 *   
 *
 */
class ProcessComm : public GeneralComm<ProcessEnv,ProcessQueue>{
private:
    using base_type=GeneralComm<ProcessEnv,ProcessQueue>;
    MPI_Comm Comm_;///<The MPI communicator we are wrapping
    bool Active_;///<True if in on the action
    friend class ProcessEnv;///<Allows Env to make the initial comm
    ProcessComm(MPI_Comm Comm,ProcessEnv* Env);
public:
    ProcessComm(ProcessComm&&)=default;
    ProcessComm& operator=(ProcessComm&&)=default;
    ~ProcessComm();///<Gives resources back.
    
    
    MPI_Comm mpi_comm()const{return Comm_;}///<Returns MPI comm for interfacing
    void barrier()const;///<Waits for all processes to get to this call
    size_t size()const;///<The number of processors on this comm
    size_t rank()const;///<What MPI rank this process is on this comm
    bool active()const;///<True if our comm is in on the MPI action
    
    
    /** \brief Splits the communicator to allow for actual parallelism to be
     *         done.
     * 
     *  The ProcessEnv returns a const communicator, which you can't do anything
     *  with until you split it (think of it as allocating the resources).  
     *  Upon "allocation" you now have a communicator that can do stuff.  This
     *  communicator has however many processes you requested and the remaining
     *  processes are available for doing other work.
     * 
     *  For example, say we have 4 processes to begin with.  You want 2.  You
     *  would then do:
     *  \code
     *  ProcessComm NewComm=OldComm.split(2);
     *  \endcode
     * 
     *  At this point you then get a new communicator back.  On two of the four
     *  processes NewComm.active() will be false (signaling to you that these
     *  are not the processes you have been assigned) and on the other two that
     *  call will be true (these are your processes)
     * 
     *  \param[in] NProcs The number of processes you want.  Default is 0, which
     *                    is a special value indicating all available processes
     * 
     *  \note Somewhat paradoxically if you want to run serially you need to
     *        request all of the processes (and then not use any MPI calls).
     *  
     */
    std::unique_ptr<ProcessComm> split(size_t NProcs=0)const;
    
    
    
    ///Allows you to add tasks one at a time (useful if tasks are all different)
    template<typename return_type,typename functor_type>
    ProcessFuture<return_type> add_task(const functor_type& Fxn)
    {
        ProcessTask<return_type,functor_type,ProcessComm> Task(Fxn,*this);
        return Queue_->add_task<return_type>(Task);
    }
    
    /** \brief More efficient way of adding many tasks at once
     * 
     *  
     *
    template<typename return_type,typename iterator_type>
    ProcessFuture<std::vector<return_type>> add_tasks(iterator_type Begin,iterator_type End)
    {
        return Queue_->add_tasks<return_type>(Begin,End);
    }*/
};



}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_PROCESSCOMM_HPP */

