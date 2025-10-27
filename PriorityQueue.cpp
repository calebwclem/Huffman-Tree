// PriorityQueue.cpp
#include "PriorityQueue.h"   // or whatever your header is named
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

// ---- Assumed TreeNode (leaf) fields used by this PQ ----
// For Part 2, PQ holds ONLY leaves, so we rely on:
//   node->word      : std::string (the token string)
//   node->key_word  : std::string (for ordering; for leaves it's == word)
//   node->freq      : std::size_t (frequency)
// Left/right pointers may exist but are unused here.

PriorityQueue::PriorityQueue(std::vector<TreeNode*> nodes)
: items_(std::move(nodes)) {
    std::sort(items_.begin(), items_.end(),
              [](const TreeNode* a, const TreeNode* b) { return higherPriority(a, b); });
    // Optional: assert(isSorted());
}

std::size_t PriorityQueue::size() const noexcept {
    return items_.size();
}
bool PriorityQueue::empty() const noexcept {
    return items_.empty();
}

TreeNode* PriorityQueue::findMin() const noexcept {
    return items_.empty() ? nullptr : items_.back();   // MIN at back (freq asc at back)
}

TreeNode* PriorityQueue::extractMin() noexcept {
    if (items_.empty()) return nullptr;
    TreeNode* min = items_.back();
    items_.pop_back();
    return min;
}

void PriorityQueue::deleteMin() noexcept {
    if (!items_.empty()) items_.pop_back();
}

void PriorityQueue::insert(TreeNode* node) {
    // Keep items_ sorted by higherPriority (desc by freq, tie word asc).
    auto before = [](const TreeNode* a, const TreeNode* b) {
        return higherPriority(a, b);
    };
    auto pos = std::lower_bound(items_.begin(), items_.end(), node, before);
    items_.insert(pos, node);
    // Optional: assert(isSorted());
}

bool PriorityQueue::higherPriority(const TreeNode* a, const TreeNode* b) noexcept {
    if (a->count != b->count) return a->count > b->count;     // higher freq first
    return a->key_word < b->key_word;                     // tie: lexicographic asc
}

bool PriorityQueue::isSorted() const {
    for (std::size_t i = 1; i < items_.size(); ++i) {
        if (!higherPriority(items_[i-1], items_[i])) return false;
    }
    return true;
}

void PriorityQueue::print(std::ostream& os) const {
    // Emit .freq format per spec: right-justified freq in width 10, ONE space, then word, newline.
    for (const auto* n : items_) {
        // In Part 2, items are leaves: n->word is the token string.
        os << std::setw(10) << n->count << ' ' << n->word << '\n';
    }
}
