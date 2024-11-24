#include <cassert>
#include <memory>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <future>
#include <ranges>

#pragma once

enum Color { R, B };


template<typename T>
struct Node {
    Node(Color c, 
        std::shared_ptr<const Node> const & lft, 
        T val, 
        std::shared_ptr<const Node> const & rgt)
        : _c(c), _lft(lft), _val(val), _rgt(rgt)
    {}
    T _val;
    Color _c;
    std::shared_ptr<const Node> _lft;
    std::shared_ptr<const Node> _rgt;
};

template<typename T>
using RBTree = std::shared_ptr<const Node<T>>;

template<typename T>
inline bool isEmpty(const RBTree<T>& locRoot) {
    return !locRoot;
}


template<typename T>
Color rootColor(const RBTree<T>& locRoot) {
    assert (!isEmpty(locRoot));
    return locRoot->_c;
}

template<typename T>
inline T root(const RBTree<T>& locRoot) {
    assert(!isEmpty(locRoot));
    return locRoot->_val;
}

template<typename T>
inline RBTree<T> left(const RBTree<T>& locRoot) {
    assert(!isEmpty(locRoot));
    return locRoot->_lft;
}

template<typename T>
inline RBTree<T> right(const RBTree<T>& locRoot) {
    assert(!isEmpty(locRoot));
    return locRoot->_rgt;
}

template<typename T>
inline RBTree<T> paint(const RBTree<T>& locRoot, Color c) {
    assert(!isEmpty(locRoot));
    return std::make_shared<const Node<T>>(c, left(locRoot), root(locRoot), right(locRoot));
}

template<typename T>
auto curryPaint(Color c) {
    return [c](const RBTree<T>& locRoot) {
        assert(!isEmpty(locRoot));
        return std::make_shared<const Node<T>>(c, left(locRoot), root(locRoot), right(locRoot));
    };
};
template<typename T>
auto paintRed = curryPaint<T>(R);

template<typename T>
auto paintBlack = curryPaint<T>(B);

template<typename T>
inline bool doubledLeft(const RBTree<T>& locRoot) {
    return !isEmpty(locRoot)
        && rootColor(locRoot) == R
        && !isEmpty(left(locRoot))
        && rootColor(left(locRoot)) == R;
}

template<typename T>
inline bool doubledRight(const RBTree<T>& locRoot) {
    return !isEmpty(locRoot)
        && rootColor(locRoot) == R
        && !isEmpty(right(locRoot))
        && rootColor(right(locRoot)) == R;
}

template<typename T>
RBTree<T> balance(Color c, const RBTree<T>& lft, const T& x, const RBTree<T>& rgt) {
    if (c == B && doubledLeft(lft))
        return std::make_shared<const Node<T>>(R, 
            paint(left(lft), B), 
            root(lft), 
            std::make_shared<const Node<T>>(B, right(lft), x, rgt));
    else if (c == B && doubledRight(lft))
        return std::make_shared<const Node<T>>(R,
            std::make_shared<const Node<T>>(B, left(lft), root(lft), left(right(lft))),
            root(right(lft)), 
            std::make_shared<const Node<T>>(B, right(right(lft)), x, rgt));
    else if (c == B && doubledLeft(rgt))
        return std::make_shared<const Node<T>>(R,
            std::make_shared<const Node<T>>(B, lft, x, left(left(rgt))), 
            root(left(rgt)), 
            std::make_shared<const Node<T>>(B, right(left(rgt)), root(rgt), right(rgt)));
    else if (c == B && doubledRight(rgt))
        return std::make_shared<const Node<T>>(R, 
            std::make_shared<const Node<T>>(B, lft, x, left(rgt)), 
            root(rgt), 
            paint(right(rgt), B));
    else
        return std::make_shared<const Node<T>>(c, lft, x, rgt);
}
        
