// main.cpp — Part 3 end-to-end driver: Scanner → BST → .freq → Huffman(.hdr + .code)
// Adjust include names if yours differ.
// main.cpp — final driver: expects ./input_output/<base>.txt ONLY
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Scanner.hpp"
#include "BST.h"
#include "PriorityQueue.h"
#include "HuffmanTree.h"

namespace fs = std::filesystem;

static void usage(const char* prog) {
    std::cerr << "Usage: " << prog << " <base>.txt\n"
              << "  (input file must be located in ./input_output)\n";
    std::exit(1);
}

int main(int argc, char* argv[]) {
    if (argc != 2) usage(argv[0]);

    // Enforce input_output/ policy
    fs::path filename = fs::path(argv[1]).filename();     // ignore any path the user passed
    if (filename.extension() != ".txt") {
        std::cerr << "Error: expected a .txt file name (e.g., TheBells.txt)\n";
        return 1;
    }

    fs::path dir = fs::path("input_output");
    if (!fs::exists(dir)) {
        std::cerr << "Error: input directory does not exist: " << dir << "\n";
        return 2;
    }

    fs::path in = dir / filename;
    std::ifstream fin(in);
    if (!fin) {
        std::cerr << "Error: input file not found or cannot be opened: " << in << "\n";
        return 3;
    }

    // Derive outputs in the SAME directory
    std::string base = in.stem().string();
    fs::path tokensPath = dir / (base + ".tokens");
    fs::path freqPath   = dir / (base + ".freq");
    fs::path hdrPath    = dir / (base + ".hdr");
    fs::path codePath   = dir / (base + ".code");

    // 1) Scanner → tokens + .tokens
    std::vector<std::string> tokens;
    {
        Scanner sc{in};
        error_type err = sc.tokenize(tokens, tokensPath);
        if (err != NO_ERROR) {
            std::cerr << "Error: scanner/tokenizer failed (" << err << ") for " << in << "\n";
            return 4;
        }
    }

    // 2) BST → counts (lex by word)
    BST bst;
    bst.bulkInsert(tokens);

    std::vector<std::pair<std::string, std::size_t>> counts_lex;
    counts_lex.reserve(bst.size());
    bst.inorderCollect(counts_lex); // appends in word-ascending order

    // Required BST stats
    unsigned H = bst.height();
    std::size_t U = bst.size();
    std::size_t T = tokens.size();
    std::size_t MIN = 0, MAX = 0;
    if (!counts_lex.empty()) {
        MIN = counts_lex.front().second;
        MAX = counts_lex.front().second;
        for (const auto& p : counts_lex) {
            if (p.second < MIN) MIN = p.second;
            if (p.second > MAX) MAX = p.second;
        }
    }
    std::cout << "BST height: " << H << "\n";
    std::cout << "BST unique words: " << U << "\n";
    std::cout << "Total tokens: " << T << "\n";
    std::cout << "Min frequency: " << MIN << "\n";
    std::cout << "Max frequency: " << MAX << "\n";

    // 3) .freq via PriorityQueue (count desc, tie key_word/word asc)
    {
        std::vector<std::unique_ptr<TreeNode>> owners;
        owners.reserve(counts_lex.size());
        std::vector<TreeNode*> raw;
        raw.reserve(counts_lex.size());

        for (const auto& [w, c] : counts_lex) {
            auto node = std::make_unique<TreeNode>(w); // if you have (w,c) ctor, use it instead
            node->count = c;
            node->left = node->right = nullptr;
            raw.push_back(node.get());
            owners.emplace_back(std::move(node));
        }

        PriorityQueue pq(std::move(raw));
        std::ofstream ofs(freqPath);
        if (!ofs) {
            std::cerr << "Error: unable to open output .freq: " << freqPath << "\n";
            return 5;
        }
        pq.print(ofs); // uses setw(10) << count << ' ' << word << '\n'
        if (!ofs) {
            std::cerr << "Error: failed while writing .freq: " << freqPath << "\n";
            return 6;
        }
    }

    // 4) Huffman tree → .hdr and .code
    HuffmanTree htree = HuffmanTree::buildFromCounts(counts_lex);

    // .hdr (pre-order over leaves: "word code")
    {
        std::ofstream hdr(hdrPath);
        if (!hdr) {
            std::cerr << "Error: unable to open output .hdr: " << hdrPath << "\n";
            return 7;
        }
        error_type err = htree.writeHeader(hdr);
        if (err != NO_ERROR || !hdr) {
            std::cerr << "Error: failed while writing .hdr: " << hdrPath << "\n";
            return 8;
        }
    }

    // .code (ASCII 0/1 wrapped to 80 cols, final newline)
    {
        std::ofstream code(codePath);
        if (!code) {
            std::cerr << "Error: unable to open output .code: " << codePath << "\n";
            return 9;
        }
        error_type err = htree.encode(tokens, code, 80);
        if (err != NO_ERROR || !code) {
            std::cerr << "Error: failed while writing .code: " << codePath << "\n";
            return 10;
        }
    }

    return 0;
}
//End main phase 3 (final phase)

