/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DVector.hpp
 * Author: richard
 *
 * Created on February 22, 2016, 12:53 PM
 */

#ifndef DVECTOR_HPP
#define DVECTOR_HPP
#include<map>
#include<memory>
#include "madness/world/MADworld.h"
#include "CommStats.hpp"

namespace LibTaskForce{

/** \brief This class is meant to hold the elements in a distributed fashion,
 *  then when you ask for an element, communication occurs to give it to you
 *  if the element is not local.
 * 
 *   General notes:
 *    - Ownership of elements is set via the CommStats class.
 *    - Accessing an element from every process will be expensive with this
 *      class.  If you want to do that check out the TaskResults class.
 *    - WorldObject has a deleted default constructor so all objects that use
 *      this DVector pick up that peculiarity.  Copy/Assignment should be ok
 */    
template<typename T>
class DVector: private madness::WorldObject<DVector<T>>{
    private:
        typedef madness::WorldObject<DVector<T>> Base_t;
        typedef madness::Future<T> Future_t;
        typedef std::shared_ptr<Future_t> SharedFuture_t;
        
        ///The data, as futures to allow lazy evaluation
        std::map<size_t,SharedFuture_t> MyData_;
        
        ///The details of the parallelism
        std::shared_ptr<const CommStats> MyStats_;      
        
    public:
        DVector(madness::World& world,std::shared_ptr<const CommStats> MyStats):
            Base_t(world),MyStats_(MyStats){Base_t::process_pending();}
        DVector(const DVector<T>&)=default;
        DVector(DVector<T>&&)=default;
        DVector<T>& operator=(const DVector<T>&)=default;
        
        ///Deleted because of base class and the fact that we have no good
        ///default for the base class
        DVector()=delete;
        
        ///All clean-up is managed for us
        ~DVector()=default;
        
        ///Returns the element, communicating if need be
        T Get(size_t i)const{
            T data;
            if(MyStats_->MyTask(i))data=MyData_.at(i)->get();
            else{
                Future_t temp=
                    Base_t::send(MyStats_->WhoRanTask(i),&DVector<T>::Get,i);
                data=temp.get();
            }
            return data;
        }
        
        ///Sets element i to a future, that we take over
        void Set(size_t i,const Future_t& value){
            MyData_[i]=SharedFuture_t(new Future_t(value));
        }
        
        ///Sets element i to the value
        void Set(size_t i,const T& value){Set(i,Future_t(value));}
       
};

    
}//End namespace
#endif /* DVECTOR_HPP */

