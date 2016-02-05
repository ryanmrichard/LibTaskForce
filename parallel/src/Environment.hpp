#ifndef PARALLEL_ENVIRONMENT_HPP_
#define PARALLEL_ENVIRONMENT_HPP_

namespace LibTaskForce{

class Communicator;

/** \brief A class to manage our parallel environment
 *
 *  At the moment our parallelism is wrapped around Madness's parallel
 *  library.  Under their philosophy, which I rather like, you have
 *  communicators that schedule your tasks for you.  When trying to do a
 *  parallel run, you first ask the environment for a communicator,
 *  and you will be given the current communicator.  You may then use all
 *  of the resources on that communicator or split some off.
 *
 *
 */
class Environment{
   public:
      ///Sets up an environment with NThreads threads (use 0 for this to
      ///be determined automatically)
      Environment(size_t NThreads=0,int argc=NULL, char** argv=NULL);

      ///Shuts down madness, only call at end of program
      ~Environment();

      ///Returns the current communicator
      Communicator& Comm();
   private:
      ///The number of arguments the program was started with
      int argc_;
      ///A list of the arguments the program was started with
      char** argv_;
      ///The number of threads the program was started with
      size_t NThreads_;
      ///The initial communicator
      Communicator* Comm_;
};


}//End namespaces

#endif /* PARALLEL_ENVIRONMENT_HPP_ */
