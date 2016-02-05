#include<sstream>
#include "Timer.hpp"

namespace Util{
    
SmartTimer::SmartTimer(const std::string& Name, bool IsParallel):
    Name_(Name),IsParallel_(IsParallel),IsStopped_(true){
    Start();
}

void SmartTimer::Start(){
    IsStopped_=false;
    for(size_t i=0;i<Offsets_.size();i++)Offsets_[i]=0.0;
    StartTimes_[0]=std::chrono::high_resolution_clock::now();
}

void SmartTimer::Stop(){
    StopTimes_[0]=std::chrono::high_resolution_clock::now();
    IsStopped_=true;
}

double SmartTimer::GetTime(TimeTypes i)const{
    Time_t done=StopTimes_[static_cast<size_t>(i)];
    if(!IsStopped_)
        done=std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds Time=
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                                    done-StartTimes_[static_cast<size_t>(i)]);
    // \todo do the time conversion better
    return static_cast<double>(Time.count()) * 1.0e-9 +
                   Offsets_[static_cast<size_t>(i)];
}

void SmartTimer::Resume(){
    if(IsStopped_){
        for(size_t i=0;i<Offsets_.size();i++)
            Offsets_[i]=GetTime(static_cast<TimeTypes>(i));
        IsStopped_=false;
        StartTimes_[0]=std::chrono::high_resolution_clock::now();
    }
}

std::string SmartTimer::ToString()const{
    std::stringstream ss;
    ss<<Name_<<": "<<GetTime(TimeTypes::WALL)<<" Wall(s)";
    return ss.str();
}
    
    
}//End namespace