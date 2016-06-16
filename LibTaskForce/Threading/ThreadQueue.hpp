/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file ThreadQueue.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 10, 2016
 */

#ifndef LIBTASKFORCE_GUARD_THREADQUEUE_HPP
#define LIBTASKFORCE_GUARD_THREADQUEUE_HPP
#include <tbb/tbb.h>

namespace LibTaskForce {
template<typename T> class ThreadFuture;

///Abstracts away the actual queue implementation
class ThreadQueue{
private:
    tbb::task_group Queue_;///< The actual queue
public:    
    ThreadQueue(size_t)
    {}
    
    template<typename TaskType>
    ThreadFuture<typename TaskType::return_type> add_task(TaskType& Task)
    {
        ThreadFuture<typename TaskType::return_type> 
            Fut(std::move(Task.P_->get_future()),*this);
        Queue_.run(Task);
        return Fut;
    }
    
    template<typename TaskType,typename const_iterator>
    typename TaskType::return_type reduce(TaskType& Task,
                                          const_iterator Begin,
                                          const_iterator End)
    {
        using return_type=typename TaskType::return_type;
        tbb::blocked_range<const_iterator> r(Begin,End);
        tbb::parallel_reduce(r,Task);
        return Task.MySum_;
    }
    
    void wait()
    {
        Queue_.wait();
    }
};

}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_THREADQUEUE_HPP */

