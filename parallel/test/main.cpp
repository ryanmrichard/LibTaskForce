#include <iostream>
#include<chrono>
#include<thread>
#include "LibParallel.hpp"
#include "Util/Timer.hpp"

double SumValues(double rhs,double lhs){
    std::chrono::milliseconds timespan(5000);
    std::this_thread::sleep_for(timespan);
    return rhs+lhs;
}

namespace Par=LibTaskForce;
int main(int argc,char** argv){
   Par::Util::SmartTimer TotalTimer("Total Timer"); 
   size_t NThreads=2;
   Par::Environment DaEnv(NThreads);
   Par::Communicator& Comm=DaEnv.Comm();
   
   std::vector<Par::Future<double>> Ftrs;
   Par::Util::SmartTimer FillTimer("Fill Timer");
   for(size_t i=0;i<100;++i){
   Ftrs.push_back(
       Comm.AddTask(SumValues,((double)i)*1.2,((double)i)*2.2));
   }
   FillTimer.Stop();
   
   double sum=0;
   for(size_t i=0;i<Ftrs.size();i++)sum+=*(Ftrs[i]);
   TotalTimer.Stop();
   
   std::cout<<FillTimer<<std::endl;
   std::cout<<TotalTimer<<std::endl;
   return 0;
}
