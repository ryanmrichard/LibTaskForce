/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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

