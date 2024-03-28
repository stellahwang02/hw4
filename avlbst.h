#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"
#include <cassert>

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // added
    // AVLNode<Key, Value>* root_;
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO

    // helpers
    void updateBalance(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* balance(AVLNode<Key, Value>* node);

    // Helper functions
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
    void rotateRight(AVLNode<Key, Value>* node);
    void rotateLeft(AVLNode<Key, Value>* node);
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

};


/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // TODO

  Node<Key, Value>* curr = this->internalFind(new_item.first);
  if (curr != nullptr) {
    curr->setValue(new_item.second);
    return;
  }

  // Create a new AVLNode with balance 0
    AVLNode<Key, Value>* new_node = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    new_node->setBalance(0);
    
    if (this->root_ == nullptr) {
        this->root_ = new_node;
        return;
    }

    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey())
            current = static_cast<AVLNode<Key, Value>*>(current->getLeft());
        else if (new_item.first > current->getKey())
            current = static_cast<AVLNode<Key, Value>*>(current->getRight());
    }

    new_node->setParent(parent);
    // Insert the new node
    if (new_item.first < parent->getKey()) {
        parent->setLeft(new_node);
    } else {
        parent->setRight(new_node);
    }

    if (std::abs(parent->getBalance()) == 1){
      parent->setBalance(0);
    } 
    else {
      if (new_node == parent->getLeft()){
        parent->setBalance(-1);
      }
      else{
        parent->setBalance(1);
      }
      // Update the balance of parent nodes and perform fix if necessary
      insertFix(parent, new_node);
    }
}



