set(ParallelSrc Communicator.cpp
                Environment.cpp
                MadnessHelper.cpp
                Util/Timer.cpp
)
set(ParallelHeaders Communicator.hpp
                    Environment.hpp
                    Future.hpp
                    LibParallel.hpp
                    MadnessHelper.hpp
                    ParallelAssert.hpp
                    Util/Timer.hpp
)
add_library(parallel SHARED ${ParallelSrc})
include_directories(${MADNESS_INCLUDE_DIRS} ${MPI_C_INCLUDE_PATH})
install(TARGETS parallel DESTINATION lib)
install(FILES ${ParallelHeaders} DESTINATION include)
