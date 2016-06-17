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

/** \file GeneralTask.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 16, 2016
 */

#ifndef LIBTASKFORCE_GUARD_GENERALTASK_HPP
#define LIBTASKFORCE_GUARD_GENERALTASK_HPP

namespace LibTaskForce {

template<typename functor_type,typename comm_type>
struct Task {
    comm_type& CurrentComm_;
    functor_type Fxn_;
    Task(const functor_type& Fxn,comm_type& Comm) :
       CurrentComm_(Comm),Fxn_(Fxn)
    {}
    Task(const Task&)=default;
};


}//End namespace libtaskforce
#endif /* PULSAR_GHUARD_GENERALTASK_HPP */

