#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <ncurses.h>

class Position {
    public:
        int y_;
        int x_;
        int linear_pos_;

        Position(int y, int x) {
            y_ = y;
            x_ = x;
        }

        // LinearPos represents position in grid as if it were a one
        // dimensional array. Grid width is needed to calculate this.
        int LinearPos(int width) {
            return (y_ * (width + 1)) + x_;
        }
};

struct Word {
    std::string chars;
    Position pos = Position(0, 0);
};

struct Config {
    int height;
    int width;
    int tries;
};

struct ViewContent {
    std::vector<std::string> char_grid;
    std::string status;
};

// Checks if given vector contains value
template <typename T>
bool Contains(std::vector<T> vec, T val) {
    for (auto x : vec) {
        if (x == val) return true;
    }
    return false;
};

class Puzzle {
    public:
        // Initializes object's char_grid_ with random chars and Words.
        Puzzle(Config config) {
            grid_height_ = config.height;
            grid_width_ = config.width;
            tries_left_ = config.tries;
            tries_ = config.tries;

            SetStatus();

            word_map_ = CreateWordMap();
            char_grid_ = GenerateContent();
        }

        // Returns a string vec to be handled and printed by ncurses. If cursor
        // is not on a word, returns whole char_grid_ as single element in vec,
        // else returns chars before cursor, chars in word cursor is on, and 
        // chars after word cursor is on as three elements.
        ViewContent View(Position cursor) {
            // If cursor is not on a Word, just return char_grid as vector
            if (!word_map_.count(cursor.y_)) {
                return {std::vector<std::string>{char_grid_}, status_};
            }

            if (cursor.x_ != word_map_[cursor.y_].pos.x_) {
                return {std::vector<std::string>{char_grid_}, status_};
            }

            // If cursor is on a Word, return:
            // [chars before Word, Word, chars after Word]
            std::vector<std::string> result_vector;
            Word selected_word = word_map_[cursor.y_];

            std::string before_word = 
            char_grid_.substr(0, selected_word.pos.LinearPos(grid_width_));
            result_vector.push_back(before_word);

            result_vector.push_back(selected_word.chars);
            
            std::string after_word =
            char_grid_.substr(selected_word.pos.LinearPos(grid_width_) + 4);
            result_vector.push_back(after_word);

            return {result_vector, status_};
        }

        // Replaces word with dots if when selected and displays latest status
        void Update(Position cursor) {
            if (word_map_.count(cursor.y_)) {
                if (word_map_[cursor.y_].pos.x_ == cursor.x_) {
                    word_map_[cursor.y_].chars == "....";
                }
            }
            tries_left_ -= 1;
            SetStatus();
        }

    private:
        std::string char_grid_;
        std::string status_;
        int tries_left_;
        int tries_;
        std::vector<std::string> words_in_puzzle_;
        const int max_word_count_ = 4;
        int grid_height_;
        int grid_width_;
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
            for (int i = 0; i < max_word_count_; ++i) {
                std::string new_word;
                do {
                    new_word = wordList[rand() % wordList.size()];
                }
                while (Contains(words_in_puzzle_, new_word));
                words_in_puzzle_.push_back(new_word);
                
                int word_line_pos;
                do {
                    word_line_pos = rand() % grid_height_;
                }
                while (Contains(lines_used, word_line_pos));
                lines_used.push_back(word_line_pos);
        
                new_word_map[word_line_pos] = Word {
                    chars: new_word,
                    pos: Position(word_line_pos, rand() % (grid_width_ - 4))
                };
            }
            return new_word_map;
        }

        // Returns an grid_height_ x grid_width_ string of random chars and Words
        std::string GenerateContent() {
            std::string content;
            std::string line;
            bool word_on_line;
    
            // Generate content one line at a time to be used as char_grid_
            for (int i = 0; i < grid_height_; ++i) {
                line = "";
                word_on_line = 
                (word_map_.find(i) != word_map_.end());

                // Fill line with random chars and maybe word
                while (line.length() < grid_width_) {
                    if (word_on_line && word_map_[i].pos.x_ == line.length()) {
                        line += word_map_[i].chars;
                    } else {
                        // rand range corresponds to char ascii values
                        // do while to exclude \ and %
                        char c;
                        do {
                            c = rand() % 94 + 33;
                        } while(c == 37 || c == 92);

                        line += c;
                    }
                }
                content += line + '\n';
            }
            return content;
        }

        // Sets status_ according to tries left
        void SetStatus() {
            status_ = "Tries left: " +
                std::to_string(tries_left_) + "/" + std::to_string(tries_);
        }
};
