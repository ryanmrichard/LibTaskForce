/*
 *@BEGIN LICENSE
 *
 * PSI4: an ab initio quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *@END LICENSE
 */
#include <cstdlib>
#include "madness.h"
#include "Environment.hpp"
#include "Communicator.hpp"
#include "ParallelAssert.hpp"

namespace bpmodule{
namespace LibParallel{



Environment::Environment(size_t NThreads,int argc, char** argv):
      argc_(argc),argv_(argv),NThreads_(NThreads){
   PARALLEL_ASSERT(madness::initialized(),
         "Madness was already initialized.  I don't know what to do...");
   if(NThreads_!=0){//Need to throttle number of threads
      std::stringstream Variable;
      //The only way I know of to set the number of threads...
      Variable<<"MAD_NUM_THREADS="<<NThreads_<<std::endl;
      //Cry siliently that we have to remove the const...
      PARALLEL_ASSERT(
            putenv(const_cast<char *>(Variable.str().c_str()))!=0,
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
   PARALLEL_ASSERT((Comm_==nullptr),"Communicator is not set");
   return *Comm_;
}
}}//End namespaces

