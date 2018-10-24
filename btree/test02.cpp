#include <algorithm>
#include <iostream>
#include <iterator>

#include "btree.h"

void foo(const btree<int> &b) {
  std::copy(b.begin(), b.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
}

int main(void) {
  btree<int> b;
    
    b.insert(1);
    b.insert(10);
    b.insert(3);
    b.insert(4);
    b.insert(2);
    b.insert(11);
    b.insert(5);
    b.insert(6);
    b.insert(7);
    b.insert(0);
    
    for(btree<int>::iterator iter = b.find(11); iter != b.begin(); --iter){
        std::cout << *iter << std::endl;

    }
  foo(b);
  
  return 0;
}
