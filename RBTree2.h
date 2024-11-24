#include <iostream>
#include <memory>
#include <cassert>

enum Color { RED, BLACK };

template <typename T>
struct Node {
    T data;
    Color color;
    std::shared_ptr<Node> left, right, parent;

    Node(T data) : data(data), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

template <typename T>
class RBTree {
private:
    std::shared_ptr<Node<T>> root;

    void rotateLeft(std::shared_ptr<Node<T>> &node) {
        auto rightChild = node->right;
        node->right = rightChild->left;
        if (node->right) 
            node->right->parent = node;
        rightChild->parent = node->parent;
        if (!node->parent) 
            root = rightChild;
        else if (node == node->parent->left)
            node->parent->left = rightChild;
        else node->parent->right = rightChild;
            rightChild->left = node;
        node->parent = rightChild;
    }

    void rotateRight(std::shared_ptr<Node<T>> &node) {
        auto leftChild = node->left;
        node->left = leftChild->right;
        if (node->left) node->left->parent = node;
        leftChild->parent = node->parent;
        if (!node->parent) root = leftChild;
        else if (node == node->parent->left) node->parent->left = leftChild;
        else node->parent->right = leftChild;
        leftChild->right = node;
        node->parent = leftChild;
    }

    void fixInsert(std::shared_ptr<Node<T>> &node) {
        std::shared_ptr<Node<T>> parent = nullptr;
        std::shared_ptr<Node<T>> grandparent = nullptr;
        while (node != root && node->color == RED && node->parent->color == RED) {
            parent = node->parent;
            grandparent = parent->parent;
            if (parent == grandparent->left) {
                auto uncle = grandparent->right;
                if (uncle && uncle->color == RED) {
                    grandparent->color = RED;
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    node = grandparent;
                } else {
                    if (node == parent->right) {
                        rotateLeft(parent);
                        node = parent;
                        parent = node->parent;
                    }
                    rotateRight(grandparent);
                    std::swap(parent->color, grandparent->color);
                    node = parent;
                }
            } else {
                auto uncle = grandparent->left;
                if (uncle && uncle->color == RED) {
                    grandparent->color = RED;
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    node = grandparent;
                } else {
                    if (node == parent->left) {
                        rotateRight(parent);
                        node = parent;
                        parent = node->parent;
                    }
                    rotateLeft(grandparent);
                    std::swap(parent->color, grandparent->color);
                    node = parent;
                }
            }
        }
        root->color = BLACK;
    }

public:
    RBTree() : root(nullptr) {}

    void insert(const T &data) {
        auto node = std::make_shared<Node<T>>(data);
        if (!root) {
            root = node;
            root->color = BLACK;
        } else {
            auto temp = root;
            std::shared_ptr<Node<T>> parent = nullptr;
            while (temp) {
                parent = temp;
                if (node->data < temp->data) temp = temp->left;
                else temp = temp->right;
            }
            node->parent = parent;
            if (node->data < parent->data) parent->left = node;
            else parent->right = node;
            fixInsert(node);
        }
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

    void inorderTraversal(std::shared_ptr<Node<T>> node) const {
        if (!node) return;
        inorderTraversal(node->left);
        std::cout << node->data << " ";
        inorderTraversal(node->right);
    }

    void display() const {
        inorderTraversal(root);
        std::cout << std::endl;
    }
};

template <class T, class Iterator> 
RBTree<T> insert(RBTree<T> tree, Iterator begin, Iterator end) { 
    for (auto it = begin; it != end; ++it) { 
        tree = tree.insert(*it); 
    } 
    return tree; 
}
