/*

usage:     make && ./Words $fileWithTwoWords $fileWithDictToConnectThem [$optionalArgToRunTheUnitTest]
e.g.:      make && ./Words words_5.txt dict_5.txt

We find the shortest path between two words using a dictionary to connect between them. Words in the dictionary (represented by N vertices in a graph)
are considered neighbours if they are of the same length, and they differ by exactly one letter. We call the two corresponding vertices 'first' and 'final'
and search for the path between them. The algorithm (basically, a simplified Dijkstra, running with the speed of O(N^2)) is:
1. We compare all pairs of vertices in the graph to find neighbours.
2. Assign score=0 to the first vertex and score=infinity to others.
3. Find neighbours of the vertex with the smallest score and assign them score+1.
4. Repeat point 3 until we have either traversed all the vertices or encountered the final vertex.

*/

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <sstream>
#include <iostream>
#include <vector>

class Dict { // contains the graph plus meta-data
    std::vector<std::string> words; // words in the dictionary
    const std::string wordBegin, wordEnd; // the first and the last words that need to be transformed
    size_t indFirst = UINT_MAX, indLast = UINT_MAX; // indices of the first and final word

public:
    std::vector<std::string> seq; // the sequence we are looking for

    static bool areNeighbours(const std::string& s1, const std::string& s2) { // iff the two words differ only by one letter
        auto mismatch = 0u;
        auto it_s1 = s1.begin(), it_s2 = s2.begin();
        for (; it_s1 != s1.end(); ++it_s1, ++it_s2)
            if (*it_s1 != *it_s2 && ++mismatch > 1)
                return false;
        return mismatch != 0;
    }

    Dict(const std::string& wordBegin, const std::string& wordEnd)
        : wordBegin(wordBegin)
        , wordEnd(wordEnd) {}

    ~Dict() {}

    void push(const std::string& word) { // add a vertex (word) to the graph
        if (word.size() != wordBegin.size()) // ignoring words of different size in the dictionary
            return;
        auto ind = words.size();
        words.push_back(word);
        if (words.back() == wordBegin)
            indFirst = ind;
        if (words.back() == wordEnd)
            indLast = ind;
    }

    int connect(bool screen) { // resize the arrays and find connections between vertices
        if (indFirst == UINT_MAX || indLast == UINT_MAX) {
            std::cout << "-- No connection (the dictionary is missing the first or the last word)" << std::endl;
            return 2;
        }
        std::vector<size_t> score(words.size(), UINT_MAX); // will contain distances from the first vertex
        score[indFirst] = 0;

        // finding which words are connected
        std::vector<bool> connections(words.size() * words.size()); // connections[i + j * wordBegin.size()] == true for iff the words #i and #j in the dictionary are connected (exactly one letter differs)
        for (auto i = 0u; i < words.size(); ++i) {
            connections[i * (1 + words.size())] = false; // not connecting the word with itself
            for (auto j = 0u; j < i; ++j)
                connections[i + j * words.size()] = connections[j + i * words.size()] = areNeighbours(words[i], words[j]);
        }

        std::vector<size_t> previousVertex(words.size(), UINT_MAX); // index of the neighbour with lower score
        std::vector<bool> usedVertex(words.size(), false);
        size_t indVertex; // index of the vertex whose neighbours we are currently checking
        while (true) {
            // finding a new vertex with the lowest score
            auto currentScore = UINT_MAX; // score of the current word
            for (auto i = 0u; i < words.size(); ++i) {
                if (usedVertex[i] || score[i] >= currentScore) // skip vertices that we have checked already and those with higher score than we found already
                    continue;
                currentScore = score[i];
                indVertex = i;
                usedVertex[i] = true;
            }
            if (currentScore == UINT_MAX)
                break;

            // connecting and assigning scores to neighbours of the vertex
            ++currentScore;
            bool scoreAssigned = false; // iff this iteration assigns at least one new score
            for (auto j = 0u; j < words.size(); ++j) {
                if (score[j] > currentScore && connections[indVertex + j * words.size()]) {
                    score[j] = currentScore;
                    previousVertex[j] = indVertex;
                    if (j == indLast) // reached the last word
                        break;
                }
            }
        }

        if (previousVertex[indLast] == UINT_MAX) {
            std::cout << "-- No connection (no path found)" << std::endl;
            return 3;
        }

        auto ind = indLast;
        while (true) {
            if (screen)
                std::cout << words[ind] << '\n';
            seq.push_back(words[ind]);
            if (ind == indFirst)
                break;
            ind = previousVertex[ind];
        }

        return 0;
    }
};

