//
//  EuclideanVector.h
//  ass2
//
//  Created by 吴一江 on 2017/8/23.
//  Copyright © 2017年 吴一江. All rights reserved.
//

#ifndef EUCLIDEANVECTOR_H_
#define EUCLIDEANVECTOR_H_

#include <sys/types.h>
#include <math.h>
#include <vector>
#include <list>
#include <iterator>

namespace evec{
class EuclideanVector {
    private:
    unsigned int d;
    double *mags = nullptr;
    
    public:
    
    EuclideanVector() ;
    
    EuclideanVector(const unsigned int dimension);
    
    EuclideanVector(const unsigned int dimension, const double magnitude) ;
    
    EuclideanVector(const unsigned int dimension, double  magnitude_[]);
    
    // use template to read iterator
    template <typename Iter>
    EuclideanVector(const Iter& begin,
                    const Iter& end,
                    typename Iter::iterator_category* p=0) : d{(unsigned int)std::distance(begin, end)}{
        
        mags = new double [d];
        unsigned int i = 0;
        for(auto iter = begin; iter !=end;++iter,i++){
            *(mags+i) = (*iter);
        }
    }
    EuclideanVector(const std::list<double> l);
    
      
    EuclideanVector(const std::vector<double> l);
    
    //list initialize
    EuclideanVector(const std::initializer_list<double> l);
    
     //deconstructor
    ~EuclideanVector();
    
    //copy constructor
    EuclideanVector(const EuclideanVector& euclideanVector);
    //copy assignment
    EuclideanVector& operator=(const EuclideanVector &euclideanVector);
    
    //move consturctor
    EuclideanVector(EuclideanVector&& euclideanVector);
    
    
    //move assignment
    EuclideanVector& operator=(EuclideanVector &&euclideanVector);
    //
    uint getNumDimensions() const ;
    double get(uint i) const ;
    
    double getEuclideanNorm() const;
    
    EuclideanVector createUnitVector() const;

    double operator[](int i) const ;  // getter
    double& operator[](int i) ;       // setter
    //operator
    EuclideanVector& operator+=(const EuclideanVector& rhs);
    EuclideanVector& operator-=(const EuclideanVector& rhs);
    EuclideanVector& operator*=(const double x);
    EuclideanVector& operator/=(const double x);
    //casting
    operator std::vector<double>() const;
    operator std::list<double>() const;
    //nonmenber function
    friend std::ostream &operator<<(std::ostream &os, const EuclideanVector &euclideanVector);
    friend bool operator==(const EuclideanVector &lhs, const EuclideanVector &rhs);
    friend bool operator!=(const EuclideanVector &lhs, const EuclideanVector &rhs);
    friend EuclideanVector& operator+(const EuclideanVector &lhs, const EuclideanVector &rhs);
    friend EuclideanVector& operator-(const EuclideanVector &lhs, const EuclideanVector &rhs);
    friend double operator*(const EuclideanVector &lhs, const EuclideanVector &rhs);
    friend EuclideanVector& operator*(const EuclideanVector &euclideanVector, const double times);
    friend EuclideanVector& operator*(const double times, const EuclideanVector &euclideanVector);
    friend EuclideanVector& operator/(const EuclideanVector &euclideanVector, const double divisor);
};
    std::ostream &operator<<(std::ostream &os, const EuclideanVector &euclideanVector);
    bool operator==(const EuclideanVector &lhs, const EuclideanVector &rhs);
    bool operator!=(const EuclideanVector &lhs, const EuclideanVector &rhs);
    EuclideanVector& operator+(const EuclideanVector &lhs, const EuclideanVector &rhs);
    EuclideanVector& operator-(const EuclideanVector &lhs, const EuclideanVector &rhs);
    double operator*(const EuclideanVector &lhs, const EuclideanVector &rhs);
    EuclideanVector& operator*(const EuclideanVector &euclideanVector, const double times);
    EuclideanVector& operator*(const double times, const EuclideanVector &euclideanVector);
    EuclideanVector& operator/(const EuclideanVector &euclideanVector, const double divisor);

}
#endif /* EuclideanVector_h */
