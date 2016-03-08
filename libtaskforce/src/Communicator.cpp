#include<sstream>
#include "Communicator.hpp"
#include "Environment.hpp"
#include "MadnessHelper.hpp"
#include "ParallelAssert.hpp"
#include "madness/world/MADworld.h"
namespace LibTaskForce{

Communicator::Communicator(size_t NThreads,const MPI_Comm& Comm,
                           Environment* Env,bool Control,CommStats* Stats,
                           bool Register):
                      Env_(Env),NThreads_(NThreads),TasksAdded_(0),
                      MyStats_(Stats),Registered_(Register){
    PARALLEL_ASSERT(NThreads>0,
                    "Requested number of threads must be greater than 0"
    );
    //SetMadThreads(NThreads_);
    World_=std::unique_ptr<madness::World>(new madness::World(
              SafeMPI::Intracomm(Comm,Control)
           ));
    if(Registered_)this->Register();
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

bool Communicator::IsActive()const{return MyStats_->Active();}

MPI_Comm Communicator::MPIComm()const{
    return World_->mpi.comm().Get_mpi_comm();
}

void Communicator::Barrier()const{
   if(BarrierOn_&&MyStats_)MyStats_->World().mpi.comm().Barrier();
}

//Does a floor with integers
size_t DivideFxn(size_t N, size_t D){return (N-N%D)/D;}

Communicator Communicator::Split(size_t n,
                                 size_t m,
                                 size_t Maxm)const{
    
    PARALLEL_ASSERT(n<=NThreads()&& n>0,
       "Number of requested threads must be in the range [1,N]");
    PARALLEL_ASSERT(m<=NProcs() && m>0,
       "Number of requested processes must be in the range [1,M]");
    PARALLEL_ASSERT(Maxm<=NProcs(),
       "Maximum number of usable processors must be in range [0,M]");
    //If you are giving a task more than one process, it gets all the threads 
    //too
    if(m>1)n=NThreads();
    //This is the maximum number of processes we can use
    if(Maxm==0)Maxm=NProcs();
    
    PARALLEL_ASSERT(m<=Maxm,
      "Number of requested processes must be in the range [1,Maxm]");
    
    //Number of extra processes and number of process groups
    size_t ExtraProcs=Maxm%m,NGroups=DivideFxn(Maxm,m);
    
    /*
     *  Here's the plan.  We have NProcs() to give away so normally any process
     *  with rank>=Maxm would be extra, but if r=Maxm%m is not 0, there are r
     *  other processes that aren't doing anything (we never give you more or
     *  less than m).  Thus, these processes also need to go into the extra
     *  group.  This means the first "extra" process is actually the one
     *  with rank Maxm-ExtraProcs.  The remaining processes are then put into
     *  floor(Maxm/m) groups of m processes each.
     */
    size_t Me=Rank(),FirstExtraProc=Maxm-ExtraProcs;
    bool IsActive=(Me<=FirstExtraProc);
    size_t MyGroup=(IsActive?DivideFxn(Me,m):NGroups);
    
    RoundRobin* MyStats=new RoundRobin(this->World_,IsActive,NGroups,MyGroup);
    
    /*  I apparently do not understand MPI_groups so we're doing this with
     *  MPI_Split...
     * 
     *  Madness's communicator is written to split communicators based on 
     *  groups and if I could get that to work we could force this MPI call
     *  down into Madness
     */
    MPI_Comm MPI_NewComm;
    MPI_Comm_split(MPIComm(),MyGroup,0,&MPI_NewComm);
    
    ///Now make a new comm, with the appropriate number of threads
    ///Note that this is the number of task spawners, each task spawner
    ///contains n threads
    size_t ThreadsForTasks=DivideFxn(NThreads(),n);

    //Here's a cool C++11 trick to retrun non-copyable and non-moveable
    //objects, return just the braced-init-list    
    return {ThreadsForTasks,MPI_NewComm,Env_,true,MyStats,true};
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
      <<" process(es) and "<<NThreads()<<" thread(s).";
    return ss.str();
}


Communicator::~Communicator(){
    if(Registered_)Release();
    Env_=nullptr;
    std::cout<<"I am released"<<std::endl;
}

}//End namespaces

