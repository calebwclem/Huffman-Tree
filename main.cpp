
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "PriorityQueue.h"        // <-- adjust if your filename differs
#include "TreeNode.h"           // <-- or the header that defines TreeNode (word, freq, left, right)

static std::unique_ptr<TreeNode> make_leaf(std::string w, std::size_t f) {
    // Adapt to your TreeNode constructor/fields:
    auto n = std::make_unique<TreeNode>(std::move(w), f);
    // If your TreeNode lacks a (word,freq) ctor, set fields manually:
    // n->word = w; n->freq = f; n->left = n->right = nullptr;
    return n;
}

int main() {
    // Own the nodes here; PQ is non-owning
    std::vector<std::unique_ptr<TreeNode>> owners;
    owners.emplace_back(make_leaf("wolf",   42));
    owners.emplace_back(make_leaf("apple",   7));
    owners.emplace_back(make_leaf("camp's",  7));
    owners.emplace_back(make_leaf("fire",    3));
    owners.emplace_back(make_leaf("zebra",   7));
    owners.emplace_back(make_leaf("alpha",   7));

    std::vector<TreeNode*> raw;
    raw.reserve(owners.size());
    for (auto& up : owners) raw.push_back(up.get());

    // Build PQ (sorts inside ctor): order should be
    // 42 wolf, 7 alpha, 7 apple, 7 camp's, 7 zebra, 3 fire   (desc by freq, tie word asc)
    PriorityQueue pq(std::move(raw));
    assert(pq.size() == 6);
    assert(!pq.empty());

    // Min is at the back -> "fire",3
    {
        TreeNode* mn = pq.findMin();
        assert(mn && mn->word == "fire" && mn->count == 3);
    }

    // extractMin removes "fire"
    {
        TreeNode* mn = pq.extractMin();
        assert(mn && mn->word == "fire" && pq.size() == 5);
    }

    // Insert another 7-freq word to test tie-breaker position ("marmot")
    auto extra = make_leaf("marmot", 7);
    TreeNode* extra_raw = extra.get();
    owners.emplace_back(std::move(extra)); // keep ownership alive
    pq.insert(extra_raw);
    assert(pq.size() == 6);

    // Now, popping mins one by one should yield:
    // 7 zebra, 7 marmot, 7 camp's, 7 apple, 7 alpha, 42 wolf
    // (because min is at back; among equal 7s, lexicographically *last* appears first when popping)
    const std::vector<std::pair<std::string,std::size_t>> expected_mins = {
        {"zebra",7}, {"marmot",7}, {"camp's",7}, {"apple",7}, {"alpha",7}, {"wolf",42}
    };

    for (const auto& [ew, ef] : expected_mins) {
        TreeNode* m = pq.findMin();
        assert(m && m->word == ew && m->count == ef);
        pq.deleteMin();
    }
    assert(pq.empty());

    // Rebuild a fresh PQ to test print() (.freq formatting)
    {
        std::vector<std::unique_ptr<TreeNode>> own2;
        own2.emplace_back(make_leaf("wolf",   42));
        own2.emplace_back(make_leaf("alpha",   7));
        own2.emplace_back(make_leaf("apple",   7));
        own2.emplace_back(make_leaf("camp's",  7));
        own2.emplace_back(make_leaf("zebra",   7));

        std::vector<TreeNode*> raw2;
        for (auto& up : own2) raw2.push_back(up.get());
        PriorityQueue pq2(std::move(raw2));

        std::ostringstream oss;
        pq2.print(oss); // should emit in current (desc) order

        const std::string out = oss.str();
        // Spot-check first and last lines for exact spacing:
        // First line must be "        42 wolf\n" (10-wide right-justified 42, then ONE space, then word)
        const std::string first = out.substr(0, out.find('\n') + 1);
        assert(first == "        42 wolf\n");

        // Last printed line should correspond to the lexicographically largest among 7s ("zebra")
        std::istringstream iss(out);
        std::string line, last_line;
        while (std::getline(iss, line)) last_line = line;
        assert(last_line == "         7 zebra");
    }

    std::cout << "PriorityQueue tests passed.\n";
    return 0;
}