template<typename T>
RBTree<T> ins(const RBTree<T>& locRoot, const T& x) {
    if (isEmpty(locRoot))
        return std::make_shared<const Node<T>>(R,
            RBTree<T>(), 
            x, 
            RBTree<T>());
    T y = root(locRoot);
    Color c = rootColor(locRoot);
    if (x < y)
        return balance(c, ins(left(locRoot), x), y, right(locRoot));
    else if (y < x)
        return balance(c, left(locRoot), y, ins(right(locRoot), x));
    else
        return locRoot; // no duplicates
}

template<typename T>
RBTree<T> insert(const RBTree<T>& locRoot, const T& x) {
    RBTree<T> t = ins(locRoot, x);
    return std::make_shared<const Node<T>>(B, left(t), root(t), right(t));
}

template<class T, class F>
void forEach(const RBTree<T>& locRoot, F f) {
    if (!isEmpty(locRoot)) {
        forEach(left(locRoot), f);
        f(root(locRoot));
        forEach(right(locRoot), f);
    }
}

template<class T, class Beg, class End>
RBTree<T> inserted(RBTree<T> t, Beg it, End end) {
    for(auto i = it; i != end; ++i ){
        t = insert(t, *i);
    }
    return t;
}

/* template<class T>
RBTree<T> merge(const RBTree<T>& left, const RBTree<T>& right) {
    // Merging is non-trivial in RB trees, but can be approximated.
    // For simplicity, insert all elements of `right` into `left`.
    std::vector<T> elements;
    elements.reserve(20000);
    forEach(right, [&](const T& x) { elements.emplace_back(x); });
    return inserted(left, elements.begin(), elements.end());
}
 */

// Function to build a balanced tree from sorted elements
template<typename Iter>
RBTree<typename Iter::value_type> buildBalancedTree(Iter begin, Iter end) {
    if (begin == end) return nullptr;
    auto mid = begin + std::distance(begin, end) / 2;
    return std::make_shared<Node<typename Iter::value_type>>(B, 
        buildBalancedTree(begin, mid),
        *mid,
        buildBalancedTree(mid + 1, end));
}
 

template<typename T>
RBTree<T> merge(const RBTree<T>& left, const RBTree<T>& right) {
    // Vectors to hold in-order traversal results
    std::vector<T> leftElements, rightElements;
    forEach(left, [&](const T& x) { leftElements.emplace_back(x); });
    forEach(right, [&](const T& x) { rightElements.emplace_back(x); });

    // Merge step: similar to merging two sorted arrays
    std::vector<T> mergedElements;
    mergedElements.reserve(leftElements.size() + rightElements.size());

    auto it1 = leftElements.begin(), it2 = rightElements.begin();
    while (it1 != leftElements.end() && it2 != rightElements.end()) {
        if (*it1 < *it2) {
            mergedElements.push_back(*it1++);
        } else if (*it2 < *it1) {
            mergedElements.push_back(*it2++);
        } else { 
            // Equal elements: take one and skip duplicates
            mergedElements.push_back(*it1++);
            ++it2;
        }
    }
    // Append remaining elements
    mergedElements.insert(mergedElements.end(), it1, leftElements.end());
    mergedElements.insert(mergedElements.end(), it2, rightElements.end());

    // Build a balanced tree from the merged unique elements
    return buildBalancedTree(mergedElements.begin(), mergedElements.end());
}

template<class T, class Iter>
RBTree<T> parallelInsert(RBTree<T> t, Iter begin, Iter end, size_t depth = 0) {
    constexpr size_t PARALLEL_THRESHOLD = 15000;
    constexpr size_t MAX_DEPTH = 3; // Limit parallel depth to avoid overhead.

    // Compute distance without assuming random access
    auto dist = std::ranges::distance(begin, end);

    if (dist <= PARALLEL_THRESHOLD || depth >= MAX_DEPTH) {
        // Base case: Insert sequentially
        return inserted(t, begin, end);
    }

    // Split the range into two halves
    auto mid = begin + dist / 2;

    // Process each half in parallel
    auto leftFuture = std::async(std::launch::async, [&]() {
        return parallelInsert(t, begin, mid, depth + 1);
    });
    auto rightTree = parallelInsert(t, mid, end, depth + 1);

    // Merge the results
    return merge(leftFuture.get(), rightTree);
}

