set(LibTaskForceHeaders 
                    AddTask.hpp
                    CommStats.hpp
                    Communicator.hpp
                    DVector.hpp
                    Environment.hpp
                    Future.hpp
                    LibTaskForce.hpp
                    MadnessHelper.hpp
                    MPIWrappers.hpp
                    ParallelAssert.hpp
                    Reduce.hpp
                    Serialization.hpp
                    TaskResults.hpp
                    Timer.hpp
)
install(DIRECTORY LTFcereal DESTINATION include)
install(FILES ${LibTaskForceHeaders} DESTINATION include)
