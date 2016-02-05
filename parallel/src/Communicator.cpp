#include "Communicator.hpp"
#include "madness/world/MADworld.h"
namespace LibTaskForce{

Communicator::Communicator(madness::World& world):World_(world){

}


void Communicator::Barrier()const{
   World_.gop.fence();
}

}//End namespaces

