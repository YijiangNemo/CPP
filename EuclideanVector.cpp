//
//  ass2
//
//  Created by 吴一江 on 2017/8/23.
//  Copyright © 2017年 吴一江. All rights reserved.
//

#include <iostream>
#include "EuclideanVector.h"
#include <vector>
#include <cassert>
namespace evec {
    evec::EuclideanVector::EuclideanVector() :  evec::EuclideanVector::EuclideanVector(1) {}

    
    evec::EuclideanVector::EuclideanVector(const unsigned int dimension) :d{dimension} {
        mags = new double [d];
        for (unsigned int i = 0; i < d; i++) {
            *(mags+i) = 0;
        }
    }
    
    evec::EuclideanVector::EuclideanVector(const unsigned int dimension, const double magnitude) :
       d{dimension}{
           mags = new double [d];
           for (unsigned int i = 0; i < d; i++) {
               *(mags+i) = magnitude;
           }

       }
    
       evec::EuclideanVector::EuclideanVector(const std::list<double> l) :
    d{(unsigned int)l.size()}
    {
        mags = new double [d];
        unsigned int i = 0;
        for (std::list<double>::const_iterator iter = l.begin(); iter!= l.end(); ++iter) {
            *(mags+i) = (*iter);
            i++;
        }
    }
    
    
    evec::EuclideanVector::EuclideanVector(const std::vector<double> l) :
    d{(unsigned int)l.size()}
    {
        mags = new double [d];
        unsigned int i = 0;
        for (std::vector<double>::const_iterator iter = l.begin(); iter!= l.end(); ++iter) {
            *(mags+i) = (*iter);
            i++;
        }
    }
    evec::EuclideanVector::EuclideanVector(const unsigned int dimension, double magnitude_[]):d{dimension}{
        mags = new double [d];
        for(unsigned int i = 0;i<d;i++){
            
            *(mags+i) = magnitude_[i];}
    }

    
       evec::EuclideanVector::~EuclideanVector() {
        if (mags != nullptr) delete [] mags;
        mags = nullptr;
        
    }
    evec::EuclideanVector::EuclideanVector(const std::initializer_list<double> l) :
    d{(unsigned int)l.size()}
    {
        mags = new double [d];
        unsigned int i = 0;
        for(std::initializer_list<double>::const_iterator it = l.begin(); it != l.end(); ++it) {
            *(mags+i) = (*it);
            i++;
        }
    }

    //copy consturctor
    evec::EuclideanVector::EuclideanVector(const EuclideanVector& euclideanVector) :
    d{euclideanVector.d}
    {
        if (mags != nullptr) {
            delete [] mags;
        }
        mags = new double [d];
        double *new_mags = euclideanVector.mags;
        for (unsigned int i = 0; i < d; i++) {
            *(mags+i) = *(new_mags+i);
        }
        
    }
    //copy assignment operator
    evec::EuclideanVector& evec::EuclideanVector::operator=(const evec::EuclideanVector &euclideanVector)
    {
        if (this != &euclideanVector) {
            if (mags != nullptr) {
                delete [] mags;
            }
            d = euclideanVector.d;
            mags = new double [d];
            double *new_mags = euclideanVector.mags;
            for (unsigned int i = 0; i< d;i++) {
                *(mags+i) = *(new_mags+i);
            }
        }
        
        return *this;
    }
    
    //move constructor
    evec::EuclideanVector::EuclideanVector(EuclideanVector &&euclideanVector) :
    d{std::move(euclideanVector.d)}
    {
        assert(this != &euclideanVector);
        if (mags != nullptr) {
            delete [] mags;
        }
        euclideanVector.d = 0;
        mags = std::move(euclideanVector.mags);
        euclideanVector.mags = nullptr;
        
    }
    
    
    
    
    //move assignment operator
    evec::EuclideanVector& evec::EuclideanVector::operator=(evec::EuclideanVector &&euclideanVector)
    {
        if (this != &euclideanVector) {
            d = std::move(euclideanVector.d);
            euclideanVector.d = 0;
            if (mags != nullptr) {;
                delete [] mags;
            }
            mags = std::move(euclideanVector.mags);
            euclideanVector.mags= nullptr;
        }
        return *this;
    }

    
    uint evec::EuclideanVector::getNumDimensions() const { return d; }
   
    double evec::EuclideanVector::get(uint i) const {
        assert(i >= 0 && i < d);
        double *value = mags;
        return *(value+i);
        
    }


    double evec::EuclideanVector::getEuclideanNorm() const{
        double sum = 0;
        double *value = mags;
        for(int unsigned i = 0;i<d;i++){
            sum = sum + double(*(value+i))*double(*(value+i));}
        return sqrt(sum);}
    
    evec::EuclideanVector evec::EuclideanVector::createUnitVector() const
    {
        double EucNorm = this->getEuclideanNorm();
        
        double*  new_mag = new double[d];
        for (unsigned int i = 0; i < d; i++) {
            new_mag[i]= (*(mags+i)/EucNorm);
            
        }
        EuclideanVector new_euc = *new EuclideanVector(d , new_mag);
        return new_euc;
        
    }

    
    double evec::EuclideanVector::operator[](int i) const {
        assert(uint(i) >=0 && uint(i) <  d);
        double *value = mags;
        return *(value+i);
    }
    
