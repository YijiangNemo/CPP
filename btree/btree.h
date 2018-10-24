#ifndef BTREE_H
#define BTREE_H

#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>
#include <utility>
#include <queue>

#include <cassert>

#include "btree_iterator.h"
using namespace std;
const size_t DEFAULT_ = 40;

template <typename T> class btree;
template <typename T> std::ostream& operator<<(std::ostream& os, const btree<T>& tree);

template <typename T>
class btree {
public:
    friend class btree_iterator<T>;
    friend class const_btree_iterator<T>;
    typedef btree_iterator<T>                       iterator;
    typedef const_btree_iterator<T>                 const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

    btree() = default;
    btree(size_t t) : Max_Node_size{t} {};

    btree(const btree<T>& other);
    btree(btree<T>&& other) noexcept = default;
    ~btree() = default;

    btree<T>& operator=(const btree<T>& other);
    btree<T>& operator=(btree<T>&& other) noexcept = default;

    friend std::ostream& operator<< <T>(std::ostream& os, const btree<T>& tree);

    iterator end() const;
    iterator begin() const;
    reverse_iterator rbegin() const;
    reverse_iterator rend() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;


    iterator find(const T& elem);
    const_iterator find(const T& elem) const;

    std::pair<iterator, bool> insert(const T& elem);

    void printRoot() const {
        std::cout << "Max_Node_size: " << Max_Node_size << "\n";
        if (root != nullptr) {
            root->printNode();
        }
    }

private:
    iterator maxElement() const;
    void clear() noexcept;
    void swap(btree<T>& other) noexcept;
    iterator findElem(const T& elem) const;

    struct Node {
        Node();
        Node(unsigned int posP, Node* p, const size_t m);
        Node(unsigned int posP, Node* p, const T& val, const size_t m);
        ~Node() = default;

        void printNode(int level = 0) const;

        void reset_();

        void createLeftChildAt(unsigned int i);
        void createRightChild();

        Node* getLeftChildAt(unsigned int i) const;
        Node* getRightChild() const;
        Node* getLeftChild() const;

        bool Children_in(unsigned int i) const;
        bool Parent_at() const;
        bool hasRightChild() const;
        bool Contains_e(const T& v) const;

        // Put X in front of the element at index i. If full, do nothing
        void emplaceElementBefore(unsigned int index, const T& elem);
        bool isFull() const;
        bool check() const;


        unsigned int Parent_index {0U};
        Node* parent {nullptr};
        std::vector<T> elems_in_node {};
        std::vector<std::unique_ptr<Node>> Childrens_in_node {};
        std::unique_ptr<Node> Child_at_last {nullptr};
        size_t Max_Node_size {DEFAULT_};
    }; // NODE

    size_t Max_Node_size {DEFAULT_};   // shared ptr so updates are reflected globally, maybe
    std::unique_ptr<Node> root {nullptr};
};

template <typename T>
btree<T>::btree(const btree& other)
    : Max_Node_size{other.Max_Node_size}
{
    auto q = std::queue<Node*>{};
    q.push(other.root.get());

    while(! q.empty()) {
        auto curr = q.front();
        q.pop();
        for(auto i = 0U; i < curr->elems_in_node.size(); ++i) {
            insert(curr->elems_in_node[i]);
            if (curr->Children_in(i)) {
                q.push(curr->getLeftChildAt(i));
            }
        }
        if (curr->hasRightChild()) {
            q.push(curr->getRightChild());
        }
    }
}

template <typename T>
btree<T>& btree<T>::operator=(const btree& other) {
    btree<T>{other}.swap(*this);
    return *this;
}

template <typename T>
std::ostream& operator<< (std::ostream& o, const btree<T>& b)  {
    // TODO: change to std::copy to ostream_iterator
    if (b.root != nullptr) {
        auto q = std::queue<typename btree<T>::Node*>{};
        q.push(b.root.get());
        while(! q.empty()) {
            auto curr = q.front();
            q.pop();
            for(auto i = 0U; i < curr->elems_in_node.size(); ++i) {
                if (i > 0 || curr != b.root.get()) {
                    o << ' ';
                }
                o << curr->elems_in_node[i];

                if (curr->Children_in(i)) {
                    q.push(curr->getLeftChildAt(i));
                }
            }
            if (curr->hasRightChild()) {
                q.push(curr->getRightChild());
            }
        }
    }
    return o;
}


