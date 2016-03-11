/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AllReduce.hpp
 * Author: richard
 *
 * Created on March 11, 2016, 2:22 PM
 */

#ifndef REDUCE_HPP
#define REDUCE_HPP

template<typename ResultIn_t,typename Itr_t,typename Op_t>
Future<typename RemoveFuture<ResultIn_t>::type> Communicator::Reduce(Itr_t BeginItr, Itr_t EndItr,
                                      const Op_t& Op,size_t Chunk){
    typedef typename RemoveFuture<ResultIn_t>::type Result_t;
    Future<Result_t> RValue;
    /*bool BarrierStatus=BarrierOn_;
    BarrierOn_=false;
    bool ImActive=MyStats_->Active(),MyTask=MyStats_->MyTask(TasksAdded_);

    size_t Distance=std::distance(BeginItr,EndItr);
    if(Distance>Chunk){//We have work to do...
        //All processes need to go through this logic so that the TasksAdded_ 
        //line up
    
        //Split the range and run recursively (TasksAdded_ updated in AddTask)
    
        *   Assume we had 9 tasks in a vector
        *   {1,2,3,4,5,6,7,8,9}
        *   Distance will be 9, hence Distance%2=1 and we are
        *   advancing the begin iterator by 4.  It originally pointed
        *   to 1, now it points to 5.  Thus the LSum will entail 4 values and
        *   the right 5.
        *
        Itr_t RHSBegin(BeginItr);
        std::advance(RHSBegin,(Distance-Distance%2)/2);
        //Barrier's off...
        std::function<Future<Result_t>(Itr_t,Itr_t,const Op_t&, size_t)> Fxn=
                [=](Itr_t Bit,Itr_t Eit,const Op_t& O1, size_t C1){
                    return this->Reduce<Result_t>(Bit,Eit,O1,C1);
                 };
        Future<Result_t> LSum=
           AddTask(Fxn,BeginItr,RHSBegin,Op,Chunk);
        Future<Result_t> RSum=
           AddTask(Fxn,RHSBegin,EndItr,Op,Chunk);
        //Use a proxy to keep the asynch going...
        class ReduceProxy{
            private:
                Op_t Op_;
            public:
                ReduceProxy(const Op_t& OpIn):Op_(OpIn){}
                Result_t operator()(Future<Result_t> LHS, 
                                    Future<Result_t> RHS)const{
                return Op_(*LHS,*RHS);
                }
        };
        RValue=AddTask(ReduceProxy(Op),LSum,RSum);
    }
    else{
        if(!ImActive||!MyTask)
            RValue=Future<Result_t>(GetStats(),TasksAdded_++);
        else if(NThreads()==1){//Don't call Madness or it will spawn a thread
            Result_t Sum=Result_t();
            for(;BeginItr!=EndItr;++BeginItr)
                Sum=Op(Sum,Op(BeginItr));
            RValue=Future<Result_t>(GetStats(),TasksAdded_++,Sum);
        }
        else{//Madness takes over now
            RValue=Future<Result_t>(GetStats(),TasksAdded_++,
                World_->taskq.reduce<Result_t>(
                    madness::Range<Itr_t>(BeginItr,EndItr,Chunk),Op
                )
            );       
        }
    }
    //Need barrier for same reason we needed it in AddTask,but need to be
    //careful with the recursion...
    //If we turned it off, turn it on
    if(BarrierStatus)BarrierOn_=true;
    if(NThreads()!=1)Barrier();*/
    return RValue;
}

template<typename Itr_t,typename Op_t>
Future<bool> Communicator::ForEach(Itr_t BeginItr,Itr_t EndItr,const Op_t& Op,
                     size_t ChunkSize){
    class OpWrapper{
        private:
            Op_t Op_;
        public:
            OpWrapper(const Op_t& OpIn):Op_(OpIn){}
            OpWrapper(const OpWrapper&)=default;
            bool operator()(const Itr_t& Itr)const{
               return Op_(*Itr);
            }
            bool operator()(bool lhs,bool rhs)const{
                //We cheat since bool defaults to 0 and any non-zero value is
                //true (cheat required because reduce will default initialize
                //the first value and false && anything= false)
                size_t NewLhs=static_cast<size_t>(lhs);
                size_t NewRhs=static_cast<size_t>(rhs);
                return static_cast<bool>(NewLhs + NewRhs);
            }
    };
    return Reduce<bool>(BeginItr,EndItr,OpWrapper(Op),ChunkSize);
}


#endif /* REDUCE_HPP */