    double& evec::EuclideanVector::operator[](int i) {
        assert(uint(i) >=0 && uint(i) <  d);
        double *value = mags;
        return *(value+i);
    }


    EuclideanVector& EuclideanVector::operator+=(const EuclideanVector& rhs) {
        assert(this->getNumDimensions() == rhs.getNumDimensions());
        for (unsigned int i = 0; i < d; i++) {
            *(mags+i) += *(rhs.mags+i);}
        return *this;
    }
    
    EuclideanVector& EuclideanVector::operator-=(const EuclideanVector& rhs) {
        assert(this->getNumDimensions() == rhs.getNumDimensions());
        for (unsigned int i = 0; i < d; i++) {
            *(mags+i) -= *(rhs.mags+i);}
        return *this;
    }
    
    EuclideanVector& EuclideanVector::operator*=(const double x) {
        
        for (unsigned int i = 0; i < d; i++) {
            *(mags+i) *= x;}
        return *this;

    }
    
    EuclideanVector& EuclideanVector::operator/=(const double x) {
        for (unsigned int i = 0; i < d; i++) {
            *(mags+i) /= x;}
        return *this;
    }
    //type conversion
    evec::EuclideanVector::operator std::vector<double>() const
    {
        std::vector<double> v_;
        double *value = mags;
        for (unsigned int i = 0; i < d; i++) {
            v_.push_back(*(value+i));
        }
        return v_;
    }
    
    evec::EuclideanVector::operator std::list<double>() const
    {
        std::list<double> l;
        double *value = mags;
        for (unsigned int i = 0; i < d; i++) {
            l.push_back(*(value+i));
        }
        return l;
    }
    
    //nonmenber friend function
    std::ostream& operator<<(std::ostream& os, const evec::EuclideanVector& e) {
        os << "[";
        if (e.d != 0) {
            
            for (unsigned int i = 0; i < e.d; i++) {
                os<<*(e.mags+i);
                if (i != e.d - 1) os<<" ";
            }
        }

        os << "]";
        return os;
    }
    
    bool operator==(const EuclideanVector& lhs, const EuclideanVector& rhs) {
        if (lhs.getNumDimensions() != rhs.getNumDimensions()) return false;
        if (lhs.getNumDimensions() == rhs.getNumDimensions() && lhs.getNumDimensions() == 0) return true;
        for (unsigned int i = 0; i < lhs.getNumDimensions(); i++) {
            
            if (*(lhs.mags+i) != *(rhs.mags+i)) return false;
        }
        return true;
    }
    bool operator!=(const evec::EuclideanVector &lhs, const evec::EuclideanVector &rhs)
    {
        return !(lhs==rhs);
    }
    EuclideanVector& operator+(const evec::EuclideanVector &lhs, const evec::EuclideanVector &rhs)
    {
        
        assert(lhs.getNumDimensions() == rhs.getNumDimensions());
        double *new_mag = new double [rhs.d];
        for (unsigned int i = 0; i < rhs.d; i++) {
            new_mag[i]= *(lhs.mags+i) + *(rhs.mags+i);
            
        }
        
        EuclideanVector *new_euc = new EuclideanVector(rhs.d, new_mag);
        return *new_euc;
    }
    EuclideanVector& operator-(const evec::EuclideanVector &lhs, const evec::EuclideanVector &rhs)
    {
        
        assert(lhs.getNumDimensions() == rhs.getNumDimensions());
        double *new_mag = new double [rhs.d];
        for (unsigned int i = 0; i < rhs.d; i++) {
            new_mag[i]= *(lhs.mags+i) - *(rhs.mags+i);
            
        }
        
        EuclideanVector *new_euc = new EuclideanVector(rhs.d, new_mag);
        return *new_euc;
    }

    double operator*(const EuclideanVector& lhs, const EuclideanVector& rhs) {
        assert(lhs.getNumDimensions() == rhs.getNumDimensions() );
        double sum = 0;
        for (unsigned int i = 0;i<lhs.d;i++){
            sum = sum + double(*(lhs.mags+i) * *(rhs.mags+i));}
        return sum;
    }
    EuclideanVector& operator*(const evec::EuclideanVector &euclideanVector, const double x)
    {
        double *new_mag = new double [euclideanVector.d];
        for (unsigned int i = 0; i < euclideanVector.d; i++) {
            new_mag[i] = *(euclideanVector.mags+i)*x;
            
        }
        EuclideanVector *new_euc = new EuclideanVector(euclideanVector.d, new_mag);
        return *new_euc;
       
    }
    EuclideanVector& operator*(const double x, const evec::EuclideanVector &euclideanVector)
    {
        return euclideanVector*x;
    }

    EuclideanVector& operator/(const EuclideanVector &euclideanVector, const double div) {
        
        double *new_mag = new double [euclideanVector.d];
        for (unsigned int i = 0; i < euclideanVector.d; i++) {
            new_mag[i] = *(euclideanVector.mags+i)/div;
            
        }
        EuclideanVector *new_euc = new EuclideanVector(euclideanVector.d, new_mag);
        return *new_euc;
    }
    
}
