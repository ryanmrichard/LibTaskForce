#include<sstream>
#include "Communicator.hpp"
#include "Environment.hpp"
#include "MadnessHelper.hpp"
#include "ParallelAssert.hpp"
#include "madness/world/MADworld.h"
namespace LibTaskForce{

Communicator::Communicator(size_t NThreads,const MPI_Comm& Comm,
                      Environment* Env,bool Control):
                      Env_(Env),NThreads_(NThreads){
    PARALLEL_ASSERT(NThreads>0,
                    "Requested number of threads must be greater than 0"
    );
    SetMadThreads(NThreads_);
    World_=std::unique_ptr<madness::World>(new madness::World(
              SafeMPI::Intracomm(Comm,Control)
           ));
}    
  
/*Communicator::Communicator(madness::World world, Environment* Env):
        World_(new madness::World(world)),Env_(Env){

}*/

size_t Communicator::Rank()const{
    return static_cast<size_t>(World_->mpi.comm().Get_rank());
}

size_t Communicator::NProcs()const{
    return static_cast<size_t>(World_->mpi.comm().Get_size());
}

MPI_Comm Communicator::MPIComm()const{
    return World_->mpi.comm().Get_mpi_comm();
}

void Communicator::Barrier()const{
   World_->gop.fence();
}

//Does a floor, but ensures the types line up
size_t DivideFxn(size_t N, size_t D){
    return static_cast<size_t>(
            std::floor(static_cast<double>(N)/
                       static_cast<double>(D)
            ) 
           );
}

const std::unique_ptr<Communicator> Communicator::Split(size_t n,
                                 size_t m,
                                 size_t Maxm)const{
    
    PARALLEL_ASSERT(n<=NThreads()&& n>0,
       "Number of requested threads must be in the range [1,N]");
    PARALLEL_ASSERT(m<=NProcs() && m>0,
       "Number of requested processes must be in the range [1,M]");
    PARALLEL_ASSERT(Maxm<=NProcs(),
       "Maximum number of usable processors must be in range [0,M]");
    std::cout<<"Past assertions"<<std::endl;
    //If you are giving a task more than one process, it gets all the threads 
    //too
    if(m>1)n=NThreads();
    //This is the maximum number of processes we can use
    if(Maxm==0)Maxm=NProcs();
    
    //Do we need to make an extra group for left over processes?
    size_t ExtraProcs=Maxm%m;
    size_t NGroups=(Maxm-ExtraProcs)/m;
    size_t Extra=static_cast<size_t>(Maxm!=NProcs()||ExtraProcs>0);
    
    std::cout<<"Splitting "<<NProcs()<<" processes into "<<NGroups+Extra
             <<" groups with "<<ExtraProcs<<" leftover"<<std::endl;
    
    /*
     *  Here's the plan.  If Maxm!=NProcs() and/or Maxm is not evenly divided by
     *  m then the remaining NProcs()-Maxm+Maxm%m processes get shoved in the 
     *  last group.  The remaining floor(Maxm/m) groups of processes contain m
     *  processes each.  Which we for the time being assign like:
     *  Group[0]={0,1,2,...,m-1};
     *  Group[1]={m,m+1,m+2,...,2m-1};
     *  ...
     *  Group[floor(Maxm/m)-1]={Maxm-m,Maxm-m+1,....Maxm-1};
     *  In general the g-th group, g in the range [0,floor(Maxm/m)) contains 
     *  processes: g through (g+1)m-1 and process p is in group floor(p/m).
     * 
     *  I apparently do not understand MPI_groups so we're doing this with
     *  MPI_Split...
     * 
     *  Madness's communicator is written to split communicators based on groups
     */
    size_t Me=Rank(),FirstExtraProc=Maxm-ExtraProcs;
    size_t MyGroup=(Me>=FirstExtraProc?NGroups: DivideFxn(Me,m));
    
    MPI_Comm MPI_NewComm;
    MPI_Comm_split(MPIComm(),MyGroup,0,&MPI_NewComm);
    
    ///Now make a new world, with the appropriate number of threads
    size_t ThreadsForTasks=DivideFxn(NThreads(),n);
    std::unique_ptr<Communicator> NewComm(
       new Communicator(ThreadsForTasks,MPI_NewComm,Env_,true)
    );
    NewComm->Register();
    return NewComm; 
}

void Communicator::Register(){
    Env_->Register(*this);
}

void Communicator::Release(){
    Env_->Release(*this);
}

std::string Communicator::ToString()const{
    std::stringstream ss;
    ss<<"Communicator has "<<NProcs()
      <<" processes and "<<NThreads()<<" threads.";
    return ss.str();
}


Communicator::~Communicator(){
    Release();
    Env_=nullptr;
}

}//End namespaces

