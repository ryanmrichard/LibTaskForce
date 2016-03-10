set(LibTaskForceHeaders CommStats.hpp
                    Communicator.hpp
                    DVector.hpp
                    Environment.hpp
                    Future.hpp
                    LibTaskForce.hpp
                    MadnessHelper.hpp
                    ParallelAssert.hpp
                    Serialization.hpp
                    TaskResults.hpp
                    Timer.hpp
)
install(DIRECTORY cereal DESTINATION include)
install(FILES ${LibTaskForceHeaders} DESTINATION include)
