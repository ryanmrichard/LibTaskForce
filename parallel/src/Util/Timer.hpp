#ifndef PARALLEL_UTIL_TIMER_HPP_
#define PARALLEL_UTIL_TIMER_HPP_
#include <string>
#include <utility>
#include <chrono>
#include <array>

namespace Util{
    
///The types of clocks, only support wall time at the moment
enum class TimeTypes {WALL=0,CPU=1,SYSTEM=2,NCLOCKS=1};     
    
/** \brief A timer object with some parallel bells and whistles
 *
 *   Times are returned to you either as a pretty
 *   string suited to printing or as TimeValue objects.  If you are going
 *   to use the TimeValue objects take a peek at their documentation.
 *
 *   In general, the wall times generated with this object are accurate to
 *   around a nanosecond.  Given that a double can hold 16 sig figs, this
 *   means that if your wall time exceeds about 7.6 years you will loose a
 *   decimal place of accuracy; you've been warned.
 *
 *   Things to keep in mind:
 *     - Construction starts the timer
 *     - Calling start resets the timer
 *     - By default this timer times all MPI processes individually
       - Percent load imbalance, L (T's are times, assume N processes) is:
 *       \f[
 *        L=\left(\frac{T_{Max}}{\frac{1}{N}
 *                 \sum_{i}^{N}T_i}-1\right)\times\ 100\%
 *       \f]
 *     - Printing necessarily requires some MPI functionality involving the
 *       synching of timers
 *
 */
class SmartTimer{
   private:
      ///The name of the timer
      std::string Name_;
      
      ///Whether or not this time is unique for each MPI process
      bool IsParallel_;
      
      ///Whether or not this timer is stopped
      bool IsStopped_;
      
      ///The type of a time returned by the clock
      typedef std::chrono::time_point<
                std::chrono::high_resolution_clock> Time_t;
      
      
      /** \brief Returns the percent load balance
       *
       *  It's assumed this is being called inside PrintOut and hence
       *  that the Times will be a 3*3*NProcs long vector.  Thinking
       *  of it as a rank 3 tensor, the first index is the process
       *  number (0...NProc-1), the second is the time type being indexed
       *  from 0 to 2 mapping to WALL, CPU, and SYSTEM respectively, the
       *  final index is also from 0 to 2 and is respectively: the time in
       *  seconds, the number of decimal places (cast to a double),
       *  and the resolution in seconds.
       */
      //double ComputeLoadImbalance(TimeTypes Type,
      //                            const std::vector<double>& Times)const;

      ///The starting times
      std::array<Time_t,1> StartTimes_;
      ///The offsets to be added to Times (used for stoping and resuming)
      std::array<double,1> Offsets_;
      
      ///The stopped times
      std::array<Time_t,1> StopTimes_;
   public:
       
      ///Constructs a timer, with parallel statistics if desired
      SmartTimer(const std::string& Name,bool IsParallel=true);
      ///Starts (or restarts) the timer
      void Start();
      ///Stops the timer
      void Stop();
      ///Resumes timing (if currently timing does nothing)
      void Resume();
      ///Returns a desired time type in seconds
      double GetTime(TimeTypes Type)const;
      ///Returns true if the timer is running
      bool IsStopped()const{return IsStopped_;}
      ///Returns the statistics in a pretty string
      std::string ToString() const;
};

inline std::ostream& operator<<(std::ostream& os,const SmartTimer& ST){
    return os<<ST.ToString();
}

}
#endif /* PARALLEL_UTIL_TIMER_HPP_ */
