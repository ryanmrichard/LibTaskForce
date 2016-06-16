/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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
    
    ///Makes a new task that will be run in hybrid parallelism
    HybridTask(HybridComm& Comm,functor_type Fxn) : 
      MyComm_(Comm),Fxn_(Fxn)
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

