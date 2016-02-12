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
   std::unique_ptr<Par::Communicator> MyComm=Comm.Split(1,1);
    
    
   Par::Util::SmartTimer TotalTimer("Total Timer");
   std::vector<Par::Future<double>> Ftrs;
   Par::Util::SmartTimer FillTimer("Fill Timer");
   std::vector<double> Values;
   for(size_t i=0;i<50;++i){
       Values.push_back(((double)i)*1.2);
       Ftrs.push_back(
          MyComm->AddTask(SumValues,((double)i)*1.2,((double)i)*2.2));
   }
   Par::Future<double> Result=
       MyComm->Reduce<double>(Values.begin(),Values.end(),std::plus<double>());
   Par::Future<bool> AllPrinted=
       MyComm->ForEach<double>(Values.begin(),Values.begin()+3,
           std::function<bool(double)>(PrintValue));
   FillTimer.Stop();
   
   double sum=0;
   for(size_t i=0;i<Ftrs.size();i++)sum+=*(Ftrs[i]);
   double TheResult=*Result;
   TotalTimer.Stop();
   std::cout<<sum<<std::endl;
   //PARALLEL_ASSERT(abs(sum-5940)<0.01,
   //     "Thre result of summing our futures from add_tasks was not");
   PARALLEL_ASSERT(abs(TheResult-1470)<0.01,
                   "The result of reduction failed to be 5940");
   std::cout<<"All tests passed!!!!!"<<std::endl;
   std::cout<<FillTimer<<std::endl;
   std::cout<<TotalTimer<<std::endl;
   return 0;
}
