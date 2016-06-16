/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/** \file ProcessComm.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 13, 2016
 */

#ifndef LIBTASKFORCE_GUARD_PROCESSCOMM_HPP
#define LIBTASKFORCE_GUARD_PROCESSCOMM_HPP

#include <mpi.h>
#include "LibTaskForce/Util/ParallelAssert.hpp"
#include "LibTaskForce/Util/Serialization.hpp"
#include "LibTaskForce/Distributed/ProcessFuture.hpp"
#include "LibTaskForce/Distributed/ProcessQueue.hpp"
#include "LibTaskForce/General/GeneralComm.hpp"

namespace LibTaskForce {
class ProcessEnv;

/** \brief The main interface to distributed parallelism
 *
 *   Where TBB took care of much of our threading details we have no such
 *   luxury with MPI.  Hence, we try to make this interface parallel to that
 *   of ThreadComm, but it has some additional features related to MPI functions
 *   we'll need (send/recv, broadcast, gather).  This is pretty similar to 
 *   Boost MPI's wrapping, but without the Boost dependency.  OpenMPI provides
 *   a similar C++ syntax as well, but I'm under the impression this is only
 *   for OpenMPI and not for other MPI flavors.
 * 
 *   Again I'm trying to keep asynchronous parallelism on the table.  To do this
 *   with MPI it's a little bit different.  Processes don't really run in the
 *   background the way threads do.  We thus split the processes into two groups
 *   the ones working on a task and those who aren't.  The ones who are active
 *   start working while those who aren't just return futures.
 * 
 *   \code
 *   ProcessComm NewComm=OldComm.split(2);
 *   //Two processes sit here running ATask
 *   ProcessFuture Result1=NewComm.add_task(ATask);
 *   ProcessFuture Result2=NewComm.add_task(ATask);
 * 
 *   //Others have fallen through
 *   if(!NewComm.active()){
 *      //TODO: How do other processes do stuff?
 *   }
 *   \endcode
 *   
 *
 */
class ProcessComm : public GeneralComm<ProcessEnv,ProcessQueue>{
private:
    using base_type=GeneralComm<ProcessEnv,ProcessQueue>;
    MPI_Comm Comm_;///<The MPI communicator we are wrapping
    bool Active_;///<True if in on the action
    friend class ProcessEnv;///<Allows Env to make the initial comm
    ProcessComm(MPI_Comm Comm,ProcessEnv* Env);
public:
    ProcessComm(ProcessComm&&)=default;
    ProcessComm& operator=(ProcessComm&&)=default;
    ~ProcessComm();///<Gives resources back.
    
    
    MPI_Comm mpi_comm()const{return Comm_;}///<Returns MPI comm for interfacing
    void barrier()const;///<Waits for all processes to get to this call
    size_t size()const;///<The number of processors on this comm
    size_t rank()const;///<What MPI rank this process is on this comm
    bool active()const;///<True if our comm is in on the MPI action
    
    
    /** \brief Splits the communicator to allow for actual parallelism to be
     *         done.
     * 
     *  The ProcessEnv returns a const communicator, which you can't do anything
     *  with until you split it (think of it as allocating the resources).  
     *  Upon "allocation" you now have a communicator that can do stuff.  This
     *  communicator has however many processes you requested and the remaining
     *  processes are available for doing other work.
     * 
     *  For example, say we have 4 processes to begin with.  You want 2.  You
     *  would then do:
     *  \code
     *  ProcessComm NewComm=OldComm.split(2);
     *  \endcode
     * 
     *  At this point you then get a new communicator back.  On two of the four
     *  processes NewComm.active() will be false (signaling to you that these
     *  are not the processes you have been assigned) and on the other two that
     *  call will be true (these are your processes)
     * 
     *  \param[in] NProcs The number of processes you want.  Default is 0, which
     *                    is a special value indicating all available processes
     * 
     *  \note Somewhat paradoxically if you want to run serially you need to
     *        request all of the processes (and then not use any MPI calls).
     *  
     */
    std::unique_ptr<ProcessComm> split(size_t NProcs=0)const;
    
    enum IDs{
        ROOT_PROCESS=0
    };///< enum for setting the root process
    
    enum Tags{
        GENERIC_TAG=123,
        GENERIC_SIZE=999
    };///< Enums for message tags
    
    template<typename T>
    void send(const T& Data,size_t RecvID, size_t MsgTag=GENERIC_TAG)const
    {
        binary_type SerializedData=serialize(Data);
        int Length=(int)SerializedData.size();
        MPI_Send(&Length,1,MPI_INT,(int)RecvID,GENERIC_SIZE,Comm_);
        MPI_Send(SerializedData.data(),Length,MPI_BYTE,(int)RecvID,(int)MsgTag,Comm_);
    }
    
    template<typename T>
    void recv(T& Data,size_t SenderID,size_t MsgTag=GENERIC_TAG)const
    {
        const int Sender=(int)SenderID;  
        int Length;
        MPI_Recv(&Length,1,MPI_INT,Sender,GENERIC_SIZE,Comm_,MPI_STATUS_IGNORE);
        binary_type BinData(Length);
        MPI_Recv(BinData.data(),Length,MPI_BYTE,Sender,(int)MsgTag,Comm_,MPI_STATUS_IGNORE);
        Data=deserialize<T>(BinData);
    }

    template<typename T>
    void bcast(T& Data,size_t RootID=ROOT_PROCESS)const
    {
          binary_type BinData;
          if(rank()==RootID)BinData=serialize(Data);
          int Length=(int)BinData.size();
          const int Root=(int)RootID;
          int Error=MPI_Bcast(&Length,1,MPI_INT,Root,Comm_);
          PARALLEL_ASSERT(Error==MPI_SUCCESS,"Broadcast failed");
          if(rank()!=RootID)BinData=binary_type(Length);
          Error=MPI_Bcast(BinData.data(),Length,MPI_BYTE,Root,Comm_);
          PARALLEL_ASSERT(Error==MPI_SUCCESS,"Broadcast failed");
          if(rank()!=RootID)Data=deserialize<T>(BinData);
    } 
    
    template<typename T>
    T all_gatherv(T& Data)const
    {
        binary_type BinData=serialize(Data);
        int Length=(int)BinData.size();
        std::vector<int> Lengths(this->size());
        MPI_Allgather(&Length,1,MPI_INT,Lengths.data(),1,MPI_INT,Comm_);
        int Total=0;
        std::vector<int> Displacements(this->size());
        for(size_t i=1;i<Lengths.size();++i){
            Total+=Lengths[i-1];
            Displacements[i]=Displacements[i-1]+Lengths[i-1];
        }
        binary_type Buffer(Total);
        MPI_Allgatherv(BinData.data(),Length,MPI_BYTE,Buffer.data(),
                Lengths.data(),Displacements.data(),Comm_);
        return deserialize<T>(Buffer);
    }
    
    ///Allows you to add tasks one at a time (useful if tasks are all different)
    template<typename return_type,typename functor_type>
    ProcessFuture<return_type> add_task(const functor_type& Fxn)
    {
        return Queue_->add_task<return_type>(Fxn);
    }
    
    /** \brief More efficient way of adding many tasks at once
     * 
     *  
     *
    template<typename return_type,typename iterator_type>
    ProcessFuture<std::vector<return_type>> add_tasks(iterator_type Begin,iterator_type End)
    {
        return Queue_->add_tasks<return_type>(Begin,End);
    }*/
};



}//End namespace LIbTaskForce
#endif /* LIBTASKFORCE_GUARD_PROCESSCOMM_HPP */

