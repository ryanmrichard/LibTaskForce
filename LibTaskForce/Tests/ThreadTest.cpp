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
#include <memory>
#include "LibTaskForce/LibTaskForce.hpp"

const std::array<size_t,39> FibNums={
 0,1,1,2,3,5,8,13,21,34,55,89,144,233,377,610,987,
 1597,2584,4181,6765,10946,17711,28657,46368,75025,
 121393,196418,317811,514229,832040,1346269,
 2178309,3524578,5702887,9227465,14930352,24157817,
 39088169
};

using namespace LibTaskForce;

//Functor for computing Fibonacci number (tests add_task())
struct FibTask{
    //Unique_ptr used to ensure move semantics work
    std::unique_ptr<size_t> N_;
    FibTask(size_t N):
        N_(new size_t)
    {
        *N_=N;
    }
        
    size_t operator()(ThreadComm& Comm)const
    {
        if(*N_<2)return *N_;
        ThreadFuture<size_t> x=Comm.add_task<size_t>(FibTask(*N_-1));
        ThreadFuture<size_t> y=Comm.add_task<size_t>(FibTask(*N_-2));
        size_t RVal=x.get(),LVal=y.get();
        return RVal+LVal;
    }
};

//Functor for testing reduce() simply adds numbers together
struct MyReduceTask{
    double operator()(std::vector<double>::const_iterator itr)const
    {
        return *itr;
    }
    double operator()(double x,double y)const
    {
        return x+y;
    }
    
};

int main(int argc,char** argv){
    if(argc<1)
    {
        std::cerr<<
           "Usage: ThreadTest <NThreads> [Fibonacci] [Sum]"<<std::endl<<
           "NThreads  (int) :  Number of threads to use"<<std::endl<<
           "Fibonacci (int) : (optional) What Fibonacci number to compute"<<std::endl<<
           "Sum       (int) : (optional) Compute the sum of 1 to what number?"<<std::endl;                
        return 1;
    }
    
    const size_t NThreads=(size_t)std::atoi(argv[1]);
    const size_t N=(argc>2?(size_t)std::atoi(argv[2]):30);
    const size_t SumMax=(argc>3?(size_t)std::atoi(argv[3]):(size_t)1e9);
    
    ThreadEnv Env(NThreads);
    const ThreadComm& OrigComm=Env.comm();
    std::unique_ptr<ThreadComm> NewComm=OrigComm.split();
    std::cout<<"NThreads: "<<NewComm->size()<<std::endl;
    std::cout<<"Computing the "<<N<<"-th Fibonacci number"<<std::endl;
    
    tbb::tick_count t0=tbb::tick_count::now();
    ThreadFuture<size_t> DaNum=NewComm->add_task<size_t>(FibTask(N));
    size_t Num=DaNum.get();
    tbb::tick_count t1=tbb::tick_count::now();
    std::cout<<"Wall time: "<<(t1-t0).seconds()<<std::endl;
    if(FibNums[N]!=Num)
        throw std::runtime_error("Fibonacci number was wrong\n");

    std::vector<double> Vec(SumMax);
    const double Max=(double)SumMax;
    const double TheoryValue=Max*(Max+1.0)/2.0;
    std::iota(Vec.begin(),Vec.end(),1);
    
    std::cout<<"Computing the sum of the numbers [1,"<<SumMax<<"]"<<std::endl;
    t0=tbb::tick_count::now();
    double DaSum=NewComm->reduce<double>(MyReduceTask(),Vec.begin(),Vec.end());
    t1=tbb::tick_count::now();
    if(fabs(100.0*(DaSum-TheoryValue)/TheoryValue)>1e-5)
        throw std::runtime_error("Summation added up to the wrong value\n");
    std::cout<<"Wall time: "<<(t1-t0).seconds()<<std::endl;
    return 0;
}