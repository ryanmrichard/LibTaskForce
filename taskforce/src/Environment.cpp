#include <cstdlib>
#include "madness/world/MADworld.h"
#include "Environment.hpp"
#include "Communicator.hpp"
#include "ParallelAssert.hpp"
#include "MadnessHelper.hpp"

namespace LibTaskForce{  
    
Environment::Environment(size_t NThreads):
    Environment(NThreads,MPI_COMM_WORLD){}

Environment::Environment(size_t NThreads,const MPI_Comm& Comm):
    NThreads_(NThreads),PopLock_(true){
    PARALLEL_ASSERT(NThreads>0,
      "The number of threads must be greater than 0");
   SetMadThreads(NThreads);
   //MPI-2 and greater don't actually use these...
   int argc=0;
   char** argv=nullptr;
   //Initialize madness if it hasn't been started
   //otherwise just grab the active world
   madness::World& OrigWorld=(madness::initialized()?
           madness::initialize(argc,argv,Comm):
           madness::initialize(argc,argv));
   Comms_.push(Comm_t(
      new Communicator(NThreads,OrigWorld.mpi.comm().Get_mpi_comm(),this,true)
   ));
}


Environment::~Environment(){
    //Shouldn't throw in a destructor so we use normal assert, which crashes
    assert(Comms_.size()==1);
    
    Comms_.top()->Barrier();
    Comms_.pop();
    
   //Really should check if this has been called,
   //but madness doesn't seem to support that
   madness::finalize();
}

void Environment::Release(const Communicator& Comm2Release){
    if(PopLock_&&Comms_.size()>1){//Don't pop world
        PARALLEL_ASSERT(ParentComm_.top()==&Comm2Release,
           "Comms are not getting removed in the order I thought..."
        );
        ParentComm_.pop();
        Comms_.pop();
    }
    //Only every other call to this function releases
    PopLock_=!PopLock_;
}

void Environment::Register(const Communicator& Comm2Register){
    size_t NewThreads=Comms_.top()->NThreads()-Comm2Register.NThreads();
    Comms_.push(Comm_t(
       new Communicator(
            (NewThreads>0?NewThreads:1),Comm2Register.MPIComm(),this
           )
    ));
    ParentComm_.push(&Comm2Register);
}

const Communicator& Environment::Comm()const{
    PARALLEL_ASSERT(Comms_.size()!=0,
                    "This environment doesn't have communicators!!!!");
   return *Comms_.top();
}
}//End namespaces

