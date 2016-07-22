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

/** \file ThreadTask.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 10, 2016
 */

#ifndef LIBTASKFORCE_GUARD_THREADTASK_HPP
#define LIBTASKFORCE_GUARD_THREADTASK_HPP

#include <future>
#include "LibTaskForce/General/GeneralTask.hpp"

namespace LibTaskForce {

///A wrapper around a functor for use in ThreadComm::add_task
template<typename T,typename functor_type,typename comm_type>
struct ThreadTask :public Task<functor_type,comm_type> {
    using promise_type = std::promise<T>;
    using return_type=T;
    std::shared_ptr<promise_type> P_;///<Eventually will be the result
    ThreadTask(const ThreadTask&) = default;
    
    ThreadTask(functor_type&& F, comm_type& Cm) :
    Task<functor_type,comm_type>(std::forward<functor_type>(F),Cm),
            P_(std::make_shared<promise_type>())
    {}

    ///The call used by tbb for running the task
    void operator()()const
    {
        std::unique_ptr<comm_type> Comm=this->CurrentComm_.split();
        P_->set_value(this->Fxn_(*Comm));
    }
};

///A wrapper around a task for doing reduction on my terms
template<typename T,typename functor_type>
struct ReduceTask {
    using return_type=T;///< The type of the final answer
    using MyType=ReduceTask<return_type,functor_type>;///< The type of this
    functor_type Fxn_;///<The functor that will be used for reduction
    return_type MySum_;///< Eventually the result
    
    ///Makes a new reduction task that will apply Fxn in range [Begin,End)
    ReduceTask(const functor_type& Fxn) :
        Fxn_(Fxn),MySum_()
    {}
    
    ///The split constructor for tbb
    ReduceTask(MyType& Other,tbb::split):Fxn_(Other.Fxn_),MySum_()
    {}
    
    ///The call tbb will use
    template<typename const_iterator>
    void operator()(const tbb::blocked_range<const_iterator>& range)
    {
        //Gotcha: don't use range-based for loop b/c need to pass iterators
        return_type Sum=MySum_;
        const_iterator end=range.end();
        for(const_iterator i=range.begin();i!=end;++i)Sum=Fxn_(Sum,Fxn_(i));
        MySum_=Sum;
    }
    
    ///Used by tbb for joining results
    void join(const MyType& Other)
    {
        MySum_=Fxn_(MySum_,Other.MySum_);
    }
};


}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_THREADTASK_HPP */

