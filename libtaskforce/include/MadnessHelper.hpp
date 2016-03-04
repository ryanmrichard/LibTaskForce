/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MadnessHelper.hpp
 * Author: richard
 *
 * Created on February 11, 2016, 5:53 PM
 */

#ifndef MADNESSHELPER_HPP
#define MADNESSHELPER_HPP

namespace LibTaskForce{
    
    /** This free function sets the number of threads the next created
     *  madness::World will have.  Unfortunately, this seems to be only
     *  controllable via the environment variable MAD_NUM_THREADS
     * 
     *  \param[in] NThreads The number of threads to set MAD_NUM_THREADS to
     */
    void SetMadThreads(size_t NThreads);
    
}


#endif /* MADNESSHELPER_HPP */

