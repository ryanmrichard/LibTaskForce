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

/** \file ProcessFuture.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 13, 2016
 */

#ifndef LIBTASKFORCE_GUARD_PROCESSFUTURE_HPP
#define LIBTASKFORCE_GUARD_PROCESSFUTURE_HPP

#include<memory>

namespace LibTaskForce {
class ProcessComm;

template<typename T>
class ProcessFuture {
private:
    size_t Rank_;///<The rank that actually owns this data
    std::unique_ptr<T> Data_;///<The actual data
    const ProcessComm* Comm_;///<The comm for data transfer
public:
    ///Constructor for making a future when this process is responsible for data
    ProcessFuture(const T& Data,size_t Me,const ProcessComm* Comm):
        Rank_(Me),Data_(new T(Data)),Comm_(Comm)
    {}
        
    ///Constructor for making a future when this process is not responsible
    ProcessFuture(size_t Owner,const ProcessComm* Comm) :
    Rank_(Owner),Comm_(Comm)
    {}
    
    ProcessFuture()=default;
    ProcessFuture(const ProcessFuture<T>&)=delete;
    ProcessFuture& operator=(const ProcessFuture<T>&)=delete;
    ProcessFuture(ProcessFuture<T>&&)=default;
    ProcessFuture& operator=(ProcessFuture<T>&&)=default;
    
    
    bool empty()const{return !Data_;}///<Returns true if this future is empty
    
    ///Returns the value of the future (requires communication)
    T get(){
        T NewData;
        Comm_->bcast((empty()?NewData:*Data_),Rank_);
        return (empty()? NewData : *Data_);
    }
    
};

}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_PROCESSFUTURE_HPP */