// ITERATORS
template <typename T>
typename btree<T>::iterator btree<T>::end() const {
    return iterator(maxElement());
}
template <typename T>
typename btree<T>::iterator btree<T>::begin() const {
    if (root == nullptr || root->check()) {
        return iterator(end());
    }
    auto curr = root.get();
    while (curr->Children_in(0)) {
        curr = curr->getLeftChild();
    }
    return iterator(curr);
}
template <typename T>
typename btree<T>::reverse_iterator btree<T>::rbegin() const {
    return reverse_iterator(end());
}
template <typename T>
typename btree<T>::reverse_iterator btree<T>::rend() const {
    return reverse_iterator(begin());
}
template <typename T>
typename btree<T>::const_iterator btree<T>::cbegin() const {
    return const_iterator(begin());
}
template <typename T>
typename btree<T>::const_iterator btree<T>::cend() const {
    return const_iterator(end());
}
template <typename T>
typename btree<T>::const_reverse_iterator btree<T>::crbegin() const {
    return const_reverse_iterator(rbegin());
}
template <typename T>
typename btree<T>::const_reverse_iterator btree<T>::crend() const {
    return const_reverse_iterator(rend());
}


// METHODS
template <typename T>
typename btree<T>::iterator btree<T>::find(const T& elem) {
    return findElem(elem);
}
template <typename T>
typename btree<T>::const_iterator btree<T>::find(const T& elem) const {
    return findElem(elem);
}

template <typename T>
std::pair<typename btree<T>::iterator, bool> btree<T>::insert(const T& elem) {
    if (root == nullptr || root->check()) {
        root = std::make_unique<Node>(0, nullptr, elem, Max_Node_size);
        return std::make_pair(iterator(root.get(), 0), true);
    }

    auto curr = root.get();
    auto i = 0U;
    // get leaf node where it should be
    while (curr->isFull()) {
        // node should always be full here
        // assert(curr->elems_in_node.size() == Max_Node_size);

        if (elem < curr->elems_in_node[i]) {
            if (curr->getLeftChildAt(i) == nullptr) {
                curr->createLeftChildAt(i);
            }
            curr = curr->getLeftChildAt(i);
            i = 0;

        } else if (elem > curr->elems_in_node[i]) {
            if (i < Max_Node_size-1) {
                ++i;
            } else {
                if (curr->getRightChild() == nullptr) {
                    curr->createRightChild();
                }
                curr = curr->getRightChild();
                i = 0;
            }
        } else {
            return std::make_pair(iterator(curr, i), false);    // Found match
        }
    }
    // should always be a non-full node here
    // assert(curr->elems_in_node.size() < Max_Node_size);
    auto elems_in_node = curr->elems_in_node;
    while (i < elems_in_node.size()) {
        if (elem == elems_in_node[i]) {
            return std::make_pair(iterator(curr, i), false);    // Found match
        } else if (elem < elems_in_node[i]) {
            break;
        } else {
            ++i;
        }
    }

    curr->emplaceElementBefore(i, elem);
    // assert(curr->elems_in_node[i] == elem);
    return std::make_pair(iterator(curr, i), true);
}


