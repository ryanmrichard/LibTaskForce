#include <cstdlib>
#include "LibTaskForce/Distributed/ProcessEnv.hpp"
#include "LibTaskForce/Distributed/ProcessComm.hpp"
#include "LibTaskForce/Util/ParallelAssert.hpp"

namespace LibTaskForce{  
    
ProcessEnv::ProcessEnv(const MPI_Comm& Comm):
    WeStartedMPI_(false){
    int Temp;
    MPI_Initialized(&Temp);
    WeStartedMPI_=!(static_cast<bool>(Temp));
    if(WeStartedMPI_)MPI_Init(nullptr,nullptr);
    FirstComm_=std::unique_ptr<ProcessComm>(new ProcessComm(Comm,this));
}


ProcessEnv::~ProcessEnv(){
    if(!WeStartedMPI_)return;
    int AlreadyFinalized;
    MPI_Finalized(&AlreadyFinalized);
    if(!AlreadyFinalized)MPI_Finalize();
}

std::string ProcessEnv::print()const
{
    std::stringstream ss;
    ss<<"Environment has "<<Comms_.size()<<" nested comms at the moment"
      <<std::endl;
    return ss.str();
}

}//End namespaces

