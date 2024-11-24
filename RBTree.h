#include <cassert>
#include <memory>
#include <iostream>
#include <numeric>

// 1. No red node has a red child.
// 2. Every path from root to empty node contains the same
// number of black nodes.

template<class T>
class RBTree {
    private:
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
        explicit RBTree(std::shared_ptr<const Node> const & node) : _root(node) {} 
        Color rootColor() const {
            assert (!isEmpty());
            return _root->_c;
        }
    public:
        RBTree() {}
        RBTree(Color c, RBTree const & lft, T val, RBTree const & rgt)
            : _root(std::make_shared<const Node>(c, lft._root, val, rgt._root)) {
            assert(lft.isEmpty() || lft.root() < val);
            assert(rgt.isEmpty() || val < rgt.root());
        }
        RBTree(std::initializer_list<T> init) {
            RBTree t;
            for (T v : init) {
                t = t.inserted(v);
            }
            _root = t._root;
        } 
        template<class I>
        RBTree(I b, I e) {
            RBTree t;
            for_each(b, e, [&t](T const & v){
                t = t.inserted(v);
            });
            _root = t._root;
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

        RBTree inserted(T x) const {
            RBTree t = ins(x);
            return RBTree(B, t.left(), t.root(), t.right());
        }
        // 1. No red node has a red child.
        void assert1() const {
            if (!isEmpty()) {
                auto lft = left();
                auto rgt = right();
                if (rootColor() == R) 
                {
                    assert(lft.isEmpty() || lft.rootColor() == B);
                    assert(rgt.isEmpty() || rgt.rootColor() == B);
                }
                lft.assert1();
                rgt.assert1();
            }
        }
        int countB() const {
            if (isEmpty())
                return 0;
            int lft = left().countB();
            int rgt = right().countB();
            assert(lft == rgt);
            return (rootColor() == B)? 1 + lft: lft;
        }
    private:
        RBTree ins(T x) const {
            assert1();
            if (isEmpty())
                return RBTree(R, RBTree(), x, RBTree());
            T y = root();
            Color c = rootColor();
            if (rootColor() == B) {
                if (x < y)
                    return balance(left().ins(x), y, right());
                else if (y < x)
                    return balance(left(), y, right().ins(x));
                else
                    return *this; // no duplicates
            }
            else {
                if (x < y)
                    return RBTree(c, left().ins(x), y, right());
                else if (y < x)
                    return RBTree(c, left(), y, right().ins(x));
                else
                    return *this; // no duplicates
            }
        }
        // Called only when parent is black
        static RBTree balance(RBTree const & lft, T x, RBTree const & rgt) {
            if (lft.doubledLeft())
                return RBTree(R
                            , lft.left().paint(B)
                            , lft.root()
                            , RBTree(B, lft.right(), x, rgt));
            else if (lft.doubledRight())
                return RBTree(R
                            , RBTree(B, lft.left(), lft.root(), lft.right().left())
                            , lft.right().root()
                            , RBTree(B, lft.right().right(), x, rgt));
            else if (rgt.doubledLeft())
                return RBTree(R
                            , RBTree(B, lft, x, rgt.left().left())
                            , rgt.left().root()
                            , RBTree(B, rgt.left().right(), rgt.root(), rgt.right()));
            else if (rgt.doubledRight())
                return RBTree(R
                            , RBTree(B, lft, x, rgt.left())
                            , rgt.root()
                            , rgt.right().paint(B));
            else
                return RBTree(B, lft, x, rgt);
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
    private:
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
/* static int i = 0;
template<class T, class Beg, class End>
RBTree<T> inserted(RBTree<T> t, Beg begin, End end) {
    for (auto it = begin; it != end; ++it) {
        t = t.inserted(*it); // Immutable insertion
    }
    return t;
} */

#include <future>
#include <iterator>

static int count = 0;
template<class T, class Beg, class End>
RBTree<T> inserted(RBTree<T> t, Beg it, End end) {
    std::cout << ++count << '\n';
    if (it == end)
        return t;
    T item = *it;
    auto t1 = inserted(t, ++it, end);
    return t1.inserted(item);

}


// Merges two immutable RBTrees
template<class T>
RBTree<T> mergeTrees(RBTree<T> const& t1, RBTree<T> const& t2) {
    RBTree<T> result = t1;
    forEach(t2, [&result](T const& val) {
        result = result.inserted(val); // Insert elements of t2 into t1
    });
    return result;
}

// Parallel insertion function
template<class T, class Iter>
RBTree<T> insertedParallel(RBTree<T> t, Iter begin, Iter end, size_t threshold = 1000) {
    size_t distance = std::distance(begin, end);

    // Base case: If below the threshold, perform sequential insertion
    if (distance <= threshold) {
        for (auto it = begin; it != end; ++it) {
            t = t.inserted(*it); // Immutable insertion
        }
        return t;
    }

    // Divide the range into two halves
    auto mid = std::next(begin, distance / 2);

    // Launch parallel tasks for left and right halves
    auto leftTask = std::async(std::launch::async, insertedParallel<T, Iter>, t, begin, mid, threshold);
    auto rightTree = insertedParallel(t, mid, end, threshold);

    // Merge the resulting trees
    auto leftTree = leftTask.get();
    return mergeTrees(leftTree, rightTree);
}

/* template<class T, class Beg, class End>
RBTree<T> inserted(RBTree<T> t, Beg it, End end) {
    return std::accumulate(it, end, t, [](const RBTree<T>& tree, const T& item) {
        return tree.inserted(item);  // Insert into the tree immutably.
    });
}  */

/* template<class T>
RBTree<T> treeUnion(RBTree<T> const & a, RBTree<T> const & b)
{
    RBTree<T> res = a;
    forEach(b, [&res, &a](T const & v){
        if (!a.member(v))
            res.inserted(v);
    });
    return res;
} */