//
// Created by Ali Kooshesh on 9/27/25.
//

#include "Scanner.hpp"

#include <utility>
#include <iostream>
#include <fstream>
#include <cctype>

#include "utils.hpp"

Scanner::Scanner(std::filesystem::path inputPath) : inputPath_(std::move(inputPath)) {
    // Store the input file path for later use in tokenize()
}

error_type Scanner::tokenize(std::vector<std::string>& words) {
    // Open the input file
    std::ifstream infile(inputPath_);
    if (!infile.is_open()) {
        return UNABLE_TO_OPEN_FILE;
    }

    // Read tokens until EOF
    std::string token;
    while (!(token = readWord(infile)).empty()) {
        words.push_back(token);
    }

    infile.close();
    return NO_ERROR;
}

error_type Scanner::tokenize(std::vector<std::string>& words,
                              const std::filesystem::path& outputFile) {
    // Call the in-memory version first
    error_type result = tokenize(words);
    if (result != NO_ERROR) {
        return result;
    }

    // Write the tokens to the output file
    return writeVectorToFile(outputFile.string(), words);
}

std::string Scanner::readWord(std::istream& in) {
    std::string token;
    int ch;
    
    // Skip separators until we find a letter or EOF
    while ((ch = in.get()) != EOF) {
        // Convert to lowercase if it's an ASCII letter
        if (ch >= 'A' && ch <= 'Z') {
            ch = ch + ('a' - 'A');
        }
        
        // Check if it's a lowercase letter (start of token)
        if (ch >= 'a' && ch <= 'z') {
            token += static_cast<char>(ch);
            break;
        }
        // Otherwise it's a separator, keep skipping
    }
    
    // If we hit EOF without finding a letter, return empty string
    if (token.empty()) {
        return "";
    }
    
    // Continue reading the token
    while ((ch = in.get()) != EOF) {
        // Convert to lowercase if uppercase ASCII letter
        if (ch >= 'A' && ch <= 'Z') {
            ch = ch + ('a' - 'A');
        }
        
        // If it's a letter, add it
        if (ch >= 'a' && ch <= 'z') {
            token += static_cast<char>(ch);
        }
        // If it's an apostrophe, peek ahead
        else if (ch == '\'') {
            // Peek at the next character
            int next = in.peek();
            
            // Convert next to lowercase if uppercase
            if (next >= 'A' && next <= 'Z') {
                next = next + ('a' - 'A');
            }
            
            // If next is a letter, include apostrophe and continue
            if (next >= 'a' && next <= 'z') {
                token += '\'';
            } else {
                // Apostrophe not followed by letter, end token
                break;
            }
        }
        // Any other character is a separator, end token
        else {
            break;
        }
    }
    
    return token;
}