/*
// main_part2.cpp
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Scanner.hpp"       // adjust if yours is Scanner.h
#include "BST.h"             // adjust if yours is BST.hpp
#include "PriorityQueue.h"   // adjust if yours is PriorityQueue.hpp

namespace fs = std::filesystem;

static void print_usage_and_exit(const char* prog) {
    std::cerr << "Usage: " << prog << " input_output/<base>.txt\n";
    std::exit(1);
}


int main(int argc, char* argv[]) {
    // ---- 1) CLI parsing (exactly one argument) ----
    if (argc != 2) {
        print_usage_and_exit(argv[0]);
    }
    fs::path in = fs::path(argv[1]);
    if (!in.has_parent_path() || !fs::exists(in.parent_path())) {
        std::cerr << "Error: input directory does not exist: " << in.parent_path() << "\n";
        return 2;
    }
    if (!fs::exists(in)) {
        std::cerr << "Error: input file not found: " << in << "\n";
        return 3;
    }

    // Derive output paths in the SAME directory as input
    fs::path dir  = in.parent_path();
    std::string base = in.stem().string(); // removes final .txt
    fs::path tokensPath = dir / (base + ".tokens");
    fs::path freqPath   = dir / (base + ".freq");

    // ---- 2) Scanner: tokenize (and write .tokens) ----
    std::vector<std::string> tokens;
    {
        Scanner sc{in};
        error_type err = sc.tokenize(tokens, tokensPath);
        if (err != NO_ERROR) {
            std::cerr << "Error: scanner/tokenizer failed with code " << err
                      << " on " << in << "\n";
            return 4;
        }
    }

    // ---- 3) BST: build and collect counts (lexicographic by word) ----
    BST bst;
    bst.bulkInsert(tokens);

    std::vector<std::pair<std::string, std::size_t>> counts_lex;
    counts_lex.reserve(bst.size());
    bst.inorderCollect(counts_lex); // NOTE: inorderCollect appends

    // ---- 4) Print BST measures (exact labels & order) ----
    const unsigned H = bst.height();
    const std::size_t U = bst.size();
    const std::size_t T = tokens.size();
    std::size_t MIN = 0, MAX = 0;
    if (!counts_lex.empty()) {
        MIN = counts_lex.front().second;
        MAX = counts_lex.front().second;
        for (const auto& p : counts_lex) {
            MIN = std::min(MIN, p.second);
            MAX = std::max(MAX, p.second);
        }
    }

    std::cout << "BST height: " << H   << "\n";
    std::cout << "BST unique words: " << U << "\n";
    std::cout << "Total tokens: " << T << "\n";
    std::cout << "Min frequency: " << MIN << "\n";
    std::cout << "Max frequency: " << MAX << "\n";

    // ---- 5) PriorityQueue: build from counts and write .freq ----
    // Create leaf nodes (owned here); PQ is NON-OWNING and only stores raw pointers.
    std::vector<std::unique_ptr<TreeNode>> owners;
    owners.reserve(counts_lex.size());
    std::vector<TreeNode*> raw;
    raw.reserve(counts_lex.size());

    for (const auto& [w, c] : counts_lex) {
        auto node = std::make_unique<TreeNode>(w); // your TreeNode(std::string) ctor
        node->count = c;                            // set frequency field
        node->left = node->right = nullptr;        // explicit (not strictly required)
        raw.push_back(node.get());
        owners.emplace_back(std::move(node));
    }

    PriorityQueue pq(std::move(raw)); // sorts internally by (count desc, word asc)

    std::ofstream ofs(freqPath);
    if (!ofs) {
        std::cerr << "Error: unable to open output file for writing: " << freqPath << "\n";
        return 5;
    }
    pq.print(ofs);                      // writes in exact ".freq" format
    if (!ofs) {
        std::cerr << "Error: failed while writing: " << freqPath << "\n";
        return 6;
    }

    // Success
    return 0;
}//End main phase 2 */


/*
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

#include "Scanner.hpp"
#include "utils.hpp"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    const std::string dirName = std::string("input_output");
    const std::string inputFileName = std::string(argv[1]);
    const std::string inputFileBaseName = baseNameWithoutTxt(inputFileName);

    // build the path to the .tokens output file.
    const std::string wordTokensFileName = dirName + "/" + inputFileBaseName + ".tokens";


    // The next several if-statement make sure that the input file, the directory exist
    // and that the output file is writeable.
     if( error_type status; (status = regularFileExistsAndIsAvailable(inputFileName)) != NO_ERROR )
        exitOnError(status, inputFileName);


    if (error_type status; (status = directoryExists(dirName)) != NO_ERROR )
        exitOnError(status, dirName);

    if (error_type status; (status = canOpenForWriting(wordTokensFileName)) != NO_ERROR)
        exitOnError(status, wordTokensFileName);


    std::vector<std::string> words;
    namespace fs = std::filesystem;
    fs::path tokensFilePath(wordTokensFileName); // create a file system path using the output file.

    auto fileToWords = Scanner(inputFileName);
    if( error_type status; (status = fileToWords.tokenize(words)) != NO_ERROR)
	    exitOnError(status, inputFileName);

    if (error_type status; (status = writeVectorToFile(wordTokensFileName, words)) != NO_ERROR)
        exitOnError(status, wordTokensFileName);

    return 0;
}//End main testing phase 1*/
