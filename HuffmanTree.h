#ifndef IMPLEMENTATION_HUFFMANTREE_H
#define IMPLEMENTATION_HUFFMANTREE_H

#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <unordered_map>

#include "TreeNode.h"
#include "PriorityQueue.h"
#include "utils.hpp" // for error_type if you have it

class HuffmanTree {
public:
    static HuffmanTree buildFromCounts(const std::vector<std::pair<std::string, std::size_t>>& counts);
    ~HuffmanTree();
    HuffmanTree() = default;

    // Build a (word -> code) table (left=0, right=1; pre-order left before right).
    void buildCodebook(std::unordered_map<std::string,std::string>& out) const;

    // Emit leaves in pre-order as "word<space>code\n" (deterministic). Final newline.
    error_type writeHeader(std::ostream& os) const;

    // Encode tokens using this codebook; wrap lines to wrap_cols (80 default), final newline.
    error_type encode(const std::vector<std::string>& tokens,
                      std::ostream& os_bits,
                      int wrap_cols = 80) const;

    // Optional metric
    unsigned height() const noexcept;

private:
    explicit HuffmanTree(TreeNode* root) : root_(root) {}
    TreeNode* root_ = nullptr;

    static void destroy(TreeNode* n) noexcept;
    static void assignCodesDFS(const TreeNode* n,
                               std::string& prefix,
                               std::vector<std::pair<std::string,std::string>>& out);
    static void headerPreorder(const TreeNode* n,
                               std::string& prefix,
                               std::ostream& os);
    static unsigned heightHelper(const TreeNode* n) noexcept;
};

#endif //IMPLEMENTATION_HUFFMANTREE_H