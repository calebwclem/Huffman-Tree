//
//

#ifndef IMPLEMENTATION_PRIORITYQUEUE_H
#define IMPLEMENTATION_PRIORITYQUEUE_H
// PriorityQueue.hpp
// Vector-backed "priority queue" for Part 2.
// Stores (word, count) pairs and maintains the invariant:
//   higher frequency first (desc), tie -> word ascending (lexicographic).
//
// Notes:
// - Min element (smallest frequency, or lexicographically last on tie) sits at BACK after build().
// - build() uses std::sort once; insert() re-inserts while preserving order (O(N)).
// - writeFreq(os) emits lines per spec: setw(10) << count << ' ' << word << '\n'.
//
// This module does NOT own any TreeNodes (that’s for a later phase). It only
// stores value pairs for the .freq artifact.

#pragma once
#include <cstddef>     // std::size_t
#include <iosfwd>      // std::ostream (forward decl)
#include <string>      // std::string
#include <vector>      // std::vector
#include "TreeNode.h"
#include <iostream>

class PriorityQueue {
public:
    // Non‑owning: does NOT delete the TreeNode* it stores.
    // The constructor takes an initial set of leaves and sorts them internally.
    explicit PriorityQueue(std::vector<TreeNode*> nodes);
    ~PriorityQueue() = default;

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    // Min accessors (MIN = items_.back() under our ordering)
    [[nodiscard]] TreeNode* findMin() const noexcept;   // nullptr if empty
    TreeNode* extractMin() noexcept;                    // remove+return min, or nullptr
    void deleteMin() noexcept;                          // remove min if present

    // Insert while maintaining the invariant (O(N) due to shifting)
    // Stores the pointer without taking ownership.
    void insert(TreeNode* node);

    // Debug printing
    void print(std::ostream& os = std::cout) const;

private:
    // Invariant: items_ is kept sorted by HigherPriority(a,b)
    // i.e., (freq desc, key_word asc). Therefore the MIN is items_.back().
    // Ownership: items_ does NOT own the pointers.
    std::vector<TreeNode*> items_;

    static bool higherPriority(const TreeNode* a, const TreeNode* b) noexcept; // a before b?
    bool isSorted() const; // for assertions/tests only
};


#endif //IMPLEMENTATION_PRIORITYQUEUE_H