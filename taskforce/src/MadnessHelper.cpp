/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sstream>
#include "MadnessHelper.hpp"
#include "ParallelAssert.hpp"

namespace LibTaskForce{
void SetMadThreads(size_t NThreads){
    std::stringstream Variable;
    Variable<<NThreads;
    PARALLEL_ASSERT(
       setenv("MAD_NUM_THREADS",Variable.str().c_str(),1)==0,
              "Failed to set the number of threads");
}



}//End namespace