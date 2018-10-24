#ifndef BTREE_ITERATOR_H
#define BTREE_ITERATOR_H

#include <iterator>
#include <memory>

template <typename T> class btree;
template <typename T> class const_btree_iterator;

template <typename T> class btree_iterator {
public:
    typedef std::ptrdiff_t              difference_type;
    typedef std::forward_iterator_tag   iterator_category;
    typedef T                           value_type;
    typedef T*                          pointer;
    typedef T&                          reference;

    typedef typename btree<T>::Node Node;
    typedef const_btree_iterator<T> const_btree_iter;

    btree_iterator() = default;
    btree_iterator(Node* n, unsigned int p = 0U, bool CanWalk = true);
    btree_iterator(const btree_iterator& b) = default;
    //btree_iterator(btree_iterator&& b) noexcept = default;

    reference operator*() const;
    pointer operator->() const;

    btree_iterator& operator++();
    btree_iterator operator++(int);
    btree_iterator& operator--();
    btree_iterator operator--(int);

    // SELF
    bool operator==(const btree_iterator& other) const;
    bool operator!=(const btree_iterator& other) const;

    bool operator==(const const_btree_iter& other) const {
        return other == *this;
    }
    bool operator!=(const const_btree_iter& other) const {
        return other != *this;
    }


private:
    Node* currPtr {nullptr};
    unsigned int pos {0U};
    bool CanWalk {false};
};

template <typename T> class const_btree_iterator {
public:
    typedef std::ptrdiff_t              difference_type;
    typedef std::forward_iterator_tag   iterator_category;
    typedef T                           value_type;
    typedef T*                          pointer;
    typedef T&                          reference;

    typedef typename btree<T>::Node Node;
    typedef btree_iterator<T> btree_iter;

    const_btree_iterator() = default;
    const_btree_iterator(Node* n, unsigned int p) : btree_it{n, p} {}

    const_btree_iterator(const btree_iter& b) : btree_it{b} {}
    const_btree_iterator(const const_btree_iterator& b) = default;
    //const_btree_iterator(const_btree_iterator&& b) noexcept = default;

    reference operator*() const {
        return *btree_it;
    }
    pointer operator->() const {
        return &(operator*());
    }
    const_btree_iterator& operator++() {
        ++btree_it;
        return *this;
    }
    const_btree_iterator& operator--() {
        --btree_it;
        return *this;
    }

    bool operator==(const const_btree_iterator& other) const {
        return btree_it == other.btree_it;
    }
    bool operator!=(const const_btree_iterator& other) const {
        return !operator==(other);
    }
    bool operator==(const btree_iter& other) const {
        return btree_it == other;
    }
    bool operator!=(const btree_iter& other) const {
        return btree_it != other;
    }

private:
    btree_iter btree_it {};
};

template <typename T>
btree_iterator<T>::btree_iterator(Node* n, unsigned int p, bool CanWalk)
    : currPtr{n}, pos{p}, CanWalk{n == nullptr ? false : CanWalk}
{
}
template <typename T>
typename btree_iterator<T>::reference btree_iterator<T>::operator*() const {
    return currPtr->elems_in_node[pos];
}
template <typename T>
typename btree_iterator<T>::pointer btree_iterator<T>::operator->() const {
    return &(operator*());
}
template <typename T>
btree_iterator<T>& btree_iterator<T>::operator++() {
    if (currPtr == nullptr) {
        return *this;
    }
    if (!CanWalk) {
        CanWalk = true;
        return *this;
    }
    auto prevValue = currPtr->elems_in_node[pos];

    while (currPtr->elems_in_node[pos] <= prevValue && CanWalk) {
        if (pos < currPtr->elems_in_node.size()-1) {
            ++pos;
            while(currPtr->Children_in(pos)) {
                currPtr = currPtr->getLeftChildAt(pos);
                pos = 0;
            }
        } else {
            if (pos == currPtr->elems_in_node.size()-1 && currPtr->hasRightChild()) {
                currPtr = currPtr->getRightChild();
                pos = 0;
            } else {
                if (currPtr->Parent_at()) {
                    // assert(currPtr->parent != nullptr);
                    auto oldPos = pos;
                    auto oldPtr = currPtr;
                    while(prevValue >= currPtr->elems_in_node[pos] &&  pos == currPtr->elems_in_node.size()-1) {
                        if (!currPtr->Parent_at()) {
                            pos = oldPos;
                            currPtr = oldPtr;
                            CanWalk = false;
                            break;
                        }
                        pos = currPtr->Parent_index;
                        currPtr = currPtr->parent;
                    }

                } else {
                    CanWalk = false;
                    break;
                }
            }
        }
    }
    return *this;
}

template <typename T>
btree_iterator<T> btree_iterator<T>::operator++(int) {
    auto tmp = *this;
    ++*this;
    return tmp;
}

template <typename T>
btree_iterator<T>& btree_iterator<T>::operator--() {
    if (currPtr == nullptr) {
        return *this;
    }
    if (!CanWalk) {
        CanWalk = true;
        return *this;
    }
    auto prevValue = currPtr->elems_in_node[pos];

    while (currPtr->elems_in_node[pos] >= prevValue && CanWalk) {
        if (pos > 0) {
            //  1. go down to the max in the subtree at pos
            //  2. go left --pos
            if (currPtr->Children_in(pos)) {
                currPtr = currPtr->getLeftChildAt(pos);
                pos = currPtr->elems_in_node.size()-1;
                while(currPtr->hasRightChild()) {
                    currPtr = currPtr->getRightChild();
                    pos = currPtr->elems_in_node.size()-1;
                }
            } else {
                --pos;
            }
        } else {
            // assert(pos <= 0);
            // 1. go down to the max in the subtree at pos
            // 2. go up parents until i can go left, --pos
            if (currPtr->Children_in(pos)) {
                while(currPtr->Children_in(pos)) {
                    currPtr = currPtr->getLeftChildAt(pos);
                    pos = currPtr->elems_in_node.size()-1;
                    // assert(currPtr->elems_in_node[pos] < prevValue);
                }
            } else {
                if (currPtr->Parent_at()) {
                    // assert(currPtr->parent != nullptr);

                    auto oldPos = pos;
                    auto oldPtr = currPtr;

                    while(currPtr->Parent_at()) {
                        pos = currPtr->Parent_index;
                        currPtr = currPtr->parent;
                        if (pos > 0) {
                            break;
                        }
                    }
                    if (pos == 0) {
                        pos = oldPos;
                        currPtr = oldPtr;
                        CanWalk = false;
                    } else if (currPtr->elems_in_node[pos] > prevValue) {
                        --pos;
                    }
                }
            }
        }
    }
    return *this;
}

template <typename T>
btree_iterator<T> btree_iterator<T>::operator--(int) {
    auto tmp = *this;
    --*this;
    return tmp;
}

template <typename T>
bool btree_iterator<T>::operator==(const btree_iterator& other) const {
    if (CanWalk == false && other.CanWalk == false) {
        return true;
    }
    return currPtr == other.currPtr && pos == other.pos && CanWalk == other.CanWalk;
}

template <typename T>
bool btree_iterator<T>::operator!=(const btree_iterator& other) const {
    return !operator==(other);
}
#endif
