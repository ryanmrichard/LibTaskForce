/*
 *@BEGIN LICENSE
 *
 * PSI4: an ab initio quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *@END LICENSE
 */
#include <iostream>
#include "LibParallel.hpp"

double SumValues(double rhs,double lhs){
   return rhs+lhs;
}
namespace Par=bpmodule::LibParallel;
int main(int argc,char** argv){
   size_t NThreads=1;
   Par::Environment DaEnv(NThreads);
   Par::Communicator& Comm=DaEnv.Comm();
   Par::Future<double> Ftr=
         Comm.AddTask(SumValues,3.1,4.2);
   std::cout<<*Ftr<<std::endl;
   return 0;
}