void readFiles(const std::string& fileWordsName, const std::string& fileDictName, std::vector<std::string>& seq, bool screen = true) {
    std::string wordBegin, wordEnd;

    {
        std::ifstream fileWords(fileWordsName);
        if (!fileWords.is_open()) {
            std::cerr << "-- ERROR: cannot read the words file" << std::endl;
            ::exit(1);
        }
        getline(fileWords, wordEnd); size_t found = wordEnd.find_last_not_of("\r\t "); if (found != std::string::npos) wordEnd.erase(found + 1); else wordEnd.clear(); // std::cout << wordEnd << "  " << wordEnd.size() << std::endl;
        getline(fileWords, wordBegin); found = wordBegin.find_last_not_of("\r\t "); if (found != std::string::npos) wordBegin.erase(found + 1); else wordBegin.clear(); // std::cout << wordBegin << "  " << wordBegin.size() << std::endl;
        fileWords.close();
    }

    if (wordBegin.size() != wordEnd.size()) {
        if (screen)
            std::cout << "-- No connections (words are of different length)" << std::endl;
        return;
    }

    Dict dict(wordBegin, wordEnd);

    {
        std::string wordDict;
        std::ifstream fileDict(fileDictName);
        if (!fileDict.is_open()) {
            std::cerr << "-- ERROR: cannot read the dictionary file" << std::endl;
            ::exit(-1);
        }
        while (getline(fileDict, wordDict)) {
            size_t found = wordDict.find_last_not_of("\r\t "); if (found != std::string::npos) wordDict.erase(found + 1); else wordDict.clear();
            dict.push(wordDict);
        }
        fileDict.close();
    }
    dict.connect(screen);
    seq = dict.seq;
}

static int test_case = 0;
void check(const std::string& fileWords, const std::string& fileDict, const std::vector<std::string>& ans) {
    ++test_case;
    std::vector<std::string> seq;
    readFiles(fileWords, fileDict, seq, false);
    if (seq != ans) {
        std::cerr << "-- ERROR: test case " << test_case << " failed\n";
        ::exit(-2);
    }
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cout << "-- ERROR: must have at least 2 command line arguments: fileWords, fileDict, [3rd arg to run the unit tests]" << std::endl;
        return 1;
    }

    std::vector<std::string> seq;

    std::string fileWordsName = argv[1], fileDictName = argv[2];

    if (argc > 3) { // UNIT TESTS
        if (!Dict::areNeighbours("ABC", "ABB") || !Dict::areNeighbours("ABCD", "AZCD") || Dict::areNeighbours("AAA", "AAA") || Dict::areNeighbours("AAA", "AAAB") || Dict::areNeighbours("ACAA", "AAAB")) {
            std::cerr << "-- ERROR: test case for neighbours failed\n";
            ::exit(-2);
        }
        check("words_1.txt", "dict_1.txt", { "XYZ", "XYX", "ZYX", "ZYY", "ZXY" });
        check("words_2.txt", "dict_2.txt", { "KOT", "TOT", "TON" });
        check("words_3.txt", "dict_3.txt", {});
        check("words_4.txt", "dict_4.txt", { "ABA", "BBA", "BBB", "BAB" });
        check("words_5.txt", "dict_5.txt", { "ABA", "ABB", "ACB", "ACC", "CCC" });
    }

    readFiles(fileWordsName, fileDictName, seq);
}
