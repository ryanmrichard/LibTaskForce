#include <iostream>
#include<chrono>
#include<thread>
#include<functional>
#include<cstdlib>
#include "LibParallel.hpp"
#include "Util/Timer.hpp"

double SumValues(double rhs,double lhs){
    std::chrono::milliseconds timespan(5000);
    std::this_thread::sleep_for(timespan);
    return rhs+lhs;
}

bool PrintValue(double value){
    std::cout<<value<<std::endl;
    return true;
}

namespace Par=LibTaskForce;
int main(int argc,char** argv){
    assert(argc==2);
    size_t NThreads=atoi(argv[1]);
   Par::Environment DaEnv(NThreads);
   const Par::Communicator& Comm=DaEnv.Comm();
   {
       std::cout<<Comm<<std::endl;
       const std::unique_ptr<Par::Communicator> NewComm=Comm.Split(1,2);
       std::cout<<*NewComm<<std::endl;
   }
    
    
   /*Par::Util::SmartTimer TotalTimer("Total Timer"); 
   size_t NThreads=1;
   Par::Environment DaEnv(NThreads);
   Par::Communicator& Comm=DaEnv.Comm();
   
   std::vector<Par::Future<double>> Ftrs;
   Par::Util::SmartTimer FillTimer("Fill Timer");
   std::vector<double> Values;
   for(size_t i=0;i<100;++i){
       Values.push_back(((double)i)*1.2);
       Ftrs.push_back(
          Comm.AddTask(SumValues,((double)i)*1.2,((double)i)*2.2));
   }
   Par::Future<double> Result=
       Comm.Reduce<double>(Values.begin(),Values.end(),std::plus<double>());
   Par::Future<bool> AllPrinted=
       Comm.ForEach<double>(Values.begin(),Values.begin()+3,
           std::function<bool(double)>(PrintValue));
   FillTimer.Stop();
   
   double sum=0;
   for(size_t i=0;i<Ftrs.size();i++)sum+=*(Ftrs[i]);
   std::cout<<*Result<<std::endl;//Should be 5940
   TotalTimer.Stop();
   
   std::cout<<FillTimer<<std::endl;
   std::cout<<TotalTimer<<std::endl;*/
   return 0;
}
