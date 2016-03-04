/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TaskResults.hpp
 * Author: richard
 *
 * Created on February 22, 2016, 11:19 AM
 */

#ifndef TASKRESULTS_HPP
#define TASKRESULTS_HPP

#include <vector>
#include <mpi.h>
#include "Future.hpp"
#include "Communicator.hpp"
#include "ParallelAssert.hpp"

namespace LibTaskForce{
/** \brief Implements a buffer for collecting task results.
 *
 * By default, adding tasks to a communicator returns a future to their result.
 * If you have lots of futures and you want their results to be available on all
 * processes put your futures in this class.  Basically, this class ensures
 * that MPI communication is done efficiently, which will not happen if you
 * put all the futures in a vector and then element-wise try to retrieve them.
 * 
 * This class has basic std::vector functions like iterators, size(), and
 * element retrieval
 * 
 * Data is stored in the order you push futures back, i.e. the first future
 * you add is element 0, the second is element 1, etc.
 * 
 * Using type erasure it would be possible to put any type in this container,
 * but at the moment I do not do that.  It instead is assumed that all of the
 * results you are interested in have the same type.
 * 
 * \param[in] T The type of your result w/o the future wrapper, i.e. if you are
 *              loading this container with Future<double> instances, than T
 *              is double.
 */
template<typename T>
class TaskResults{
    public:
        TaskResults(const Communicator& Comm):
            WasSynched_(false),Stats_(Comm.GetStats()){}
        
        void push_back(const Future<T>& FIn){
            Futures_.push_back(FIn);
        }
        
        ///Retrieves the data at element i, calls Synch()
        T& operator[](size_t i){
            Synch();
            RangeCheck(i);
            return Data_[i];
        }
        
        ///Const-ly retrieves the data at element i, synch must have been used
        const T& operator[](size_t i)const{
            SynchCheck();
            RangeCheck(i);
            return Data_[i];
        }
        
        /** Calling this function ensures that all processes have the data
         *  subsequent calls are null operations.
         */  
        void Synch();
        
        
        
        typedef typename std::vector<T>::iterator iterator;
        typedef typename std::vector<T>::const_iterator const_iterator;
        
        ///Forwarding of begin with error checking
        iterator begin(){
            SynchCheck();
            return Data_.begin();
        }
        
        ///Forwarding of const begin with error checking
        const_iterator begin()const{
            SynchCheck();
            return Data_.begin();
        }
        
        ///Forwarding of end with error checking
        iterator end(){
            SynchCheck();
            return Data_.end();
        }
        
        ///Forwarding of const end with error checking
        const_iterator end()const{
            SynchCheck();
            return Data_.begin();
        }
        
        ///Number of elements that will be in the container after synching
        size_t size()const{return Futures_.size()>0?Futures_.size():Data_.size();}
        
     private:
        ///True if Synch() was called
        bool WasSynched_;
        
        ///This is where the replicated data lives
        std::vector<T> Data_;
        
        ///These are the futures we are holding
        std::vector<Future<T>> Futures_;
        
        ///These are the CommStats we are using
        std::shared_ptr<const CommStats> Stats_;
        
        ///Checks if we synched, throws if we didn't (only in Debug versions)
        void SynchCheck()const{
            PARALLEL_ASSERT(WasSynched_,"Synch() was not called!!!");
        }
        
        ///Checks if element is in range, throws if it is not (only in Debug)
        void RangeCheck(size_t i)const{
            PARALLEL_ASSERT(Data_.size()>i,
               "Requested element is not in the range [0,N_elements)");
        }
};

template<typename T>
void TaskResults<T>::Synch(){
            if(WasSynched_)return;
            WasSynched_=true;
            
            //Gather some MPI info (Madness didn't wrap gather and gatherv)
            MPI_Comm AComm=Stats_->World().mpi.comm().Get_mpi_comm();
            size_t Me=Stats_->Rank(),
                   TotalElems=Futures_.size(),
                   NProcs=Stats_->World().mpi.nproc(),
                   Root=0;
            
            //Here we initialize some arrays avoiding unnecessary allocation
            //on all processes besides root
            //BLengths=I-th element is length of process i's message
            //Offsets=I-th element is start of process i's data
            std::vector<int> BLengths(Me==Root?NProcs:0),
                             Offsets(Me==Root?NProcs:0);
            
            //For Root the unsorted buffer, for all others will become Data_
            std::vector<T> FullBuffer(TotalElems);
            //The local elements
            std::vector<T> Buffer;
            for(Future<T>& Fi : Futures_)
                if(Fi.IsLocal())Buffer.push_back(*Fi);
            
            //Number of elements to send (possibly different on each Proc and
            //in bytes)
            int NElems=Buffer.size()*sizeof(T);
            
            //At this point each process has their N results in the first N
            //elements of Data_.  Now we need to let our root process know
            //how many elements each process has
                       
            //Send the lengths to the root
            MPI_Gather(&NElems,1,MPI_INT,BLengths.data(),1,MPI_INT,Root,AComm);
            
            if(Me==Root)//Compute offsets (i=0 is already 0)
                for(size_t i=1;i<BLengths.size();i++)
                    Offsets[i]=BLengths[i-1]+Offsets[i-1];
            
            //Now receive the actual data
            MPI_Gatherv(Buffer.data(),NElems,MPI_BYTE,
                        FullBuffer.data(),BLengths.data(),Offsets.data(),
                        MPI_BYTE,Root,AComm);
            
            //Data is likely all discombobulated...so discombobulate it
                
            /* Plan: we have the futures in order, each future knows its
             * task number, use Stats to find which process ran that task
             * Because the futures are in the same order on all processes, 
             * the first time process i is found to be the owner the result
             * that goes with that is process i's first result, the second 
             * time that processor pops up, it's i's second result, etc.
             */ 
            if(Me==Root){
                std::vector<size_t> Counters(NProcs,0);
                Data_.clear();
                for(Future<T>& Fi : Futures_){
                    size_t Task=Fi.TaskNum();
                    size_t Owner=Stats_->WhoRanTask(Task);
                    size_t RealOff=Offsets[Owner]/sizeof(T)+Counters[Owner];
                    ++Counters[Owner];
                    Data_.push_back(FullBuffer[RealOff]);
                }
            }
            else{
                //Data hasn't been allocated yet, and on non root processes 
                //it'll be the same size as this array
                Data_.swap(FullBuffer);
            }
            
            //Give it to the other ranks
            Stats_->World().mpi.Bcast(Data_.data(),Data_.size(),0);            
            
            //Since we copied the data free the futures
            std::vector<Future<T>>().swap(Futures_);
        }

}//End namespace

#endif /* TASKRESULTS_HPP */

