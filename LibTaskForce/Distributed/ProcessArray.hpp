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

