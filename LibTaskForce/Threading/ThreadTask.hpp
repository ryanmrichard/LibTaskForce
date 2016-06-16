/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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

namespace LibTaskForce {
class ThreadComm;

///A wrapper around a functor for use in ThreadComm::add_task
template<typename return_type,typename functor_type>
struct ThreadTask {
    functor_type Fxn_;///<The function that will be called
    using return_type = return_type;
    using promise_type = std::promise<return_type>;
    std::shared_ptr<promise_type> P_;///<Eventually will be the result
    ThreadComm& CurrentComm_;///<The communicator we are using
    ThreadTask(const ThreadTask&) = default;
    
    ThreadTask(const functor_type& F, ThreadComm& Cm) :
    Fxn_(F), P_(std::make_shared<promise_type>()), CurrentComm_(Cm)
    {
    }

    ///The call used by tbb for running the task
    void operator()()const
    {
        std::unique_ptr<ThreadComm> Comm=CurrentComm_.split();
        P_->set_value(Fxn_(*Comm));
    }
};

///A wrapper around a task for doing reduction on my terms
template<typename return_type,typename functor_type>
struct ReduceTask {
    using MyType=ReduceTask<return_type,functor_type>;///< The type of this
    using return_type=return_type;///< The type of the final answer
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

