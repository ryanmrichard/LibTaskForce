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

/** \file HybridTask.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 15, 2016
 */

#ifndef LIBTASKFORCE_GUARD_HYBRIDTASK_HPP
#define LIBTASKFORCE_GUARD_HYBRIDTASK_HPP

namespace LibTaskForce {
class ThreadComm;
class ProcessComm;
class HybridComm;

///This class is basically a redirect for when a task gets put into
///a lower order comm
template<typename return_type,typename functor_type>
struct HybridTask {
    HybridComm& MyComm_;
    functor_type Fxn_;
    
    ///Makes a new task that will be run in hybrid parallelism via copy
    HybridTask(HybridComm& Comm,const functor_type& Fxn) : 
      MyComm_(Comm),Fxn_(Fxn)
    {}
    
    ///Makes a new task via move semantics
    HybridTask(HybridComm& Comm,functor_type&& Fxn):
       MyComm_(Comm),Fxn_(std::move(Fxn))
    {}
    
    ///Intercepts calls if we are running threaded
    return_type operator()(ThreadComm&)const{
        return Fxn_(MyComm_);
    }
    
    ///Intercepts calls if we are running distributed
    return_type operator()(ProcessComm&)const{
        return Fxn_(MyComm_);
    }
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_HYBRIDTASK_HPP */

