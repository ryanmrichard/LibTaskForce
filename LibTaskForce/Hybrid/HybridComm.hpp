/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file HybridComm.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 14, 2016
 */

#ifndef LIBTASKFORCE_GUARD_HYBRIDCOMM_HPP
#define LIBTASKFORCE_GUARD_HYBRIDCOMM_HPP

#include <memory>
#include "LibTaskForce/Threading/ThreadComm.hpp"
#include "LibTaskForce/Distributed/ProcessComm.hpp"
#include "LibTaskForce/General/GeneralComm.hpp"
#include "LibTaskForce/Hybrid/HybridFuture.hpp"
#include "LibTaskForce/Hybrid/HybridTask.hpp"

namespace LibTaskForce {
class HybridEnv;
class HybridQueue;

/** \brief The interface to a communicator that can switch back and forth
 *         between processes and threads
 *
 *  One gotcha with this class is that ProcessComm_ and ThreadComm_ start
 *  NULL.  This is because the respective environments own the memory.  In
 *  this case we need to ask the Env_ for the active communicator.  Once a
 *  sub HybridComm is made this is no longer the case.
 */
class HybridComm : public GeneralComm<HybridEnv,HybridQueue> {
private:
    using base_type=GeneralComm<HybridEnv,HybridQueue>;
    friend HybridEnv;///<Allows environment to make comms
    std::unique_ptr<ProcessComm> ProcessComm_;
    std::unique_ptr<ThreadComm> ThreadComm_;
    const ProcessComm& ActiveProcess()const;///<Returns the active ProcessComm
    const ThreadComm& ActiveThread()const;///<Returns the active ThreadComm
    HybridComm(HybridEnv* Env);
    bool UseThreads()const;///< True if we should switch to threads
public:
    ~HybridComm();
    HybridComm(HybridComm&&)=default;
    HybridComm& operator=(HybridComm&&)=default;
    
    std::unique_ptr<HybridComm> split(size_t NProcs=0,size_t NThreads=0)const;
    
    ///Access to MPI info, should only be needed for debugging or interfacing
    ///@{
    MPI_Comm mpi_comm()const;
    size_t rank()const;
    ///@}
    
    /** \brief Returns the number of active parallel resources
     *
     *   Consider a loop over tasks. One of the main goals of this library is
     *   to allow this loop to be written the same way regardless of whether
     *   threads or processes are being used to parallelize that loop.  This
     *   function will return the number of processes if processes are being
     *   used to parallelize it or the number of threads if threads are.
     */
    size_t size()const;
    size_t nthreads()const;///< Returns the number of threads
    size_t nprocs()const;///< Returns the number of processes
    
    
    template<typename return_type,typename functor_type>
    HybridFuture<return_type> add_task(const functor_type& Fxn)
    {
        HybridTask<return_type,functor_type> Task(*this,Fxn);

        using process_ptr= typename HybridFuture<return_type>::process_ptr;
        using thread_ptr= typename HybridFuture<return_type>::thread_ptr;
        thread_ptr TF(UseThreads()?new ThreadFuture<return_type>(
                     std::move(ThreadComm_->add_task<return_type>(Task)))
                     : nullptr);
        process_ptr PF(!UseThreads()?new ProcessFuture<return_type>(
                    std::move(ProcessComm_->add_task<return_type>(Task)))
                    :nullptr);
        return HybridFuture<return_type>(std::move(PF),std::move(TF));
    }
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_HYBRIDCOMM_HPP */

