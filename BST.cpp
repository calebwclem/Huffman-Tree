//
//
//
// BST.cpp
//
// Skeleton implementation for a Binary Search Tree that counts word frequencies,
// per the Project 3 (Huffman) Part 2 spec.
//
// This file provides function *definitions* matching the public/private API
// promised in BST.hpp. Bodies are left as TODOs with detailed comments that
// explain the expected behavior, invariants, and edge cases.
//
// Notes from the spec:
// - Keys are lowercase ASCII words from the Scanner (deterministic tokenization).
// - Insert: if key exists, increment its count; otherwise create a new node
//   with count = 1.
// - In-order traversal must yield (word, count) pairs in lexicographic order
//   by word (ascending).
// - Height: empty tree = 0; otherwise 1 + max(height(left), height(right)).
// - Complexity (typical): with randomized insertion order, expected O(log V)
//   per insert where V is distinct-vocabulary size. Worst case (skew) can be
//   linear depth.
// - This module does *not* write files; callers use the returned vector to
//   build the frequency-sorted list and .freq output in a separate module.

#include "BST.h"
#include <algorithm> // optional; may be useful later
#include <string>
#include <string_view>
#include <optional>
#include <utility>
#include <vector>
#include "TreeNode.h"

// ===============================
//  Public API
// ===============================

BST::~BST() {
    // Expectation:
    // - Recursively delete every node in the tree to avoid leaks.
    // - Safe on empty trees (root_ == nullptr).
    destroy(root_);
    root_ = nullptr;
}

void BST::insert(const std::string& word) {
    // Expected behavior (spec):
    // - If 'word' already exists, increment its count and do NOT create a new node.
    // - Otherwise, create a new node with count = 1 at the correct BST position.
    // - Key comparison: lexicographic on the lowercase word string (Scanner already lowercases).
    //
    // Complexity:
    // - Average O(log V) with randomized insertion order; worst-case O(V) when skewed.
    root_ = insertHelper(root_, word);
}

void BST::bulkInsert(const std::vector<std::string>& words) {
    // Convenience wrapper:
    // - Insert each token using insert().
    // - Caller may or may not have randomized the order; your logic must not assume it.
    //
    // Performance: O(T * cost(insert)), where T is number of tokens.
    for (const auto& w : words) {
        insert(w);
    }
}

bool BST::contains(std::string_view w) const noexcept {
    // Return true if the word exists in the BST; otherwise false.
    // No side effects.
    return findNode(root_, w) != nullptr;
}

std::optional<size_t> BST::countOf(std::string_view w) const noexcept {
    // Return the frequency (count) for a word if present; otherwise std::nullopt.
    // No side effects.
    const TreeNode* n = findNode(root_, w);
    if (!n) return std::nullopt;
    return n->count;
}

void BST::inorderCollect(std::vector<std::pair<std::string, size_t>>& out) const {
    // Produce a flat vector of (word, count) pairs in lexicographic order by word (ascending).
    // Deterministic and independent of insertion order.
    //
    // Contract:
    // - 'out' is appended to (caller may pass an existing vector); clear it beforehand
    //   in the caller if you need a fresh list.
    inorderHelper(root_, out);
}

size_t BST::size() const noexcept {
    // Return the number of distinct words (i.e., number of nodes).
    return sizeHelper(root_);
}

unsigned BST::height() const noexcept {
    // Return:
    // - 0 if the tree is empty (spec requirement for Part 2).
    // - Otherwise 1 + max(height(left), height(right)).
    return heightHelper(root_);
}

// ===============================
//  Private helpers
// ===============================

void BST::destroy(BST::TreeNode* n) noexcept {
    // Recursively delete the entire subtree rooted at n.
    // Safe on nullptr.
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

BST::TreeNode* BST::insertHelper(BST::TreeNode* n, const std::string& w) {
    // Standard BST insert on key 'w':
    // - If n is nullptr: create Node(w) with count=1 and return it.
    // - If w == n->word: ++n->count and return n.
    // - If w <  n->word: n->left  = insertHelper(n->left, w);  return n.
    // - If w >  n->word: n->right = insertHelper(n->right, w); return n.
    //
    // Tie-breaking:
    // - Exact string equality is the only case that increments.
    // - No balancing/rotations in this assignment.
    //
    if (!n) return new TreeNode(w);
    if (w == n->word) {
        ++n->count;
        return n;
    }
    if (w < n->word) {
        n->left = insertHelper(n->left, w);
    } else {
        n->right = insertHelper(n->right, w);
    }
    return n;
}

const BST::TreeNode* BST::findNode(const TreeNode* n, std::string_view w) noexcept {
    // Iterative or recursive lookup:
    // - Follow BST ordering until matching node or nullptr.
    while (n) {
        if (w == n->word) return n;
        if (w < n->word)  n = n->left;
        else              n = n->right;
    }
    return nullptr;
}

void BST::inorderHelper(const TreeNode* n,
                        std::vector<std::pair<std::string, size_t>>& out) {
    // In-order traversal:
    // - Visit left, then node, then right.
    // - Push back (word, count) pairs in that order.
    if (!n) return;
    inorderHelper(n->left, out);
    out.emplace_back(n->word, n->count);
    inorderHelper(n->right, out);
}

size_t BST::sizeHelper(const TreeNode* n) noexcept {
    // Return number of nodes in subtree.
    if (!n) return 0;
    return 1 + sizeHelper(n->left) + sizeHelper(n->right);
}

unsigned BST::heightHelper(const TreeNode* n) noexcept {
    // Empty subtree height = 0 (per Part 2 spec).
    // Non-empty: 1 + max(h(left), h(right)).
    if (!n) return 0;
    unsigned hl = heightHelper(n->left);
    unsigned hr = heightHelper(n->right);
    return 1 + (hl > hr ? hl : hr);
}
