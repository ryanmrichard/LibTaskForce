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

#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <cmath>
#include "LibTaskForce/LibTaskForce.hpp"
#include "LibTaskForce/Tests/MMTask.hpp"

using namespace LibTaskForce;
using Matrix_t=std::vector<double>;

int main(int argc,char** argv){
    const size_t NThreads=(size_t)std::atoi(argv[1]);
    const size_t N=(argc>2?(size_t)std::atoi(argv[2]):10);
    const size_t M=(argc>3?(size_t)std::atoi(argv[3]):2);
    assert(N%M==0);
    
    bool AllPassed=true;
    
    std::shared_ptr<HybridEnv> World(std::make_shared<HybridEnv>(NThreads));
    const HybridComm& NewComm=World->comm();
    std::unique_ptr<HybridComm> Comm=NewComm.split();
    
    if(NewComm.rank()==0)
        std::cout<<"NProcesses: "<<NewComm.nprocs()<<std::endl
                 <<"NThreads:   "<<NewComm.nthreads()<<std::endl
                 <<"Squaring a "<<N<<" by "<<N<<" matrix decomposed into "<<M*M
                 <<" blocks"<<std::endl;
    
    Matrix_t Matrix;
    for(size_t i=0;i<N*N;++i)Matrix.push_back(rand()%10);
        
    std::vector<Matrix_t> SerialBuffer(M*M),DistBuffer(M*M);
    tbb::tick_count t0=tbb::tick_count::now();
    for(size_t i=0;i<M*M;++i)SerialBuffer[i]=MMTask(N,M,i,Matrix)(*Comm);
    tbb::tick_count t1=tbb::tick_count::now();
    double SerialTime=(t1-t0).seconds(),DistTime;
    
    if(NewComm.rank()==0)
        std::cout<<"Serial time for squaring matrix: "<<SerialTime<<std::endl;
    
    std::vector<HybridFuture<Matrix_t>> DistTemp(M*M);
    t0=tbb::tick_count::now();
    for(size_t i=0;i<M*M;++i)
        DistTemp[i]=std::move(Comm->add_task<Matrix_t>(MMTask(N,M,i,Matrix)));
    for(size_t i=0;i<M*M;++i)DistBuffer[i]=DistTemp[i].get();
    t1=tbb::tick_count::now();
    DistTime=(t1-t0).seconds();
    
    const double TwoNorm=MMError(N,M,DistBuffer,SerialBuffer);
    AllPassed=(AllPassed&& TwoNorm<1e-6);
    
    if(NewComm.rank()==0)
        std::cout<<"Standard deviation between resulting matrices: "
                  <<TwoNorm<<std::endl;
    
    
    if(NewComm.rank()==0)
        std::cout<<"Distributed time for squaring matrix: "<<DistTime
                 <<std::endl<<"Speedup: "<<SerialTime/DistTime
                 <<" %Efficiency: "<<100.0/(double)NewComm.size()*(SerialTime/DistTime)
                 <<std::endl;

    return AllPassed?0:1;
}