/*
#include "Scanner.hpp"
#include "BST.h"
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    fs::path in = (argc >= 2) ? fs::path(argv[1]) : fs::path("input_output/TheBells.txt");

    // 1) Tokenize
    std::vector<std::string> tokens;
    Scanner sc{in};
    auto err = sc.tokenize(tokens);
    if (err != NO_ERROR) { std::cerr << "Scanner error " << err << "\n"; return 1; }

    // 2) Ground-truth via unordered_map
    std::unordered_map<std::string, size_t> truth;
    truth.reserve(tokens.size());
    for (const auto& w : tokens) ++truth[w];

    // 3) Build BST via insert()
    BST bst;
    for (const auto& w : tokens) bst.insert(w);

    // 4) Compare distinct sizes
    if (bst.size() != truth.size()) {
        std::cerr << "Distinct-size mismatch: BST=" << bst.size()
                  << " truth=" << truth.size() << "\n";
        return 2;
    }

    // 5) Compare every count (print first 20 mismatches)
    size_t mismatches = 0;
    for (const auto& [word, cnt] : truth) {
        auto c = bst.countOf(word);
        if (!c || *c != cnt) {
            if (++mismatches <= 20)
                std::cerr << "Mismatch: '" << word << "' bst=" << (c ? *c : 0)
                          << " truth=" << cnt << "\n";
        }
    }
    if (mismatches) { std::cerr << "Total mismatches: " << mismatches << "\n"; return 3; }

    // 6) Optional: ensure inorder is lexicographic
    std::vector<std::pair<std::string,size_t>> inorder;
    inorder.reserve(truth.size());
    bst.inorderCollect(inorder); // NOTE: caller’s responsibility to start from empty
    bool sorted = std::is_sorted(inorder.begin(), inorder.end(),
        [](auto& a, auto& b){ return a.first < b.first; });
    if (!sorted) { std::cerr << "Inorder not sorted by word asc\n"; return 4; }

    std::cout << "Scanner -> BST counts match exactly. Inorder sorted. PASS.\n";
    return 0;
}

*/

/*
// main_insert_from_file.cpp
#include "Scanner.hpp"
#include "BST.h"
#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    // 1) Pick input path (argv[1] or default to input_output/TheBells.txt)
    fs::path in = (argc >= 2) ? fs::path(argv[1]) : fs::path("input_output/TheBells.txt");

    // 2) Tokenize using the in-memory Scanner API (no files written)
    std::vector<std::string> tokens;
    Scanner sc{in};
    auto err = sc.tokenize(tokens);
    if (err != NO_ERROR) {
        std::cerr << "Scanner failed with error code " << err << " for: " << in << "\n";
        return 1;
    }
    std::cout << "Total tokens from scanner: " << tokens.size() << "\n";

    // 3) Build ground-truth counts (what insert should produce)
    std::unordered_map<std::string, size_t> truth;
    truth.reserve(tokens.size());
    for (const auto& w : tokens) ++truth[w];

    // 4) Insert into BST
    BST bst;
    for (const auto& w : tokens) bst.insert(w);

    // 5) Basic invariants
    assert(bst.size() == truth.size() && "BST distinct size must equal ground-truth distinct size");

    // 6) Verify insert increments counts (contains + countOf) for every word
    size_t checked = 0;
    for (const auto& [word, cnt] : truth) {
        assert(bst.contains(word) && "BST must contain every token seen by scanner");
        auto c = bst.countOf(word);
        assert(c && *c == cnt && "BST countOf(word) must equal ground-truth count");
        ++checked;
    }
    std::cout << "Verified " << checked << " distinct words via contains()/countOf().\n";

    // 7) Spot-check: sample a few high-frequency words (optional, just for human eyeballs)
    std::vector<std::pair<std::string,size_t>> top(truth.begin(), truth.end());
    std::sort(top.begin(), top.end(),
              [](auto& a, auto& b) {
                  if (a.second != b.second) return a.second > b.second; // freq desc
                  return a.first < b.first;                              // word asc
              });
    std::cout << "Top 10 words (truth):\n";
    for (size_t i = 0; i < std::min<size_t>(10, top.size()); ++i) {
        std::cout << "  " << top[i].first << " : " << top[i].second << "\n";
    }

    std::cout << "BST insert() tests passed on file: " << in << "\n";
    return 0;
}
*/

/*
 *
 *
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
}*/
