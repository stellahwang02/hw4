#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
// #include <algorithm>
using namespace std;

// You may add any prototypes of helper functions here

bool isLeaf(Node* node) {
  return node != nullptr && node->left == nullptr && node->right == nullptr;
}

bool equalPathsHelper(Node* root, int depth, int& leafDepth) {
  if (root == nullptr) {
    return true;
  }
  if (isLeaf(root)) {
    if (leafDepth == -1) {
      leafDepth = depth;
    }
    else if (leafDepth != depth) {
      return false;
    }
    return true;
  }
  return equalPathsHelper(root->left, depth + 1, leafDepth) &&
       equalPathsHelper(root->right, depth + 1, leafDepth);
}

bool equalPaths(Node* root) {
  int leafDepth = -1;
  return equalPathsHelper(root, 0, leafDepth);
}

// int findDepth(Node* node) {
//     if (node == nullptr) {
//         return 0;
//     }
    
//     int leftDepth = findDepth(node->left);
//     int rightDepth = findDepth(node->right);
    
//     // If the current node is a leaf, return its depth
//     if (node->left == nullptr && node->right == nullptr) {
//         return 1;
//     }
    
//     // Otherwise, return the maximum depth from its children
//     return 1 + std::max(leftDepth, rightDepth);
// }

// bool checkEqualPaths(Node* node, int depth) {
//     if (node == nullptr) {
//         return true;
//     }
    
//     // If the current node is a leaf, check if its depth matches the given depth
//     if (node->left == nullptr && node->right == nullptr) {
//         return depth == 1;
//     }
    
//     // Recursively check the left and right subtrees
//     return checkEqualPaths(node->left, depth - 1) && checkEqualPaths(node->right, depth - 1);
// }


// bool equalPaths(Node * root)
// {
//     // Add your code below
//     if (root == nullptr) {
      
//         return true;
//     }
    
//     int depth = findDepth(root);
//     return checkEqualPaths(root, depth);

// }

