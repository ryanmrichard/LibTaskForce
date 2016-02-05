#include <cstdlib>
#include "madness/world/MADworld.h"
#include "Environment.hpp"
#include "Communicator.hpp"
#include "ParallelAssert.hpp"

namespace bpmodule{
namespace LibParallel{

Environment::Environment(size_t NThreads,int argc, char** argv):
      argc_(argc),argv_(argv),NThreads_(NThreads){
    PARALLEL_ASSERT(!madness::initialized(),
         "Madness was already initialized.  I don't know what to do...");
   if(NThreads_!=0){//Need to throttle number of threads
      std::stringstream Variable;
      //The only way I know of to set the number of threads is via environment
      //variables
      Variable<<NThreads_;
      std::cout<<"MAD_NUM_THREADS="<<Variable.str().c_str()<<std::endl;
      PARALLEL_ASSERT(
            setenv("MAD_NUM_THREADS",Variable.str().c_str(),0)==0,
            "Failed to set the number of threads");
   }
   Comm_=new Communicator(madness::initialize(argc,argv));
}

Environment::~Environment(){
   Comm_->Barrier();
   delete Comm_;Comm_=nullptr;
   //Really should check if this has been called,
   //but madness doesn't seem to support that
   madness::finalize();
}

Communicator& Environment::Comm(){
   PARALLEL_ASSERT((Comm_!=nullptr),"Communicator is not set");
   return *Comm_;
}
}}//End namespaces

