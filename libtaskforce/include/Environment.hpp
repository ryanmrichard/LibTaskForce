#ifndef PARALLEL_ENVIRONMENT_HPP_
#define PARALLEL_ENVIRONMENT_HPP_

#include<stack>
#include<memory>
#include <mpi.h>
namespace LibTaskForce{

class Communicator;

/** \brief A class to manage our parallel environment
 *
 *  At the moment our parallelism is wrapped around Madness's parallel
 *  library.  Under their philosophy, which I rather like, you have
 *  communicators that schedule your tasks for you.  When scheduling parallel
 *  tasks all scheduling goes through the communicator.  The current class
 *  is sort of the home base for the communicators.  In particular it keeps
 *  track of what resources are out and what are available.
 * 
 *  I see no reason why multiple instances of this class can't exist, but I
 *  have not tested it or come up with a scenario under which that makes
 *  sense.
 */
class Environment{
   public:
      /** \brief Sets up our environment
       * 
       *  This is the setup call if MPI has not been initialized.  If it has,
       *  use the other constructor.  Starts Madness as well.  If MPI has been
       *  started this may crash or throw (depends on how you compiled your
       *  MPI version).
       * 
       *  \param[in] NThreads the number of threads (use 0 for this to have it
       *                      be determined automatically).
       */
      Environment(size_t NThreads=0);
      
      /** \brief Constructor for environment based on already started MPI
       * 
       *  This makes an environment based around an already started MPI instance
       *  and will not tear MPI down.  Obviously,  MPI_Init() should have
       *  already been called.  May throw if your MPI library was compiled
       *  with throwing enabled.
       * 
       *  \param[in] NThreads The number of threads we can use (0 will have this
       *                      determined automatically)
       *  \param[in] Comm      The MPI communicator that we should pretend is
       *                       our MPI_COMM_WORLD.  We will not take ownership
       *                       of it so you are responsible for ensuring it
       *                       outlives this class.
       */
      explicit Environment(size_t NThreads, const MPI_Comm& Comm);

      ///Shuts down madness, only call at end of program
      ~Environment();

      /** \brief Returns the current communicator
       * 
       *   For all intents and purposes, asking for a communicator is like
       *   asking for memory, it's a managed resource.  Unlike memory, the
       *   operating system doesn't manage it for you.  The point of this
       *   library is to offload that responsibility to it.  When you are done
       *   with resources you need to give them back, just like you do with
       *   memory.  If you ask for all of the memory on a system and never
       *   free it up, no one else can use it.  The same goes for the parallel
       *   resources, only that it is more important to free them up because
       *   you are more likely to be using nearly all of the resources.
       *   
       *   Attempting to access parallel resources that have been released
       *   yields the equivalent of a segfault, failing to free up
       *   resources is equivalent to a memory leak, and trying to free them
       *   up twice is a double-free.  I think I have come up with a solution
       *   that prevents any of these from happening.  When you call this
       *   function you are given a communicator that represents the currently
       *   available resources.  You then ask for what you need (feeling free
       *   to take all available resources).  The result is a new communicator
       *   given to you as a unique_ptr.  Since communicators can not be
       *   copied, assigned, or moved by users, when that unique_ptr goes out
       *   of scope the resources will be released ensuring no resource leaks.
       *   You can be assured that the same thing is occurring in all functions
       *   below you (assuming they use this library for the parallel aspect)
       *   as can the functions above you.
       * 
       *   \return An unmodifiable communicator representing the state of the
       *           parallel environment.
       */
      const Communicator& Comm()const;
   private:
      ///The number of threads the program was started with
      const size_t NThreads_;
      ///The type of a Communicator
      typedef std::unique_ptr<Communicator> Comm_t;
      ///The communicators
      std::stack<Comm_t> Comms_;
      
      ///Debugging purposes only.  The n-th comm in here is the comm that made
      ///the n+1-th comm in Comms_
      std::stack<const Communicator*> ParentComm_;
      
      ///Allows communicators to register/release themselves
      friend Communicator;
      /** \brief Registers a newly made Communicator with the environment
       * 
       *  Basically a communicator was split and we now gave up some resources.
       *  What we have to do is make a new communicator that reflects the
       *  resources we have left, which is the communicator we will give
       *  out on subsequent calls to Comm().
       * 
       *  The new communicator literally is the number of processes on what
       *  was the current comm, less those on the Comm2Register and
       *  the number of threads on the current comm less the number on
       *  Comm2Register.  MPI really helps us out here because the intracomm
       *  inside Comm2Register is the active MPI comm all we have to do is
       *  worry about the threads.  Only the comm that is split off is 
       *  registered, it is also responsible for remembering to call release.
       */
      void Register(const Communicator& Comm2Register);
      /** \brief Adds the resources back into the environment
       * 
       *  This is much easier than the register function.  Basically we
       *  just pop the last communicator off the stack, and if we are in
       *  debug mode, make sure no one kept a communicator out.  
       * 
       *  Any comm that was registered should call this function upon deletion
       */
      void Release(const Communicator& Comm2Release);

      
      
      
};


}//End namespaces

#endif /* PARALLEL_ENVIRONMENT_HPP_ */
