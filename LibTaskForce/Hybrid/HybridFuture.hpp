/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file HybridFuture.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 15, 2016
 */

#ifndef LIBTASKFORCE_GUARD_HYBRIDFUTURE_HPP
#define LIBTASKFORCE_GUARD_HYBRIDFUTURE_HPP

#include "LibTaskForce/Threading/ThreadFuture.hpp"
#include "LibTaskForce/Distributed/ProcessFuture.hpp"

namespace LibTaskForce {


/** \brief A future to a result computed using hybrid parallelism
 *
 *   Ulitmately at the end of the day the task was either given to
 *   a thread scheduler or a process scheduler, that scheduler then
 *   returned a future.  Alls we have to do is make sure we dereference
 *   the correct future.
 */
template<typename T>
class HybridFuture {
    using thread_ptr=std::unique_ptr<ThreadFuture<T>>;
    using process_ptr=std::unique_ptr<ProcessFuture<T>>;
    thread_ptr TF_;
    process_ptr PF_;
    friend class HybridComm;
    HybridFuture(process_ptr PF,thread_ptr TF) :
        TF_(std::move(TF)),PF_(std::move(PF))
    {}
public:
    HybridFuture()=default;
    ///Returns the object this future holds, blocks if not available
    T get()
    {
        if(PF_)return PF_->get();
        return TF_->get();
    }
        
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_HYBRIDFUTURE_HPP */