template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* new_node)
{
    if (parent == nullptr || new_node == nullptr) {
        return;
    }

    AVLNode<Key, Value> * g = NULL;
    if (parent->getParent() != NULL){
      g = parent->getParent();
    } else {
      return;
    }
    // Determine whether parent is the left or right child of grandparent
    bool grandparent_is_left_child = (parent == g->getLeft()); 
    bool parent_is_left_child = (new_node == parent->getLeft());

    if (grandparent_is_left_child) {
        g->updateBalance(-1);

        // Check the new balance of grandparent
    int new_balance = g->getBalance();
    if (new_balance == 0) {
        // Case 1: New balance is 0, no further action needed
        return;
    } else if (new_balance == -1) {
        // Case 2: New balance is +/- 1, call insertFix recursively with grandparent and parent
        insertFix(g, parent);
    } else {
            if (parent_is_left_child) {
                    rotateRight(g);
                    parent->setBalance(0);
                    g->setBalance(0);
                  }          
            else {
                    // Zig-zag: Left rotation of parent followed by right rotation of grandparent
                    rotateLeft(parent);
                    rotateRight(g);
                    if (new_node->getBalance() == -1) {
                      parent->setBalance(0);
                      g->setBalance(1);
                    }
                    else if (new_node->getBalance() == 0) {
                      parent->setBalance(0);
                      g->setBalance(0);
                    }
                    else {
                      parent->setBalance(-1);
                      g->setBalance(0);
                    }
                    new_node->setBalance(0);
                }
        } 
    }
    else {
      g->updateBalance(1);
      if (g->getBalance() == 0) {
        return;
      }
      else if (g->getBalance() == 1) {
        insertFix(g, parent);
      }
      else if (g->getBalance() == 2) {
        if (new_node == parent->getRight()) {
          rotateLeft(g);
          parent->setBalance(0);
          g->setBalance(0);
        }
        else {
          // Zig-zag: Right rotation of parent followed by left rotation of grandparent
          rotateRight(parent);
          rotateLeft(g);
           if (new_node->getBalance() == 1) {
              parent->setBalance(0);
              g->setBalance(-1);
            }
            else if (new_node->getBalance() == 0) {
              parent->setBalance(0);
              g->setBalance(0);
            }
            else {
              parent->setBalance(1);
              g->setBalance(0);
            }
            new_node->setBalance(0);
          }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
  // TODO
    if (this->root_ == nullptr) {
        // Tree is empty, nothing to remove
        return;
    }

    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(key));

    if (node == nullptr) {
        // Key not found in the tree, nothing to remove
        return;
    }
    // two children
    if (node->getLeft() != NULL && node->getRight() != NULL) {
      Node<Key, Value>* leaf = BinarySearchTree<Key, Value>::predecessor(node);
      this->nodeSwap(node, (AVLNode<Key, Value>*) leaf);
    }

    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* child = NULL;
    int diff = 0; // Difference in height caused by removal

    if (node->getLeft() == NULL && node->getRight() == NULL) {
      child = NULL;
    }
    else if (node->getLeft() == NULL) {
      child = node->getRight();
    }
    else if (node->getRight() == NULL) {
      child = node->getLeft();
    }
    
    if (parent == NULL) {
      this->root_ = child;
    }
    else {
      if (node == parent->getLeft()) {
        parent->setLeft(child);
        diff = +1;
      }
      else {
        parent->setRight(child);
        diff = -1;
      }
    }
    if (child != NULL) {
      child->setParent(parent);
    }
    delete node;
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{

  // If node is null, return
    if (node == nullptr) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int ndiff = -1; // Height difference for next recursive call
    int bal = node->getBalance();

    if (parent != nullptr) {
        // Compute ndiff based on whether node is left or right child of its parent
        if (node == parent->getLeft()) {
            ndiff = 1;
        } 
    }
  
    if (bal + diff == -2 || bal + diff == 2) {
        // Case 1: balance(node) + diff == -/+ 2
        AVLNode<Key, Value>* child = nullptr;
        if (bal + diff == -2) {
            child = static_cast<AVLNode<Key, Value>*>(node->getLeft());
            AVLNode<Key, Value>* temp;
            if (child->getBalance() <= 0) {
              temp = child->getLeft();
              rotateRight(node);
              if(child->getBalance() == 0) {
                node->setBalance(-1);
                child->setBalance(1);
                return;
              }
              else {
                node->setBalance(0);
                child->setBalance(0);
              }
            }
            else {
              temp = child->getRight();
              rotateLeft(child);
              rotateRight(node); 
              if (temp->getBalance() == -1) {
                node->setBalance(1);
                child->setBalance(0);
              }
              else if (temp->getBalance() == 1) {
                node->setBalance(0);
                child->setBalance(-1);
              }
              else if (temp->getBalance() == 0) {
                node->setBalance(0);
                child->setBalance(0);
              }
              temp->setBalance(0);
            }
            removeFix(parent, ndiff);
        }
        else if (node->getBalance() + diff == 2) { // for pos 2 case 
            child = static_cast<AVLNode<Key, Value>*>(node->getRight());
            AVLNode<Key, Value>* temp;
            if (child->getBalance() >= 0) {
              temp = child->getRight();
              rotateLeft(node);
              if(child->getBalance() == 0) {
                node->setBalance(1);
                child->setBalance(-1);
                return;
              }
              else {
                node->setBalance(0);
                child->setBalance(0);
              }
            }
            else {
              temp = child->getLeft();
              rotateRight(child);
              rotateLeft(node);
              if (temp->getBalance() == -1) {
                node->setBalance(0);
                child->setBalance(1);
              }
              else if (temp->getBalance() == 1) {
                node->setBalance(-1);
                child->setBalance(0);
              }
              else if (temp->getBalance() == 0) {
                node->setBalance(0);
                child->setBalance(0);
              }
              temp->setBalance(0);
            }
            removeFix(parent, ndiff);
        }
      }
      else {
        if (bal == 0) {
          node->updateBalance(diff);
          return;
        }
        else if (bal + diff == 0) {
        // Case 3: balance(node) + diff == 0
        node->setBalance(0);
        // Recursively call removeFix on parent
        }
        removeFix(parent, ndiff);
      }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = static_cast<AVLNode<Key, Value>*>(node->getLeft());
    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(node->getParent());

    // Perform rotation
    node->setLeft(pivot->getRight());
    if (pivot->getRight() != nullptr) {
        pivot->getRight()->setParent(node);
    }
    pivot->setRight(node);
    node->setParent(pivot);
    pivot->setParent(parent);

    // Update parent's child pointer
    if (parent != nullptr) {
        if (parent->getLeft() == node) {
            parent->setLeft(pivot);
        } else {
            parent->setRight(pivot);
        }
    } else {
        // If node is root, update the root
        this->root_ = pivot;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = static_cast<AVLNode<Key, Value>*>(node->getRight());
    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(node->getParent());

    // Perform rotation
    node->setRight(pivot->getLeft()); 
      if (pivot->getLeft() != nullptr) {
        pivot->getLeft()->setParent(node);
      }
      pivot->setLeft(node);
    node->setParent(pivot);
    pivot->setParent(parent);

    // Update parent's child pointer
    if (parent != nullptr) {
        if (parent->getLeft() == node) {
            parent->setLeft(pivot);
        } else {
            parent->setRight(pivot);
        }
    } else {
        // If node is root, update root
        this->root_ = pivot;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalance(AVLNode<Key, Value>* node) {
  int leftHeight = (node->getLeft() == nullptr) ? -1 : node->getLeft()->getHeight();
    int rightHeight = (node->getRight() == nullptr) ? -1 : node->getRight()->getHeight();
    node->setBalance(rightHeight - leftHeight);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::balance(AVLNode<Key, Value>* node) {
  if (node->getBalance() == -2) {
        if (node->getLeft()->getBalance() <= 0) {
            return leftLeftCase(node);
        } else {
            return leftRightCase(node);
        }
    } else if (node->getBalance() == 2) {
        if (node->getRight()->getBalance() >= 0) {
            return rightRightCase(node);
        } else {
            return rightLeftCase(node);
        }
    }
    return node;
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
