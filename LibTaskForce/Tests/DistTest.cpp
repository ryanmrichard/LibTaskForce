/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <tbb/tick_count.h>
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
    const size_t N=(argc>1?(size_t)std::atoi(argv[1]):10);
    const size_t M=(argc>2?(size_t)std::atoi(argv[2]):2);
    assert(N%M==0);
    
    bool AllPassed=true;
    
    std::shared_ptr<ProcessEnv> World(std::make_shared<ProcessEnv>());
    const ProcessComm& NewComm=World->comm();
    std::unique_ptr<ProcessComm> Comm=NewComm.split();
    
    if(NewComm.rank()==0)
        std::cout<<"NProcesses: "<<NewComm.size()<<std::endl
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
    
    std::vector<ProcessFuture<Matrix_t>> DistTemp(M*M);
    t0=tbb::tick_count::now();
    for(size_t i=0;i<M*M;++i)
        DistTemp[i]=std::move(Comm->add_task<Matrix_t>(MMTask(N,M,i,Matrix)));
    for(size_t i=0;i<M*M;++i)DistBuffer[i]=DistTemp[i].get();
    t1=tbb::tick_count::now();
    DistTime=(t1-t0).seconds();
    
    const double TwoNorm=sqrt(MMError(N,M,DistBuffer,SerialBuffer)/N*N);
    AllPassed=(AllPassed&& TwoNorm<1e-6);
    if(NewComm.rank()==0)
        std::cout<<"Standard deviation between resulting matrices: "
                  <<TwoNorm<<std::endl;
    
    
    if(NewComm.rank()==0)
        std::cout<<"Distributed time for squaring matrix: "<<DistTime
                 <<std::endl<<"Speedup: "<<SerialTime/DistTime
                 <<" %Efficiency: "<<100/NewComm.size()*(SerialTime/DistTime)
                 <<std::endl;
        
    return AllPassed?0:1;
}