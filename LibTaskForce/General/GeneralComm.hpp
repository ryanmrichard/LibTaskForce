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

/** \file GeneralComm.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 15, 2016
 */

#ifndef LIBTASKFORCE_GUARD_GENERALCOMM_HPP
#define LIBTASKFORCE_GUARD_GENERALCOMM_HPP

#include "LibTaskForce/Util/ParallelAssert.hpp"

namespace LibTaskForce {

/** \brief Communicators are a user's gateway to their parallel resources
 * 
 *  When a user wants to run something in parallel they ask the environment
 *  what resources it has available, which it provides to the user in the
 *  form of a communicator, or comm  for short.  This comm is const and
 *  can't be used for parallel work; instead the user "splits" from the
 *  comm the resources that they want.  Each comm should provide a function
 *  split() that serves this purpose.  The comm that results from the split is
 *  able to do parallel work and the user should feed tasks to that comm.
 * 
 *  \param[in] env_type The type of the environment that manages these resources
 *  \param[in] queue_type The type of a task queue for these resources
 */
template<typename env_type,typename queue_type>
class GeneralComm{
protected:
    env_type* Env_;///<The environment associated with this comm
    std::unique_ptr<queue_type> Queue_;///<This comm's task queue
    bool Registered_=false;///< True if this comm was registered
    ///Class takes control of memory assoicated with \p Queue
    GeneralComm(env_type* Env,queue_type* Queue) :
        Env_(Env),Queue_(Queue)
    {
        PARALLEL_ASSERT(Queue_!=nullptr,"Allocation of thread queue failed");
        PARALLEL_ASSERT(Env_!=nullptr,"You need to specify a non NULL env.");
    }
public:
    ///Comms are not copyable, but they are movable
    ///@{
    GeneralComm(const GeneralComm&)=delete;
    GeneralComm& operator=(const GeneralComm& Other)=delete;
    GeneralComm(GeneralComm&&)=default;
    GeneralComm& operator=(GeneralComm&&)=default;
    ///@}
    virtual ~GeneralComm()=default;
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_GENERALCOMM_HPP */

