/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "CommStats.hpp"
#include "Communicator.hpp"

namespace LibTaskForce{
    
CommStats::CommStats(const Communicator* ParentComm,bool IsActive,size_t Me):
        Active_(IsActive),MyRank_(Me),Parent_(ParentComm){}

const Communicator& CommStats::Comm()const{
        PARALLEL_ASSERT(Parent_!=nullptr,"Parent communicator is not set");
        return *Parent_;
    }   

}//End namespace LibTaskForce