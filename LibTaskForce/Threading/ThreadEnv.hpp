/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file ThreadEnv.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 9, 2016
 */

#ifndef LIBTASKFORCE_GUARD_THREADENV_HPP
#define LIBTASKFORCE_GUARD_THREADENV_HPP

#include<memory>
#include "LibTaskForce/General/GeneralEnv.hpp"

namespace tbb{
class task_scheduler_init;
}

namespace LibTaskForce {
class ThreadComm;


/** \brief This class is in charge of managing the threading environment
 *
 *  With threads the main resource to manage is well the number of threads.
 *  This class does that.  At the moment we are using TBB, which allows us
 *  to control the number of threads by making a tbb::task_scheduler_init
 *  instance.  This class is designed to allow for multiple instances
 *  although doing so is probably not a great idea.
 */
class ThreadEnv: public GeneralEnv<ThreadComm>{
private:
    size_t NThreads_;//< The number of threads we have
    ///TBB's task scheduler
    std::unique_ptr<tbb::task_scheduler_init> TaskScheduler_;
    friend ThreadComm;    
public:
    /** \brief Starts the threading environment up
     *
     *  \param[in] NThreads The number of threads this env can use.  Default is
     *                      1.  You may specify 0 to have  TBB choose for you.
     */
    ThreadEnv(size_t NThreads=1);
    
    ///Need to manually free pointers in right order or we get a TBB warning
    ~ThreadEnv();
    
    size_t size()const{return NThreads_;}
    
    ///Copy/Assignment Constructors
    /**@{*/
    ThreadEnv(const ThreadEnv&)=delete;
    ThreadEnv(ThreadEnv&&)=default;
    ThreadEnv& operator=(const ThreadEnv&)=delete;
    ThreadEnv& operator=(ThreadEnv&&)=default;
    /**@}*/
};

}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_THREADENV_HPP */

