set(ParallelSrc Communicator.cpp
                Environment.cpp
                MadnessHelper.cpp
                Timer.cpp
)

add_library(taskforce SHARED ${ParallelSrc})
include_directories(${MADNESS_INCLUDE_DIRS} ${MPI_C_INCLUDE_PATH} ../include)
install(TARGETS taskforce DESTINATION lib)
