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
auto paint(Color c) {
    return [c](const RBTree<T>& locRoot) {
        assert(!isEmpty(locRoot));
        return std::make_shared<const Node<T>>(c, left(locRoot), root(locRoot), right(locRoot));
    };
}

template<typename T>
auto paintRed = paint<T>(R);

template<typename T>
auto paintBlack = paint<T>(B);

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
auto balance(Color c) {
    return [c](const RBTree<T>& lft) {
        return [c, &lft](const T& x) {
            return [c, &lft, &x](const RBTree<T>& rgt) {
                if (c == B && doubledLeft(lft))
                    return std::make_shared<const Node<T>>(R, 
                        paintBlack<T>(left(lft)), 
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
                        paintBlack<T>(right(rgt)));
                else
                    return std::make_shared<const Node<T>>(c, lft, x, rgt);
            };
        };
    };
}

template<typename T>
std::function<RBTree<T>(const T&)> ins(const RBTree<T>& locRoot) {
    return [&locRoot](const T& x) -> RBTree<T> {
        if (isEmpty(locRoot))
            return std::make_shared<const Node<T>>(R, RBTree<T>(), x, RBTree<T>());
        
        T y = root(locRoot);
        Color c = rootColor(locRoot);

        if (x < y)
            return balance<T>(c) (ins(left(locRoot))(x)) (y) (right(locRoot));
        else if (y < x)
            return balance<T>(c)(left(locRoot)) (y) (ins(right(locRoot))(x));
        else
            return locRoot; // no duplicates
    };
}


template<typename T>
auto insert(const RBTree<T>& locRoot) {
    return [&locRoot](const T& x) -> RBTree<T> {
        RBTree<T> t = ins<T>(locRoot)(x);
        return std::make_shared<const Node<T>>(B, left(t), root(t), right(t));
    };
}

template<class T, class F>
void forEach(const RBTree<T>& t, F f) {
    if (!isEmpty(t)) {
        forEach(left(t), f);
        f(root(t));
        forEach(right(t), f);
    }
}

/* template<class T, class Beg, class End>
RBTree<T> inserted(RBTree<T> t, Beg it, End end) {
    if (it == end) {
        return t;
    }
    T item = *it;
    auto t1 = inserted(t, ++it, end);
    return insert(t1)(item);
} */

template<class T>
auto inserted(RBTree<T> t) {
    return [&t](auto it, auto end) {
        for(auto i = it; i != end; ++i) {
            t = insert(t)(*i);
        }
        return t;
    };
}

template<class T>
auto merge(RBTree<T> left) {
    return [&left](const RBTree<T>& right) {
        std::vector<T> elements;
        elements.reserve(20000);
        forEach(right, [&](const T& x) { elements.emplace_back(x); });
        return inserted(left)(elements.begin(), elements.end());
    };
}

template<class T>
auto parallelInsert(RBTree<T> t) {
    constexpr size_t PARALLEL_THRESHOLD = 10000;

    return [&t](auto begin, auto end) {
        // Compute distance
        auto dist = std::ranges::distance(begin, end);

        if (dist <= PARALLEL_THRESHOLD) {
            // Insert rest
            return inserted(t)(begin, end);
        }

        // Split the range into two halves
        auto mid = begin;
        std::advance(mid, dist / 2);

        // Process each half in parallel
        auto leftFuture = std::async(std::launch::async, [&]() {
            return parallelInsert(t)(begin, mid);
        });
        auto rightTree = parallelInsert(t)(mid, end);

        // Merge the results
        return merge(leftFuture.get())(rightTree);
    };
}
