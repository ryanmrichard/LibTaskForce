#ifndef PARALLEL_PARALLELASSERT_HPP_
#define PARALLEL_PARALLELASSERT_HPP_

//#include "../bpmodule/exception/Assert.hpp"
//#include "../bpmodule/exception/Exceptions.hpp"
#include <cassert>
#include <iostream>
///If the condition is true, execution continues, otherwise msg is printed and
///execution is aborted.  Note cond may be a command and we don't want to run it
///twice so we store its result and use that
#define PARALLEL_ASSERT(cond,msg)\
   do{bool Result=(cond);\
      if(!Result)std::cerr<<msg<<std::endl;\
      assert(Result);}while(0)

#endif /* PARALLEL_PARALLELASSERT_HPP_ */
