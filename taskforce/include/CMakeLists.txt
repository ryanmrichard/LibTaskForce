set(ParallelHeaders CommStats.hpp
                    Communicator.hpp
                    DVector.hpp
                    Environment.hpp
                    Future.hpp
                    LibParallel.hpp
                    MadnessHelper.hpp
                    ParallelAssert.hpp
                    TaskResults.hpp
                    Timer.hpp
)
install(FILES ${ParallelHeaders} DESTINATION include)
