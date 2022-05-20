#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>

std::vector<std::string> wordList = 
{
    "rads",
    "nuke",
    "caps",
    "chem",
    "jazz"
};

// Defines number of words in puzzle
const int WORD_CNT = 4;

// Checks if given vector contains value
template <typename T>
bool contains(std::vector<T> vec, T val)
{
    for (auto x: vec)
    {
        if (x == val)
        {
            return true;
        }
    }
    return false;
}

// Returns an x * y string of random characters with words scattered throughout
std::string generate_content(int x, int y)
{
    std::string content;

    // Character buffer that will be used to fill content x chars at a time
    std::string line;
    
    // Seed rand generator
    srand(time(NULL));

    // List of line indexes of words
    std::vector<int> wordIdxs;

    // List of words in puzzle
    std::vector<std::string> wordsInPuzzle;

    // Maps line indexes to words
    std::map<int, std::string> wordMap;

    // Fill wordMap
    for (int i = 0; i < WORD_CNT; ++i)
    {
        std::string newWord;

        // Loop needed to ensure every word is unique
        do
        {
            newWord = wordList[rand() % wordList.size()];
        }
        while (contains(wordsInPuzzle, newWord));

        wordsInPuzzle.push_back(newWord);

        int newIdx;

        // Loop needed to ensure unique random values in start_idxs
        do 
        {
            newIdx = rand() % y;
        }
        while (contains(wordIdxs, newIdx));

        wordIdxs.push_back(newIdx);

        wordMap[newIdx] = newWord;
    }

    // Generate content one line at a time
    // Insert words at their line indexes at random x indexes
    for (int i = 0; i < y; ++i)
    {
        // Position of word in line
        int wordXpos;
        line = "";

        // Set wordXpos to random position in line if there should be word on line i
        if (contains(wordIdxs, i)) 
        {
            // Position of word should be len(word) spaces before end of line
            // To prevent overflow
            wordXpos = rand() % (x - 4);
        }

        // Fill line with random chars and maybe word
        for (char c; line.length() < x;)
        {
            if (line.length() == wordXpos && contains(wordIdxs, i))
            {
                line += wordMap[i];
            } else
            {
            // range of c corresponds to ascii values representing chars
            c = rand() % 94 + 33;
            line += c;
            }
        }
        content += line + '\n';
    }
    return content;
}