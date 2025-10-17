//
// Created by Caleb Clements on 10/17/25.
//

#ifndef IMPLEMENTATION_BST_H
#define IMPLEMENTATION_BST_H

#pragma once
#include <string>
#include <vector>
#include <optional>

class BST {
public:
    BST() = default; //Constructor
    ~BST(); //destructor: calls destroy(root_)

    void insert(const std::string& word);             // count++
    void bulkInsert(const std::vector<std::string>&); // convenience

    [[nodiscard]] bool contains(std::string_view w) const noexcept;
    [[nodiscard]] std::optional<size_t> countOf(std::string_view w) const noexcept;

    // In-order lexicographic (word asc) → flat (word, count)
    void inorderCollect(std::vector<std::pair<std::string,size_t>>& out) const;

    [[nodiscard]] size_t size() const noexcept;   // distinct words
    [[nodiscard]] unsigned height() const noexcept; // empty = 0

private:
    struct TreeNode {
        std::string word;
        size_t count = 1;
        TreeNode* left = nullptr;
        TreeNode* right = nullptr;
        explicit TreeNode(std::string w): word(std::move(w)) {}
    };

    TreeNode* root_ = nullptr;

    static void destroy(TreeNode* n) noexcept;
    static TreeNode* insertHelper(TreeNode* n, const std::string& w);
    static const TreeNode* findNode(const TreeNode* n, std::string_view w) noexcept;
    static void inorderHelper(const TreeNode* n, std::vector<std::pair<std::string,size_t>>& out);
    static size_t sizeHelper(const TreeNode* n) noexcept;
    static unsigned heightHelper(const TreeNode* n) noexcept;
};



#endif //IMPLEMENTATION_BST_H