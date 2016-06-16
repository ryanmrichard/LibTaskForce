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
#ifndef PARALLEL_PARALLELASSERT_HPP_
#define PARALLEL_PARALLELASSERT_HPP_

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
