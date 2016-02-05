#include "Communicator.hpp"
#include "madness/world/MADworld.h"
namespace bpmodule{
namespace LibParallel{

Communicator::Communicator(madness::World& world):World_(world){

}


void Communicator::Barrier()const{
   World_.gop.fence();
}

}}//End namespaces

