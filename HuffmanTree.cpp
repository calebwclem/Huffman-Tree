//
// Created by Caleb Clements on 10/27/25.
//

#include "HuffmanTree.h"

HuffmanTree::~HuffmanTree() {
    destroy(root_);
    root_ = nullptr;
}

void HuffmanTree::destroy(TreeNode* n) noexcept {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

HuffmanTree HuffmanTree::buildFromCounts(const std::vector<std::pair<std::string, std::size_t>>& counts) {
    // Edge case: no tokens -> empty tree
    if (counts.empty()) {
        return HuffmanTree(nullptr);
    }

    // 1) Create one LEAF per (word,count). We allocate with `new` here and
    //    let HuffmanTree's destructor own+delete the whole final tree.
    std::vector<TreeNode*> leaves;
    leaves.reserve(counts.size());
    for (const auto& [w, c] : counts) {
        // Note: 'counts' is lex by word from your BST; PQ will re-order by (freq desc, key_word asc)
        auto* leaf = new TreeNode(w, c);   // leaf: key_word = word
        leaves.push_back(leaf);
    }

    // If there is only one distinct word, that single leaf IS the root.
    if (leaves.size() == 1) {
        return HuffmanTree(leaves[0]);
    }

    // 2) Seed our non-owning PriorityQueue; MIN item sits at the BACK.
    PriorityQueue pq(std::move(leaves));

    // 3) Merge two minima until one node remains.
    //    The first extracted min becomes LEFT (code '0'),
    //    the second extracted min becomes RIGHT (code '1').
    while (pq.size() >= 2) {
        TreeNode* a = pq.extractMin();     // smallest
        TreeNode* b = pq.extractMin();     // next smallest
        TreeNode* parent = new TreeNode(a, b); // sets count=sum, key_word=min
        pq.insert(parent);                 // re-insert; PQ restores ordering
    }

    // 4) The final remaining node is the root.
    TreeNode* root = pq.extractMin();      // PQ now empty
    return HuffmanTree(root);
}


void HuffmanTree::buildCodebook(std::unordered_map<std::string,std::string>& out) const {
    out.clear();
    if (!root_) return;
    std::vector<std::pair<std::string,std::string>> pairs; // (word,code)
    std::string prefix;
    assignCodesDFS(root_, prefix, pairs);
    for (auto& [w,c] : pairs) out.emplace(w, c);
}

void HuffmanTree::assignCodesDFS(const TreeNode* n, std::string& prefix,
                                 std::vector<std::pair<std::string,std::string>>& out) {
    if (!n)
        return;

    if (!n->left && !n->right) {
        out.emplace_back(n->word, prefix.empty() ? "0" : prefix);
        return;
    }
    prefix.push_back('0');
    assignCodesDFS(n->left,  prefix, out);
    prefix.pop_back();


    prefix.push_back('1');
    assignCodesDFS(n->right, prefix, out);
    prefix.pop_back();
}

error_type HuffmanTree::writeHeader(std::ostream& os) const {
    if (!root_) return NO_ERROR; // empty header ok
    std::string prefix;
    writeHeaderPreorder(root_, prefix, os);
    if (os.fail()) return FAILED_TO_WRITE_FILE;
    return NO_ERROR;
}

void HuffmanTree::writeHeaderPreorder(const TreeNode* n, std::string& prefix, std::ostream& os) {
    //Empty tree -> just return
    if (!n) return;

    //Check if this is a leaf node
    if (!n->left && !n->right) {
        os << n->word << ' ' << (prefix.empty() ? "0" : prefix) << '\n';
        return;
    }

    //Traverse left subtree
    if (n->left != nullptr) {
        prefix.push_back('0');
        writeHeaderPreorder(n->left,  prefix, os);
        prefix.pop_back();
    }

    //traverse right subtree
    if (n->right != nullptr) {
        prefix.push_back('1');
        writeHeaderPreorder(n->right, prefix, os);
        prefix.pop_back();
    }
}

error_type HuffmanTree::encode(const std::vector<std::string>& tokens,
                               std::ostream& os_bits, int wrap_cols) const {
    std::unordered_map<std::string,std::string> code;
    buildCodebook(code);
    std::size_t col = 0;

    for (const auto& t : tokens) {
        auto it = code.find(t);
        if (it == code.end()) return FAILED_TO_WRITE_FILE; // or a custom mismatch error
        const std::string& bits = it->second;
        for (char b : bits) {
            os_bits.put(b);
            if (++col == static_cast<std::size_t>(wrap_cols)) {
                os_bits.put('\n');
                col = 0;
            }
        }
    }
    if (col != 0) os_bits.put('\n'); // final newline
    if (os_bits.fail()) return FAILED_TO_WRITE_FILE;
    return NO_ERROR;
}
