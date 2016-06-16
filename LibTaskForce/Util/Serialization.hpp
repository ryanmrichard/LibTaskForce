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

/* 
 * File:   Serialization.hpp
 * Author: richard
 *
 * Created on March 9, 2016, 3:27 PM
 */

#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include <sstream>
#include <vector>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>


namespace LibTaskForce{

typedef std::vector<char> binary_type;
    
///Given a vector of binary data "casts" it to type T;
template<typename T>
T deserialize(binary_type& Buffer){
    std::stringstream ss;
    ss.rdbuf()->pubsetbuf(Buffer.data(),Buffer.size());
    T DeSerial;
    {
        cereal::PortableBinaryInputArchive DeSerializer(ss);
        DeSerializer(DeSerial);
    }
    return DeSerial;
}

///Given a serializable object of type T, serializes it into our binary type
template<typename T>
binary_type serialize(T Buffer){
    std::stringstream ss;
    {
        cereal::PortableBinaryOutputArchive ar(ss);
        ar(Buffer);
    }
    const std::string s=ss.str();
    return binary_type(s.begin(),s.end());
    /*const size_t slen=s.length();
    Binary_t temp;
    for(size_t i=0;i<slen;++i)temp.push_back(s[i]);
    return temp;*/
}   

}//End namespace


#endif /* SERIALIZATION_HPP */

