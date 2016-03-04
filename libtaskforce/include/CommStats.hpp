/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CommStats.hpp
 * Author: richard
 *
 * Created on February 22, 2016, 11:47 AM
 */

#ifndef COMMSTATS_HPP
#define COMMSTATS_HPP

namespace LibTaskForce{

/** \brief Little class for storing the details of a communicator
 * 
 *  The primary purpose of this class is to provide an interface to how
 *  tasks are scheduled among processors.  For example, in a round-robin
 *  distribution tasks are assigned among the \f$NProc\f$ processes, such that
 *  process \f$i\f$ gets task \f$n\f$ if \f$i=n%NProc\f$.
 * 
 *  This class also maintains whether a communicator is active.  Say we have
 *  4 processes and you request 3 of them.  For three of those processes, they
 *  would be "active" in that they are part of the group you wanted.  For the
 *  other process it would report false for its activity.
 * 
 *  This class is intended to be derived from for each scheduling scenario
 * 
 */
class CommStats{
public:
    ///Initializes the communicator's statistics
    CommStats(std::shared_ptr<madness::World> Parent,
              bool IsActive=true,size_t Me=0):
        Active_(IsActive),MyRank_(Me),Parent_(Parent){}
        
    ///No clean-up, but makes the compiler hush
    virtual ~CommStats(){}
       
    ///Returns true if this communicator is active in the MPI action
    bool Active()const{return Active_;}
    
    ///Returns my rank
    size_t Rank()const{return MyRank_;}
    
    ///True if I should run this particular task
    bool MyTask(size_t TaskNum)const{return WhoRanTask(TaskNum)==MyRank_;}
    
    ///Returns the process that ran the task
    virtual size_t WhoRanTask(size_t TaskNum)const=0;
    
    ///Returns the Madness world on which communication should be done
    madness::World& World()const{return *Parent_;}
    
private:
    ///True if communicator is active
    const bool Active_;
    ///My process rank
    const size_t MyRank_;
    ///The Madness world of our parent, needed for communication
    std::shared_ptr<madness::World> Parent_;
};

///CommStats specialization to tasks being handled via a round-robin manner
class RoundRobin: public CommStats{
private:
    const size_t NRanks_;
public:
    ///By default creates a communicator where everyone runs every task
    RoundRobin(std::shared_ptr<madness::World> Parent=
                      std::shared_ptr<madness::World>(),
               bool IsActive=true,
               size_t NRanks=1,
               size_t Me=0):
        CommStats(Parent,IsActive,Me),NRanks_(NRanks){}
    size_t WhoRanTask(size_t TaskNum)const{return TaskNum%NRanks_;}
};


}//End namespaces

#endif /* COMMSTATS_HPP */
