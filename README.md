# STUDENT INFORMATION
- NAME: Caleb Clements
- STUDENT ID: 008878539
- COURSE: CS315 - Data Structures
- REPOSITORY LINK: https://github.com/calebwclem/Huffman-Tree


# COLLABORATION & SOURCES
- I completed this assignment individually. 
- No code was shared with or received from other students.
- Primary sources: the Project 3 spec and starter utilities provided by the instructor; course lecture notes; C++ reference for filesystem, vector, string, algorithm, iomanip, and optional.
- Any third-party materials were used only for API clarification (e.g., cppreference for standard library functions). No external libraries were used.
- ChatGPT was used for discusssion on algorithms, some bug fixing, and some of this README.


# IMPLEMENTATION DETAILS

### SCANNER
- Reads the word from the file and returns it as a string.
- Tokenize the input string
- Tokens are either a single character or a string of characters.
- Tokens are stored in a list.

### utils

Small helpers shared across modules.

- Error codes: enum error_type { NO_ERROR, FILE_NOT_FOUND, DIR_NOT_FOUND, UNABLE_TO_OPEN_FILE, ERR_TYPE_NOT_FOUND, UNABLE_TO_OPEN_FILE_FOR_WRITING, FAILED_TO_WRITE_FILE };

- Conventions:

    - All file outputs end with a newline.

    - On failure, print a clear message to stderr and exit non-zero (driver handles this).

### PriorityQueue

Vector-backed, non-owning priority queue used to simulate a min-heap behavior for Huffman builds and to emit .freq.

- Invariant / comparator

    - Sort by count descending, tie-break by key_word ascending (see HuffmanTree below).

    - Therefore the minimum element is always at the back (items_.back()).

- API

    - explicit PriorityQueue(std::vector<TreeNode*> nodes); (sorts internally and keeps raw pointers non-owningly)

    - size(), empty(), findMin(), extractMin(), deleteMin(), insert(TreeNode*)

    - print(std::ostream&) const → writes .freq using std::setw(10) << count << ' ' << word << '\n'

- Output (.freq)

    - <base>.freq: lines formatted exactly as: right-justified 10-wide count, a single space, then the word; sorted by (count desc, tie word/key asc).

### BST
Binary Search Tree that counts word frequencies from tokens.

- Node: struct TreeNode { std::string word; std::size_t count; TreeNode* left; TreeNode* right; };

- Core behavior

    - insert(const std::string& w): if the word exists, ++count; else create new node with count=1. Lexicographic compare on word.

    - bulkInsert(const std::vector<std::string>& tokens).

    - inorderCollect(std::vector<std::pair<std::string,std::size_t>>& out) const:
produces (word,count) pairs sorted by word ascending (in-order traversal), independent of insertion order.

- Queries/metrics

    - contains, countOf, size() (= distinct words), height() (empty tree = 0).

- Complexity

    - Expected O(T log V) with randomized insertion; worst-case skew O(T·V) is acceptable for chapter-length inputs.

### HuffmanTree
Builds the Huffman tree from (word,count) pairs, assigns codes, writes the header, and encodes tokens.

- TreeNode extension for Part 3

    - Added std::string key_word for deterministic tie-breaking:

        - Leaf: key_word = word

        - Internal: key_word = min(left.key_word, right.key_word) (lexicographic)

- Ownership

    - HuffmanTree owns the entire tree; destructor recursively deletes nodes.

    - Class is non-copyable and movable to avoid double-delete.

- Building

    - static HuffmanTree buildFromCounts(const std::vector<std::pair<std::string,std::size_t>>& counts);

    - Create one leaf per (word,count).

    - Initialize PriorityQueue (non-owning, min at back).

    - Repeatedly extractMin() twice → first becomes left (0), second right (1); create parent node (count = sum, key_word = min(left.key_word, right.key_word)), and re-insert parent.

    - Root is the final remaining node. Edge cases: 0 symbols → empty tree; 1 symbol → single node (code = "0").

- Code assignment / header / encoding

    - void buildCodebook(std::unordered_map<std::string,std::string>& out) const;

      - DFS from root; left edge appends '0', right appends '1'; leaves record (word, code).

      - Single-symbol edge case: code is "0".

    - error_type writeHeader(std::ostream& os) const;

      - Pre-order over leaves only; each line: word<space>code, final newline; do not sort.

    - error_type encode(const std::vector<std::string>& tokens, std::ostream& os_bits, int wrap_cols=80) const;

      - Replaces each token with its code; outputs ASCII '0'/'1'.

      - Wrap exactly at 80 columns (except possibly last line) and end with a newline.

      - If any token lacks a code, return an error.

    - unsigned height() const noexcept; (empty = 0).

- Outputs

    - <base>.hdr — header mapping (pre-order over leaves): word code

    - <base>.code — encoded bitstream (0/1 chars), 80-column wrapped, final newline


# TESTING & STATUS
Everything is working as expected and complies with the overall requirements of the assignment.

## TO BUILD

```bash
g++ -std=c++20 -Wall *.cpp -o huffman_part3
```

## TO RUN
```bash
./huffman_part3 TheBells.txt
```