// PRIVATES
template <typename T>
typename btree<T>::iterator btree<T>::maxElement() const {
    auto curr = root.get();
    while (curr->hasRightChild()) {
        curr = curr->getRightChild();
    }
    return iterator(curr, curr->elems_in_node.size()-1, false);
}
template <typename T>
void btree<T>::clear() noexcept {
    Max_Node_size = DEFAULT_;
    root.reset(nullptr);
}
template <typename T>
void btree<T>::swap(btree& other) noexcept {
    std::swap(Max_Node_size, other.Max_Node_size);
    std::swap(root, other.root);
}
template <typename T>
typename btree<T>::iterator btree<T>::findElem(const T& elem) const {
    if (root == nullptr || root->check()) {
        return end();
    }

    auto curr = root.get();
    auto i = 0U;
    // get leaf node where it should be
    while (curr->isFull()) {
        if (elem < curr->elems_in_node[i]) {
            if (curr->Children_in(i)) {
                curr = curr->getLeftChildAt(i);
                i = 0;
            } else {
                return end();
            }
        } else if (elem > curr->elems_in_node[i]) {
            if (i < Max_Node_size-1) {
                ++i;
            } else {
                if (curr->getRightChild() == nullptr) {
                    return end();
                }
                curr = curr->getRightChild();
                i = 0;
            }
        } else {
            return iterator(curr, i);    // Found match
        }
    }

    // should always be a non-full node here
    // assert(curr->elems_in_node.size() < Max_Node_size);
    auto elems_in_node = curr->elems_in_node;
    while (i < elems_in_node.size()) {
        if (elem == elems_in_node[i]) {
            return iterator(curr, i);    // Found match
        } else if (elem < elems_in_node[i]) {
            break;
        } else {
            ++i;
        }
    }
    return end();
}


// NODE
template <typename T>
btree<T>::Node::Node() {
    reset_();
}
template <typename T>
btree<T>::Node::Node(unsigned int posP, Node* p, const size_t m)
    : Parent_index{posP}, parent{p}, Max_Node_size{m}
{
    reset_();
}
template <typename T>
btree<T>::Node::Node(unsigned int posP, Node* p, const T& val, const size_t m)
    : Parent_index{posP}, parent{p}, elems_in_node{val}, Max_Node_size{m}
{
    reset_();
}
template <typename T>
void btree<T>::Node::printNode(int level) const {
    std::cout << "NODE " << level << ": ";
    for (auto i = 0U; i < elems_in_node.size(); ++i) {
        std::cout << elems_in_node[i] << " (" << Parent_index << ") ";
    }
    std::cout << "\n";
    for(auto i = 0U; i < Childrens_in_node.size(); ++i) {
        if (Childrens_in_node[i] != nullptr) {
            Childrens_in_node[i]->printNode(level+1);
        }
    }
    if (Child_at_last != nullptr) {
        Child_at_last->printNode(level+1);
    }
}
template <typename T>
void btree<T>::Node::reset_() {
    Childrens_in_node.clear();
    for (auto i = 0U; i < Max_Node_size; ++i) {
        Childrens_in_node.push_back(nullptr);
    }
    Child_at_last = nullptr;
}
template <typename T>
void btree<T>::Node::createLeftChildAt(unsigned int i) {
    Childrens_in_node[i] = std::make_unique<Node>(i, this, Max_Node_size);
}
template <typename T>
void btree<T>::Node::createRightChild() {
    Child_at_last = std::make_unique<Node>(elems_in_node.size()-1, this, Max_Node_size);
}

template <typename T>
typename btree<T>::Node* btree<T>::Node::getLeftChildAt(unsigned int i) const {
    return Childrens_in_node[i].get();
}
template <typename T>
typename btree<T>::Node* btree<T>::Node::getRightChild() const {
    return Child_at_last.get();
}
template <typename T>
typename btree<T>::Node* btree<T>::Node::getLeftChild() const {
    return getLeftChildAt(0);
}
template <typename T>
bool btree<T>::Node::Children_in(unsigned int i) const {
    return getLeftChildAt(i) != nullptr;
}
template <typename T>
bool btree<T>::Node::Parent_at() const {
    return parent != nullptr;
}
template <typename T>
bool btree<T>::Node::hasRightChild() const {
    return Child_at_last != nullptr;
}
template <typename T>
bool btree<T>::Node::Contains_e(const T& v) const {
    return std::find(elems_in_node.cbegin(), elems_in_node.cend(), v);
}

// Put X in front of the element at index i. If full, do nothing
template <typename T>
void btree<T>::Node::emplaceElementBefore(unsigned int index, const T& elem) {
    if (! isFull()) {
        elems_in_node.emplace(elems_in_node.cbegin() + index, elem);
    }
}
template <typename T>
bool btree<T>::Node::isFull() const {
    return elems_in_node.size() == Max_Node_size;
}
template <typename T>
bool btree<T>::Node::check() const {
    return elems_in_node.empty();
}
#endif
