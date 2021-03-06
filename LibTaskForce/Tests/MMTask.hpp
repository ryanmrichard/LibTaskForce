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

/** \file MMTask.hpp
 *  \brief Write Me!!!!!!
 *  \author Ryan M. Richard
 *  \version 1.0
 *  \date June 16, 2016
 */

#ifndef LIBTASKFORCE_GUARD_MMTASK_HPP
#define LIBTASKFORCE_GUARD_MMTASK_HPP

#include<vector>

using Matrix_t=std::vector<double>;
using Buffer_t=std::vector<Matrix_t>;

/* Here we do a nieve matrix multiplication by blocks.  We start with an N by N
 * matrix and break it up in to m^2 blocks.  For simplicity we assume
 * N%m==0.  This means each block is N/m elements by N/m elements.
 * 
 * We label the blocks 0 to (N/m)^2-1 left to right, top to bottom.
 * If we are told to compute block i, then defining j=i%m and k=(i-j)/m this 
 * means our block is comprised of rows k*(N/m) to (k+1)*(N/m) and columns 
 * j*(N/m) to (j+1)*(N/m)
 *
 */
struct MMTask{
    const size_t N_,BlockSide_,BlockCol_,BlockRow_;
    const Matrix_t& Matrix_;
    MMTask(size_t N,size_t M,size_t Me,Matrix_t& Matrix):
        N_(N),BlockSide_(N/M),BlockCol_(Me%M),BlockRow_((Me-BlockCol_)/M),
        Matrix_(Matrix)
    {}
    
    template<typename T>
    Matrix_t operator()(T&)const
    {
        const size_t RowStart=BlockRow_*BlockSide_,RowEnd=RowStart+BlockSide_;
        const size_t ColStart=BlockCol_*BlockSide_,ColEnd=ColStart+BlockSide_;
        Matrix_t Result(BlockSide_*BlockSide_);
        for(size_t i=RowStart;i<RowEnd;++i){
            const size_t I=i-RowStart;
            for(size_t j=ColStart;j<ColEnd;++j){
                const size_t J=j-ColStart;
                for(size_t k=0;k<N_;++k)
                    Result[I*BlockSide_+J]+=Matrix_[i*N_+k]*Matrix_[k*N_+j];
            }
        }
        return Result;
    }
};


double MMError(size_t N,size_t M,
                const Buffer_t& DistBuffer,const Buffer_t& SerialBuffer){
    double TwoNorm=0.0;
    const size_t BlockSide=N/M;
    for(size_t i=0;i<DistBuffer.size();++i){
        for(size_t j=0;j<BlockSide;++j){
            for(size_t k=0;k<BlockSide;++k){
                double Temp=DistBuffer[i][j*BlockSide+k]-
                            SerialBuffer[i][j*BlockSide+k];
               TwoNorm+=Temp*Temp;
            }
        }
    }
    return sqrt(TwoNorm/(double)N*(double)N);
}

#endif /* LIBTASKFORCE_GUARD_MMTASK_HPP */

