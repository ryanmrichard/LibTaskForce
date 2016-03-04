#include <iostream>
#include<chrono>
#include<thread>
#include<functional>
#include<cstdlib>
#include "LibParallel.hpp"
#include "Timer.hpp"


double SumValues(double rhs,double lhs){
    std::chrono::milliseconds timespan(1000);
    std::this_thread::sleep_for(timespan);
    return rhs+lhs;
}

bool PrintValue(double value){
    std::cout<<value<<std::endl;
    return true;
}

class SumSquares{
public:
    template<typename T>
    double operator()(const T& Itr)const{return (*Itr)*(*Itr);}
    double operator()(double lhs,double rhs)const{return lhs+rhs;}
};

namespace Par=LibTaskForce;
int main(int argc,char** argv){
    assert(argc==2);
    size_t NThreads=atoi(argv[1]);
   Par::Environment DaEnv(NThreads);
   const Par::Communicator& Comm=DaEnv.Comm();
   Par::Communicator MyComm=Comm.Split(1,1);
   std::cout<<MyComm<<std::endl; 
    
   Par::Util::SmartTimer TotalTimer("Total Timer");
   Par::TaskResults<double> Ftrs(MyComm);
   Par::Util::SmartTimer FillTimer("Fill Timer");
   std::vector<double> Values;
   for(size_t i=0;i<50;++i){
       Values.push_back(((double)i)*1.2);
       Ftrs.push_back(
          MyComm.AddTask(SumValues,((double)i)*1.2,((double)i)*2.2));
   }
   Par::Future<double> Result=
       MyComm.Reduce<double>(Values.begin(),Values.end(),SumSquares());
   Par::Future<bool> AllPrinted=
      MyComm.ForEach(Values.begin(),Values.begin()+3,
           std::function<bool(double)>(PrintValue));
   FillTimer.Stop();
   
   //PARALLEL_ASSERT(*AllPrinted,"One of the for-each tasks failed");
   std::cout<<" Printing complete"<<std::endl;
   double sum=0;
   for(size_t i=0;i<Ftrs.size();i++)sum+=Ftrs[i];
   //double TheResult=*Result;
   TotalTimer.Stop();
   PARALLEL_ASSERT(abs(sum-4165)<0.01,
        "The result of summing our futures from add_tasks was not 4165");
   //PARALLEL_ASSERT(abs(TheResult-58212)<0.01,
   //                "The result of reduction failed to be 58,212");
   std::cout<<"All tests passed!!!!!"<<std::endl;
   std::cout<<FillTimer<<std::endl;
   std::cout<<TotalTimer<<std::endl;
   return 0;
}
