#include <memory>
#include <cassert>

enum Color { RED, BLACK };

template <typename T>
class RBTree {
    struct Node {
        Color color;
        T value;
        std::shared_ptr<Node> left, right;

        Node(Color c, T val, std::shared_ptr<Node> lft = nullptr, std::shared_ptr<Node> rgt = nullptr)
            : color(c), value(val), left(lft), right(rgt) {}
    };

    std::shared_ptr<Node> root;

    std::shared_ptr<Node> balance(Color c, std::shared_ptr<Node> left, T val, std::shared_ptr<Node> right) {
        // Balancing cases for red-black tree
        if (c == BLACK && left && left->color == RED) {
            if (left->left && left->left->color == RED) { // Left-left case
                return std::make_shared<Node>(RED,
                                              left->value,
                                              std::make_shared<Node>(BLACK, left->left->value, left->left->left, left->left->right),
                                              std::make_shared<Node>(BLACK, val, left->right, right));
            }
            if (left->right && left->right->color == RED) { // Left-right case
                return std::make_shared<Node>(RED,
                                              left->right->value,
                                              std::make_shared<Node>(BLACK, left->value, left->left, left->right->left),
                                              std::make_shared<Node>(BLACK, val, left->right->right, right));
            }
        }
        if (c == BLACK && right && right->color == RED) {
            if (right->left && right->left->color == RED) { // Right-left case
                return std::make_shared<Node>(RED,
                                              right->left->value,
                                              std::make_shared<Node>(BLACK, val, left, right->left->left),
                                              std::make_shared<Node>(BLACK, right->value, right->left->right, right->right));
            }
            if (right->right && right->right->color == RED) { // Right-right case
                return std::make_shared<Node>(RED,
                                              right->value,
                                              std::make_shared<Node>(BLACK, val, left, right->left),
                                              std::make_shared<Node>(BLACK, right->right->value, right->right->left, right->right->right));
            }
        }
        return std::make_shared<Node>(c, val, left, right); // Default case
    }

    std::shared_ptr<Node> insert(std::shared_ptr<Node> node, T val) {
        if (!node) return std::make_shared<Node>(RED, val);
        if (val < node->value)
            return balance(node->color, insert(node->left, val), node->value, node->right);
        else if (val > node->value)
            return balance(node->color, node->left, node->value, insert(node->right, val));
        return node; // No duplicates
    }

public:
    RBTree() : root(nullptr) {}

    void insert(T val) {
        root = insert(root, val);
        if (root) root->color = BLACK; // Ensure root is always black
    }

    bool find(T val) const {
        auto node = root;
        while (node) {
            if (val < node->value)
                node = node->left;
            else if (val > node->value)
                node = node->right;
            else
                return true;
        }
        return false;
    }

    bool isEmpty() const { return !root; }
    T treeRoot() const {
        assert(!isEmpty());
        return root->value;
    }
    RBTree left() const {
        assert(!isEmpty());
        return RBTree(root->left);
    }
    RBTree right() const {
        assert(!isEmpty());
        return RBTree(root->right);
    }
};

template<class T, class F>
void forEach(RBTree<T> const & t, F f) {
    if (!t.isEmpty()) {
        forEach(t.left(), f);
        f(t.treeRoot());
        forEach(t.right(), f);
    }
}

template <class T, class U> 
RBTree<T> insert(RBTree<T>&& tree, U& data) { 
    for (const auto& str : data) {
        tree.insert(str); // Insert sorted data into tree
    }
    return tree;
}