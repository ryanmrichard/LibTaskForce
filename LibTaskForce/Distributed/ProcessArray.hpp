/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file ProcessArray.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 14, 2016
 */

#ifndef LIBTASKFORCE_GUARD_PROCESSARRAY_HPP
#define LIBTASKFORCE_GUARD_PROCESSARRAY_HPP

#include "ProcessComm.hpp"


namespace LibTaskForce {

template<typename T>
class ProcessArray{
    private:
        std::vector<T> LocalData_;
        std::vector<size_t> ActualLocation_;
        const ProcessComm* Comm_;
    public:
        
        void push_back(const T& Data,size_t Location)
        {
            ActualLocation_.push_back(Location);
            LocalData_.push_back(Data);
        }
        
        void push_back(size_t Location)
        {
            ActualLocation_.push_back(Location)
        }
        
        std::vector<T> get()
        {
            Comm_->barrier();
            std::vector<T> Temp=Comm_->all_gatherv(LocalData_);
            //Now sort
        }
            
};

}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_PROCESSARRAY_HPP */

