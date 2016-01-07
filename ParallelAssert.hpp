#ifndef PARALLEL_PARALLELASSERT_HPP_
#define PARALLEL_PARALLELASSERT_HPP_

//#include "../bpmodule/exception/Assert.hpp"
//#include "../bpmodule/exception/Exceptions.hpp"
#include <cassert>
#define PARALLEL_ASSERT(cond,msg)\
   do{if(!cond)std::cout<<msg<<std::endl;assert(cond);}while(0)

#endif /* PARALLEL_PARALLELASSERT_HPP_ */
