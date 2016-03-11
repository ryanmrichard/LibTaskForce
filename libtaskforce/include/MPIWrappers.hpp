/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MPIWrappers.hpp
 * Author: richard
 *
 * Created on March 11, 2016, 2:25 PM
 */

#ifndef MPIWRAPPERS_HPP
#define MPIWRAPPERS_HPP

template<typename T>
void Communicator::Bcast(T& Data,size_t Root)const{
          Binary_t BinData;
          if(Rank()==Root)BinData=Serialize(Data);
          size_t Length=BinData.size();
          int Error=MPI_Bcast((int*)(&Length),1,MPI_INT,(int)Root,MPIComm());
          PARALLEL_ASSERT(Error==MPI_SUCCESS,"Broadcast failed");
          if(Rank()!=Root)BinData=Binary_t(Length);
          Error=MPI_Bcast(BinData.data(),(int)Length,MPI_BYTE,(int)Root,MPIComm());
          PARALLEL_ASSERT(Error==MPI_SUCCESS,"Broadcast failed");
          if(Rank()!=Root)Data=DeSerialize<T>(BinData);
}

template<typename T>
void Communicator::Recv(T& Data,size_t Sender,size_t MsgTag)const{
          size_t Length;
          MPI_Recv((int*)(&Length),1,MPI_INT,(int)Sender,999,
                   MPIComm(),MPI_STATUS_IGNORE);
          Binary_t BinData(Length);
          MPI_Recv(BinData.data(),(int)Length,MPI_BYTE,(int)Sender,
                   (int)MsgTag,MPIComm(),MPI_STATUS_IGNORE);
          Data=DeSerialize<T>(BinData);
}

template<typename T>
void Communicator::Send(const T& Data,size_t Recv, size_t MsgTag)const{
          Binary_t BinData=Serialize(Data);
          size_t Length=BinData.size();
          MPI_Send((int *)(&Length),1,MPI_INT,(int)Recv,999,MPIComm());
          MPI_Send(BinData.data(),(int)Length,MPI_BYTE,(int)Recv,
                   (int)MsgTag,MPIComm());
}
#endif /* MPIWRAPPERS_HPP */

