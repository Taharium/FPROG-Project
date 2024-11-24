#include <vector>
#include <string>
#include <thread>
#include <future>
#include <algorithm>
#include <iostream>

#include <cassert>
#include <memory>
#include <iostream>
#include <numeric>
#include <algorithm>

#pragma once

template<typename T>
class RBTree {

    public:
        enum Color { R, B };
        struct Node {
            Node(Color c, 
                std::shared_ptr<const Node> const & lft, 
                T val, 
                std::shared_ptr<const Node> const & rgt)
                : _c(c), _lft(lft), _val(val), _rgt(rgt)
            {}
            Color _c;
            std::shared_ptr<const Node> _lft;
            T _val;
            std::shared_ptr<const Node> _rgt;
        };
        RBTree() {}
        RBTree(Color c, RBTree const & lft, T val, RBTree const & rgt)
            : _root(std::make_shared<const Node>(c, lft._root, val, rgt._root)) {
            assert(lft.isEmpty() || lft.root() < val);
            assert(rgt.isEmpty() || val < rgt.root());
        }

        RBTree(std::initializer_list<T> init) {
            RBTree t;
            for (T v : init) {
                t = t.insert(v);
            }
            _root = t._root;
        }

        RBTree insert(T x) const {
            RBTree t = ins(x);
            return RBTree(B, t.left(), t.root(), t.right());
        }

        bool isEmpty() const { return !_root; }
        T root() const {
            assert(!isEmpty());
            return _root->_val;
        }
        RBTree left() const {
            assert(!isEmpty());
            return RBTree(_root->_lft);
        }
        RBTree right() const {
            assert(!isEmpty());
            return RBTree(_root->_rgt);
        }

        static RBTree balance(Color c
                    , RBTree const & lft
                    , T x
                    , RBTree const & rgt) {
            if (c == B && lft.doubledLeft())
                return RBTree(R
                            , lft.left().paint(B)
                            , lft.root()
                            , RBTree(B, lft.right(), x, rgt));
            else if (c == B && lft.doubledRight())
                return RBTree(R
                            , RBTree(B, lft.left(), lft.root(), lft.right().left())
                            , lft.right().root()
                            , RBTree(B, lft.right().right(), x, rgt));
            else if (c == B && rgt.doubledLeft())
                return RBTree(R
                            , RBTree(B, lft, x, rgt.left().left())
                            , rgt.left().root()
                            , RBTree(B, rgt.left().right(), rgt.root(), rgt.right()));
            else if (c == B && rgt.doubledRight())
                return RBTree(R
                            , RBTree(B, lft, x, rgt.left())
                            , rgt.root()
                            , rgt.right().paint(B));
            else
                return RBTree(c, lft, x, rgt);
        }

    private:

        explicit RBTree(std::shared_ptr<const Node> const & node) : _root(node) {} 

        RBTree ins(T x) const {
            if (isEmpty())
                return RBTree(R, RBTree(), x, RBTree());
            T y = root();
            Color c = rootColor();
            if (x < y)
                return balance(c, left().ins(x), y, right());
            else if (y < x)
                return balance(c, left(), y, right().ins(x));
            else
                return *this; // no duplicates
        }

        bool doubledLeft() const {
            return !isEmpty()
                && rootColor() == R
                && !left().isEmpty()
                && left().rootColor() == R;
        }
        bool doubledRight() const {
            return !isEmpty()
                && rootColor() == R
                && !right().isEmpty()
                && right().rootColor() == R;
        }
        RBTree paint(Color c) const {
            assert(!isEmpty());
            return RBTree(c, left(), root(), right());
        }

        Color rootColor() const {
            assert (!isEmpty());
            return _root->_c;
        }

        std::shared_ptr<const Node> _root;

};

template<class T, class F>
void forEach(RBTree<T> const & t, F f) {
    if (!t.isEmpty()) {
        forEach(t.left(), f);
        f(t.root());
        forEach(t.right(), f);
    }
}

template<class T, class Beg, class End>
RBTree<T> inserted(RBTree<T> t, Beg it, End end) {
    for(auto i = it; i != end; ++i ){
        t = t.insert(*i);
    }
    return t;
}


// Function to merge two trees
template<typename T>
RBTree<T> mergeTrees(const RBTree<T>& t1, const RBTree<T>& t2) {
    RBTree<T> result = t1;
    forEach(t2, [&](T val) {
        result = result.insert(val);
    });
    return result;
}

// Helper function to build a tree from a sorted range
template<typename T, typename Iter>
RBTree<T> buildTreeFromSortedRange(Iter begin, Iter end) {
    RBTree<T> tree;
    for (Iter it = begin; it != end; ++it) {
        tree = tree.insert(*it);
    }
    return tree;
}

// Multithreaded insertion function
template<typename T, class Begin, class End>
RBTree<T> insertedParallel(Begin begin, End end, size_t numThreads = std::thread::hardware_concurrency()) {
    size_t totalElements = std::distance(begin, end);
    if (totalElements == 0) {
        return RBTree<T>();
    }

    // Determine chunk size
    size_t chunkSize = (totalElements + numThreads - 1) / numThreads; // Round up
    std::vector<std::future<RBTree<T>>> futures;

    // Launch threads to build partial trees
    for (size_t i = 0; i < numThreads; ++i) {
        auto chunkBegin = begin + i * chunkSize;
        auto chunkEnd = (i == numThreads - 1) ? end : begin + (i + 1) * chunkSize;

        // Sort the chunk and build the tree
        futures.push_back(std::async(std::launch::async, [chunkBegin, chunkEnd]() {
            std::vector<T> sortedChunk(chunkBegin, chunkEnd);
            std::sort(sortedChunk.begin(), sortedChunk.end());
            return buildTreeFromSortedRange<T>(sortedChunk.begin(), sortedChunk.end());
        }));
    }

    // Collect partial trees
    std::vector<RBTree<T>> partialTrees;
    partialTrees.reserve(numThreads);
    for (auto& fut : futures) {
        partialTrees.push_back(fut.get());
    }

    // Sequentially merge all partial trees
    RBTree<T> result;
    for (const auto& tree : partialTrees) {
        result = mergeTrees(result, tree);
    }

    return result;
}
