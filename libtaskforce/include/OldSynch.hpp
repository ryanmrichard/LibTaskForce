/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OldSynch.hpp
 * Author: richard
 *
 * This file contains the Old Synch() routine for TAskResults if for some
 * reason I need it again.... you shouldn't use it ever
 * 
 * Created on March 9, 2016, 6:13 PM
 */

#ifndef OLDSYNCH_HPP
#define OLDSYNCH_HPP

template<typename T>
void GathervWrapper(const T* data,int NElems,T* Buffer,
                    int* Lengths,int * Offsets,int Root, MPI_Comm AComm){
    MPI_Gatherv(data,NElems,MPI_BYTE,Buffer,
                Lengths,Offsets,MPI_BYTE,Root,AComm);
}

//template<typename T>
//void GathervWrapper<std::vector<T>>(const )





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
            GathervWrapper(Buffer.data(),NElems,FullBuffer.data(),
                           BLengths.data(),Offsets.data(),Root,AComm);
            
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
            BCastWrapper(Data_.data(),Data_.size(),0,AComm);         
            
            //Since we copied the data free the futures
            //std::vector<Future<T>>().swap(Futures_);
            Futures_.clear();
            Futures_.resize(0);
        }


#endif /* OLDSYNCH_HPP */

