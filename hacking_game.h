#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <ncurses.h>

#define GRID_WIDTH 8
#define GRID_HEIGHT 8

struct Word {
    std::string chars;
    int y_pos;
    int x_pos;
};

struct CursorPos {
    int y_pos;
    int x_pos;
};

// Checks if given vector contains value
template <typename T>
bool Contains(std::vector<T> vec, T val) {
    for (auto x : vec) {
        if (x == val) return true;
    }
    return false;
}

class Puzzle {
    public:
        // Initializes object's char_grid_ with random chars and Words.
        Puzzle() {
            this->word_map_ = CreateWordMap();
            this->char_grid_ = GenerateContent();
        }

        // Returns a string vec to be handled and printed by ncurses. If cursor
        // is not on a word, returns whole char_grid_ as single element in vec,
        // else returns chars before cursor, chars in word cursor is on, and 
        // chars after word cursor is on as three elements.
        std::vector<std::string> View(CursorPos cursor) {
            // If cursor is not on a Word, just return char_grid as vector
            if (!this->word_map_.count(cursor.y_pos)) {
                return std::vector<std::string>{this->char_grid_};
            }

            if (cursor.x_pos != this->word_map_[cursor.y_pos].x_pos) {
                return std::vector<std::string>{this->char_grid_};
            }

            // If cursor is on a Word, return:
            // [chars before Word, word, chars after Word]
            std::vector<std::string> result_vector;
            Word selected_word = this->word_map_[cursor.y_pos];

            int word_abs_position = 
            (cursor.y_pos * GRID_WIDTH) + cursor.x_pos;

            std::string before_word = 
            this->char_grid_.substr(0, word_abs_position);
            result_vector.push_back(before_word);

            result_vector.push_back(selected_word.chars);
            
            std::string after_word =
            this -> char_grid_.substr(word_abs_position + 4);
            result_vector.push_back(after_word);

            return result_vector;
        }

        // Replaces word with dots if when selected and displays latest status
        void Update(CursorPos CursorPos) {

        }

    private:
        std::string char_grid_;
        std::string status_;
        int tries_left_;
        std::vector<std::string> words_in_puzzle_;
        const int max_word_count_ = 4;
        // Maps line indexes to words
        std::map<int, Word> word_map_;

        std::vector<std::string> wordList = {
            "rads",
            "nuke",
            "caps",
            "chem",
            "jazz"
        };

        // Returns map of line indexes to word structs
        std::map<int, Word> CreateWordMap() {
            std::map<int, Word> new_word_map;
            
            // Used to keep track of which lines have been assigned a Word
            std::vector<int> lines_used;

            srand(time(NULL));

            // Adds max_word_count_ unique Words to new_word_map
            for (int i = 0; i < this->max_word_count_; ++i) {
                std::string new_word;
                do {
                    new_word = wordList[rand() % wordList.size()];
                }
                while (Contains(this->words_in_puzzle_, new_word));
                this->words_in_puzzle_.push_back(new_word);
                
                int word_line_pos;
                do {
                    word_line_pos = rand() % GRID_HEIGHT;
                }
                while (Contains(lines_used, word_line_pos));
                lines_used.push_back(word_line_pos);
        
                new_word_map[word_line_pos] = Word {
                    chars: new_word, 
                    y_pos: word_line_pos,
                    x_pos: rand() % (GRID_WIDTH - 4)
                };
            }
            return new_word_map;
        }

        // Returns an GRID_HEIGHT x GRID_WIDTH string of random chars and Words
        std::string GenerateContent() {
            std::string content;
            std::string line;
            bool word_on_line;
    
            // Generate content one line at a time to be used as char_grid_
            for (int i = 0; i < GRID_HEIGHT; ++i) {
                line = "";
                word_on_line = 
                (this->word_map_.find(i) != this->word_map_.end());

                // Fill line with random chars and maybe word
                while (line.length() < GRID_WIDTH - 1) {
                    if (word_on_line && this->word_map_[i].x_pos == line.length()) {
                        line += this->word_map_[i].chars;
                    } else {
                        // rand range corresponds to char ascii values
                        //line += rand() % 94 + 33;
                        line += 'Q';
                    }
                }
                content += line + '\n';
            }
            return content;
        }
};


