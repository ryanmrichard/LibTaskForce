/*  
 *   LibTaskForce: An open-source library for task-based parallelism
 * 
 *   Copyright (C) 2016 Ryan M. Richard
 * 
 *   This file is part of LibTaskForce.
 *
 *   LibTaskForce is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   LibTaskForce is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with LibTaskForce.  If not, see <http://www.gnu.org/licenses/>.
 */ 

/** \file MPIWrappers.hpp
 *  \brief C++-ified wrappers around MPI's usual calls
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 16, 2016
 */

#ifndef LIBTASKFORCE_GUARD_MPIWRAPPERS_HPP
#define LIBTASKFORCE_GUARD_MPIWRAPPERS_HPP

#include <mpi.h>
#include "LibTaskForce/Util/Serialization.hpp"
#include "LibTaskForce/Util/ParallelAssert.hpp"

namespace LibTaskForce {

enum IDs {
    ROOT_PROCESS = 0
}; ///< enum for setting the root process

enum Tags {
    GENERIC_TAG = 123,
    GENERIC_SIZE = 999
}; ///< Enums for message tags

inline size_t rank(MPI_Comm Comm){
    int DaRank;
    MPI_Comm_rank(Comm,&DaRank);
    return static_cast<size_t>(DaRank);
}

inline size_t size(MPI_Comm Comm){
    int DaSize;
    MPI_Comm_size(Comm,&DaSize);
    return static_cast<size_t>(DaSize);
}

template<typename T>
void send(const T& Data, size_t RecvID, MPI_Comm Comm, size_t MsgTag = GENERIC_TAG)
{
    binary_type SerializedData = serialize(Data);
    int Length = (int) SerializedData.size();
    MPI_Send(&Length, 1, MPI_INT, (int) RecvID, GENERIC_SIZE, Comm);
    MPI_Send(SerializedData.data(), Length, MPI_BYTE, (int) RecvID, (int) MsgTag, Comm);
}

template<typename T>
void recv(T& Data, size_t SenderID, MPI_Comm Comm, size_t MsgTag = GENERIC_TAG)
{
    const int Sender = (int) SenderID;
    int Length;
    MPI_Recv(&Length, 1, MPI_INT, Sender, GENERIC_SIZE, Comm, MPI_STATUS_IGNORE);
    binary_type BinData(Length);
    MPI_Recv(BinData.data(), Length, MPI_BYTE, Sender, (int) MsgTag, Comm, MPI_STATUS_IGNORE);
    Data = deserialize<T>(BinData);
}

template<typename T>
void bcast(T& Data, MPI_Comm Comm, size_t RootID = ROOT_PROCESS)
{
    binary_type BinData;
    
    if (rank(Comm) == RootID)BinData = serialize(Data);
    int Length = (int) BinData.size();
    const int Root = (int) RootID;
    int Error = MPI_Bcast(&Length, 1, MPI_INT, Root, Comm);
    PARALLEL_ASSERT(Error == MPI_SUCCESS, "Broadcast failed");
    if (rank(Comm) != RootID)BinData = binary_type(Length);
    Error = MPI_Bcast(BinData.data(), Length, MPI_BYTE, Root, Comm);
    PARALLEL_ASSERT(Error == MPI_SUCCESS, "Broadcast failed");
    if (rank(Comm) != RootID)Data = deserialize<T>(BinData);
}

template<typename T>
T all_gatherv(T& Data,MPI_Comm Comm)
{
    binary_type BinData = serialize(Data);
    int Length = (int) BinData.size();
    std::vector<int> Lengths(size(Comm));
    MPI_Allgather(&Length, 1, MPI_INT, Lengths.data(), 1, MPI_INT, Comm);
    int Total = 0;
    std::vector<int> Displacements(size(Comm));
    for (size_t i = 1; i < Lengths.size(); ++i) {
        Total += Lengths[i - 1];
        Displacements[i] = Displacements[i - 1] + Lengths[i - 1];
    }
    binary_type Buffer(Total);
    MPI_Allgatherv(BinData.data(), Length, MPI_BYTE, Buffer.data(),
            Lengths.data(), Displacements.data(), MPI_BYTE, Comm);
    return deserialize<T>(Buffer);
}



}//End namespace LibTaskForce
#endif /* LIBTASKFORCE_GHUARD_MPIWRAPPERS_HPP */

