//
// Created by Caleb Clements on 10/17/25.
//
#pragma once
#include <string>

#ifndef IMPLEMENTATION_TREENODE_H
#define IMPLEMENTATION_TREENODE_H

struct TreeNode {
    std::string word;
    std::string key_word;
    size_t count = 1;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
    explicit TreeNode(std::string w): word(std::move(w)) {}
    TreeNode(std::string w, std::size_t f): word(std::move(w)), count(f) {}
};

#endif //IMPLEMENTATION_TREENODE_H