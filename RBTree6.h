#include <cassert>
#include <memory>
#include <iostream>
#include <functional>

enum Color { R, B };

// Node struct that holds value, color, and left/right children
template<typename T>
struct Node {
    Color color;
    std::shared_ptr<const Node<T>> left;
    T value;
    std::shared_ptr<const Node<T>> right;
    
    Node(Color c, std::shared_ptr<const Node<T>> const& lft, T val, std::shared_ptr<const Node<T>> const& rgt)
        : color(c), left(lft), value(val), right(rgt) {}
};

// A functional approach using just functions for the RBTree
template<typename T>
using RBTree = std::shared_ptr<const Node<T>>;

// Utility to check if the tree is empty
template<typename T>
bool isEmpty(const RBTree<T>& t) {
    return !t;
}

// Root value of the tree
template<typename T>
T root(const RBTree<T>& t) {
    assert(!isEmpty(t));
    return t->value;
}

// Left subtree of the tree
template<typename T>
RBTree<T> left(const RBTree<T>& t) {
    assert(!isEmpty(t));
    return t->left;
}

// Right subtree of the tree
template<typename T>
RBTree<T> right(const RBTree<T>& t) {
    assert(!isEmpty(t));
    return t->right;
}

// Insertion operation for RBTree, returns a new tree
template<typename T>
std::function<RBTree<T>(const RBTree<T>&)> insert(T x) {
    return [x](const RBTree<T>& t) -> RBTree<T> {
        if (isEmpty(t)) {
            return std::make_shared<const Node<T>>(R, nullptr, x, nullptr); // New red node
        }

        T y = root(t);
        Color c = t->color;

        if (x < y) {
            return balance(c, insert(x)(left(t)), y, right(t));
        } else if (x > y) {
            return balance(c, left(t), y, insert(x)(right(t)));
        } else {
            return t; // No duplicates
        }
    };
}

// Balancing the tree according to red-black properties
template<typename T>
RBTree<T> balance(Color c, const RBTree<T>& lft, T x, const RBTree<T>& rgt) {
    if (c == B && doubledLeft(lft)) {
        return std::make_shared<const Node<T>>(R,
            paint(left(lft), B),
            root(left(lft)),
            std::make_shared<const Node<T>>(B, left(right(lft)), x, right(rgt)));
    } else if (c == B && doubledRight(rgt)) {
        return std::make_shared<const Node<T>>(R,
            std::make_shared<const Node<T>>(B, left(lft), root(lft), left(rgt)),
            root(rgt),
            paint(right(rgt), B));
    } else if (c == B && doubledRight(lft)) {
        return std::make_shared<const Node<T>>(R,
            std::make_shared<const Node<T>>(B, left(lft), root(lft), left(right(lft))),
            root(right(lft)),
            std::make_shared<const Node<T>>(B, right(right(lft)), x, right(rgt)));
    } else if (c == B && doubledLeft(rgt)) {
        return std::make_shared<const Node<T>>(R,
            std::make_shared<const Node<T>>(B, left(lft), root(lft), right(left(rgt))),
            root(rgt),
            right(rgt));
    }
    return std::make_shared<const Node<T>>(c, lft, x, rgt);
}

// Check if the left subtree is doubled red
template<typename T>
bool doubledLeft(const RBTree<T>& t) {
    return !isEmpty(t) && t->color == R && !isEmpty(left(t)) && left(t)->color == R;
}

// Check if the right subtree is doubled red
template<typename T>
bool doubledRight(const RBTree<T>& t) {
    return !isEmpty(t) && t->color == R && !isEmpty(right(t)) && right(t)->color == R;
}

// Paint the node with a specific color
template<typename T>
RBTree<T> paint(const RBTree<T>& t, Color c) {
    assert(!isEmpty(t));
    return std::make_shared<const Node<T>>(c, left(t), root(t), right(t));
}

template<class T, class F>
void forEach(const RBTree<T>& t, F f) {
    if (!isEmpty(t)) {
        forEach(left(t), f);
        f(root(t));
        forEach(right(t), f);
    }
}

// Example for inserting multiple values
template<typename T, typename Iter>
RBTree<T> inserted(Iter begin, Iter end) {
    RBTree<T> tree = nullptr;
    for (auto it = begin; it != end; ++it) {
        tree = insert(*it)(tree);
    }
    return tree;
}