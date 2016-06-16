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

/** \file GeneralEnv.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 14, 2016
 */

#ifndef LIBTASKFORCE_GUARD_GENERALENV_HPP
#define LIBTASKFORCE_GUARD_GENERALENV_HPP

#include <stack>
#include <memory>
#include "LibTaskForce/Util/ParallelAssert.hpp"

namespace LibTaskForce {

/** \brief Environments are in charge of managing parallel resources
 * 
 *   The current class is code factorization and interface specification for
 *   all environments.  Environments are in charge of handing out parallel
 *   resources (threads, processes, or both).  Users get access to these
 *   resources via communicators.  Ultimately all environments derive from
 *   this class, but note that all of them use different communicators and hence
 *   do not share a common base class.  Again, this class is meant to provide 
 *   code factorization and not polymorphism.
 *  
 *   The decision to make register/release comm protected stems from the fact
 *   that we do not want users doing this on their own, but rather want this
 *   to go down in a controlled manner dictated by the comms themselves.
 * 
 *   \param[in] comm_type The type of communicator associated with the resources
 */
template<typename comm_type>
class GeneralEnv {
protected:
    ///The comm from which all other comms originate
    std::unique_ptr<comm_type> FirstComm_;
    
    ///A stack of comms actively doing parallel stuff
    std::stack<const comm_type*> Comms_;
    
    ///Adds a communicator to the stack.  Afterwards this is the comm users get
    void register_comm(const comm_type* Comm2Register)
    {
        Comms_.push(Comm2Register);
    }
    
    ///Signals you are done with a communicator.  In debug mode checks for leaks
    void release_comm(const comm_type& Comm)
    {
        PARALLEL_ASSERT(!Comms_.empty(),"No Comms left!!!!");
        PARALLEL_ASSERT(&Comm==Comms_.top(),"Comm leak detected");
        Comms_.pop();
    }
    
public:
    
    ///Returns the active communicator
    const comm_type& comm()const
    {
        return (Comms_.empty()? *FirstComm_:*Comms_.top());
    }
    
    virtual ~GeneralEnv()=default;
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_GENERALENV_HPP */

