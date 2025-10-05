//
// Created by Ali Kooshesh on 9/27/25.
//

#ifndef IMPLEMENTATION_FILETOWORDS_HPP
#define IMPLEMENTATION_FILETOWORDS_HPP
#include <string>
#include <vector>
#include <filesystem>

#include "utils.hpp"

class Scanner {
public:
    explicit Scanner(std::filesystem::path inputPath);

    // Tokenize into memory (according to the Rules in this section).
    error_type tokenize(std::vector<std::string>& words);

    // Tokenize and also write one token per line to 'outputFile' (e.g., <base>.tokens).
    // This overload should internally call the in‑memory tokenize() to avoid duplicate logic.
    error_type tokenize(std::vector<std::string>& words,
                        const std::filesystem::path& outputFile);

    ~Scanner() = default;

private:
    // Read the next token from 'in'. Returns empty string when no more tokens.
    // Follows the project’s tokenization rules: letters a–z with optional internal apostrophes;
    // digits, punctuation, hyphens/dashes, whitespace, and non‑ASCII are separators.
    static std::string readWord(std::istream& in);

    std::filesystem::path inputPath_;
};

#endif //IMPLEMENTATION_FILETOWORDS_HPP