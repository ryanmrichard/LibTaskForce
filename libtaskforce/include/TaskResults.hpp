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
#include <sstream>
#include <iostream>
#include "Future.hpp"
#include "Communicator.hpp"
#include "ParallelAssert.hpp"
#include "Serialization.hpp"
namespace LibTaskForce {

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
     *              is double.  T must be serializable by cereal (all stl containers
     *              and basic types are by default
     * 
     * 
     */
    template<typename T>
    class TaskResults {
    public:

        TaskResults(const Communicator& Comm) :
            WasSynched_(false),Stats_(Comm.GetStats()) {
        }

        void push_back(const Future<T>& FIn) {
            Futures_.push_back(FIn);
        }

        ///Retrieves the data at element i, calls Synch()

        T& operator[](size_t i) {
            Synch();
            RangeCheck(i);
            return Data_[i];
        }

        ///Const-ly retrieves the data at element i, synch must have been used

        const T& operator[](size_t i)const {
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

        iterator begin() {
            SynchCheck();
            return Data_.begin();
        }

        ///Forwarding of const begin with error checking

        const_iterator begin()const {
            SynchCheck();
            return Data_.begin();
        }

        ///Forwarding of end with error checking

        iterator end() {
            SynchCheck();
            return Data_.end();
        }

        ///Forwarding of const end with error checking

        const_iterator end()const {
            SynchCheck();
            return Data_.begin();
        }

        ///Number of elements that will be in the container after synching

        size_t size()const {
            return Futures_.size() > 0 ? Futures_.size() : Data_.size();
        }

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

        void SynchCheck()const {
            PARALLEL_ASSERT(WasSynched_, "Synch() was not called!!!");
        }

        ///Checks if element is in range, throws if it is not (only in Debug)

        void RangeCheck(size_t i)const {
            PARALLEL_ASSERT(Data_.size() > i,
                    "Requested element is not in the range [0,N_elements)");
        }
    };

    template<typename T>
    void TaskResults<T>::Synch() {
        //General note: We use int for quantities in this section b/c
        // that's what MPI expects
        if (WasSynched_)return;
        WasSynched_ = true;

        typedef std::vector<T> Result_t;
        
        const size_t Me=Stats_->Rank(),NProcs = Stats_->Comm().NProcs(),Root=0;
        const Communicator& Comm=Stats_->Comm();
        
        bool AmIRoot = (Me == Root);

        std::vector<size_t> Offsets(AmIRoot?NProcs:0);
        
        //Two buffers we need, first is unsorted data on root, second is
        //each process' data
        Result_t FullResults, LocalResults;

        for (Future<T>& Fi : Futures_)
            if (Fi.IsLocal())LocalResults.push_back(*Fi);

        //Now we implement a sort of gatherv
        //\todo Can this be written both serialized and with gatherv?
        for (size_t i = 0; i < NProcs; ++i) {
            if (AmIRoot)Offsets[i] = FullResults.size();
            if (i == Root) {
                if (AmIRoot)
                    FullResults.insert(FullResults.end(),
                        LocalResults.begin(), LocalResults.end());
                continue;
            }
            if (AmIRoot) {
                Result_t DeSerial;
                Comm.Recv(DeSerial,i);
                FullResults.insert(FullResults.end(),
                        DeSerial.begin(), DeSerial.end());
            } else if (Me == i)
                Comm.Send(LocalResults,Root);
        }

        int BufferLength = 0;
        //Data is likely all discombobulated...so discombobulate it

        /* Plan: we have the futures in order, each future knows its
         * task number, use Stats to find which process ran that task
         * Because the futures are in the same order on all processes, 
         * the first time process i is found to be the owner the result
         * that goes with that is process i's first result, the second 
         * time that processor pops up, it's i's second result, etc.
         */
        if (AmIRoot) {
            std::vector<size_t> Counters(NProcs, 0);
            Result_t().swap(Data_);
            for (Future<T>& Fi : Futures_) {
                size_t Task = Fi.TaskNum();
                size_t Owner = Stats_->WhoRanTask(Task);
                size_t RealOff = Offsets[Owner] + Counters[Owner];
                ++Counters[Owner];
                Data_.push_back(FullResults[RealOff]);
            }
        }
        
        Comm.Bcast(Data_,Root);
        
        //Since we copied the data free the futures
        std::vector<Future < T >> ().swap(Futures_);
    }

}//End namespace

#endif /* TASKRESULTS_HPP */

