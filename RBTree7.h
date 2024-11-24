#include <iostream>
#include <memory>
#include <cassert>
#include <functional>

enum Color { R, B };

template<typename T>
struct Node {
    Color _c;
    T _val;
    std::shared_ptr<const Node> _lft;
    std::shared_ptr<const Node> _rgt;

    Node(Color c, T val, std::shared_ptr<const Node> lft = nullptr, std::shared_ptr<const Node> rgt = nullptr)
        : _c(c), _val(val), _lft(lft), _rgt(rgt) {}
};

// Helper functions to work with trees
template<typename T>
bool isEmpty(const std::shared_ptr<const Node<T>>& root) {
    return !root;
}

template<typename T>
T root(const std::shared_ptr<const Node<T>>& root) {
    assert(!isEmpty(root));
    return root->_val;
}

template<typename T>
std::shared_ptr<const Node<T>> left(const std::shared_ptr<const Node<T>>& root) {
    assert(!isEmpty(root));
    return root->_lft;
}

template<typename T>
std::shared_ptr<const Node<T>> right(const std::shared_ptr<const Node<T>>& root) {
    assert(!isEmpty(root));
    return root->_rgt;
}

template<typename T>
Color rootColor(const std::shared_ptr<const Node<T>>& root) {
    assert(!isEmpty(root));
    return root->_c;
}

// Balance function for Red-Black Tree
template<typename T>
std::shared_ptr<const Node<T>> balance(Color c, const std::shared_ptr<const Node<T>>& lft, T x, const std::shared_ptr<const Node<T>>& rgt) {
    if (c == B && left(lft)->_c == R) {  // Left double
        return std::make_shared<Node<T>>(R, root(lft), left(left(lft)), std::make_shared<Node<T>>(B, x, right(left(lft)), rgt));
    } else if (c == B && right(lft)->_c == R) {  // Right double
        return std::make_shared<Node<T>>(R, root(right(lft)), std::make_shared<Node<T>>(B, x, left(lft), left(right(lft))), right(right(lft)));
    } else if (c == B && left(rgt)->_c == R) {  // Left double on right
        return std::make_shared<Node<T>>(R, root(left(rgt)), std::make_shared<Node<T>>(B, x, lft, left(left(rgt))), right(rgt));
    } else if (c == B && right(rgt)->_c == R) {  // Right double on right
        return std::make_shared<Node<T>>(R, root(rgt), std::make_shared<Node<T>>(B, x, lft, left(rgt)), right(right(rgt)));
    } else {
        return std::make_shared<Node<T>>(c, root(lft), left(lft), right(rgt));
    }
}

// Insertion function
template<typename T>
std::shared_ptr<const Node<T>> insert(const std::shared_ptr<const Node<T>>& root, T x) {
    if (isEmpty(root)) {
        return std::make_shared<Node<T>>(R, x);
    }

    T y = root(root);
    Color c = rootColor(root);

    if (x < y) {
        return balance(c, insert(left(root), x), y, right(root));
    } else if (y < x) {
        return balance(c, left(root), y, insert(right(root), x));
    } else {
        return root; // No duplicates
    }
}

// Paint the root color of the tree
template<typename T>
std::shared_ptr<const Node<T>> paint(const std::shared_ptr<const Node<T>>& root, Color c) {
    assert(!isEmpty(root));
    return std::make_shared<Node<T>>(c, root(root), left(root), right(root));
}

// Utility to perform in-order traversal
template<typename T, typename F>
void forEach(const std::shared_ptr<const Node<T>>& root, F f) {
    if (!isEmpty(root)) {
        forEach(left(root), f);
        f(root(root));
        forEach(right(root), f);
    }
}

// Insert a range of elements into a Red-Black tree
template<typename T, typename Beg, typename End>
std::shared_ptr<const Node<T>> inserted(std::shared_ptr<const Node<T>> root, Beg it, End end) {
    for (auto i = it; i != end; ++i) {
        root = insert(root, *i);
    }
    return root;
}

