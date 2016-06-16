/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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

