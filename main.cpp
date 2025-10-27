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

/*
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
