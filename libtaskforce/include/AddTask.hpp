/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AddTask.hpp
 * Author: richard
 *
 * Created on March 11, 2016, 2:21 PM
 */

#ifndef ADDTASK_HPP
#define ADDTASK_HPP

template<typename Fxn_t,typename...Args>
Future<
    typename RemoveFuture<
        typename std::result_of<Fxn_t(Args...)>::type
    >::type
>
Communicator::AddTask(Fxn_t YourTask,Args... args){
    typedef typename std::result_of<Fxn_t(Args...)>::type RealType_t;
    typedef typename RemoveFuture<RealType_t>::type Result_t;
    size_t TaskOwner=MyStats_->WhoRanTask(TasksAdded_);
    bool ImActive=MyStats_->Active(),MyTask=MyStats_->MyTask(TasksAdded_);
    Future<Result_t> Rvalue;
    //If I'm not running tasks, or if this isn't my problem I just return a
    //future
    if(!ImActive||!MyTask){
        Rvalue=Future<Result_t>(GetStats(),TasksAdded_++);
    }
    else if(NThreads()==1){//We're the only one running tasks...
        RealType_t Temp=YourTask(args...);
        Result_t Temp2=GetValue<Result_t>(Temp);
        Rvalue=Future<Result_t>(GetStats(),TasksAdded_++,Temp2);
    }
    else{ //Otherwise stick it in the queue and be done with it
        madness::Future<RealType_t> Value=World_->taskq.add(YourTask,args...);
        //Here's the trick if RealType_t is a Future we need to add a task that
        //dereferences it to the queue, the result of which is the actual value
        madness::Future<Result_t> RealValue;
        if(IsFuture<RealType_t>::value){
            std::function<Result_t(RealType_t)> fxn=[](RealType_t in){
                return GetValue<Result_t>(in);
            };
            RealValue=World_->taskq.add(fxn,Value);
        }
        else RealValue=ToMadFuture<Result_t>(Value);
        Rvalue=Future<Result_t>(GetStats(),TasksAdded_++,RealValue); 
    }
    //Need to wait for all processes to add the task or else they may get ahead
    //and request the task from a remote process, before that remote process
    //has gone through here.  For one thread there won't be any active messages
    //so this doesn't apply, plus the barrier makes the MPI processes wait even
    //if it's not their task
    if(NThreads()!=1)Barrier();
    return Rvalue;
  }



#endif /* ADDTASK_HPP */

