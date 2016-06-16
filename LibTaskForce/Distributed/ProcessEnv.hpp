#ifndef PARALLEL_ENVIRONMENT_HPP_
#define PARALLEL_ENVIRONMENT_HPP_

#include<stack>
#include<memory>
#include <mpi.h>
#include "LibTaskForce/General/GeneralEnv.hpp"

namespace LibTaskForce{

class ProcessComm;

/** \brief A class to manage our MPI parallelism
 *
 *  At the moment we have
 *  communicators that schedule your tasks for you.  When scheduling parallel
 *  tasks all scheduling goes through the communicator.
 * 
 *  I see no reason why multiple instances of this class can't exist, but I
 *  have not tested it or come up with a scenario under which that makes
 *  sense.
 */
class ProcessEnv: public GeneralEnv<ProcessComm>{
   public:      
      /** \brief Constructor for environment based on possibly already started
       *   MPI
       * 
       *  This makes an environment based around a possibly already started MPI
       *  instance
       *  and will not tear MPI down.  Obviously,  MPI_Init() should have
       *  already been called.  May throw if your MPI library was compiled
       *  with throwing enabled.
       * 
       *  \param[in] Comm      The MPI communicator that we should attach to
       *                       We will take ownership if we start MPI
       */
      explicit ProcessEnv(const MPI_Comm& Comm=MPI_COMM_WORLD);

      ///Shuts down MPI if we started it
      ~ProcessEnv();

      std::string print()const;
   private:
       bool WeStartedMPI_;///<True if we started MPI (and have to destroy it)
            
      ///Allows communicators to register/release themselves
      friend ProcessComm;
};

inline std::ostream& operator<<(std::ostream& os, const ProcessEnv& Env){
    return os<<Env.print();
}

}//End namespaces

#endif /* PARALLEL_ENVIRONMENT_HPP_ */
