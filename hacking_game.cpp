#include <string>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <ncurses.h>
#include <fstream>
#include <bits/stdc++.h>

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

enum class UpdateResult {
    kOk,
    kNoWord,
    kLoss,
    kWin
};

struct Word {
    std::string chars;
    Position pos = Position(0, 0);
};

struct Config {
    int height;
    int width;
    int tries;
    int word_length;
    int word_count;
    std::string dict_file;
};

struct ViewContent {
    std::vector<std::string> char_grid_sections;
    std::vector<std::string> status_col;
    int attempts_left;
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
        // Initializes Puzzle according to Config given and 
        // default status_col message. Calls CreateWordMap, GenerateContent,
        // and GenerateDictionary
        Puzzle(Config config) {
            grid_height_ = config.height;
            grid_width_ = config.width;
            tries_left_ = config.tries;
            tries_ = config.tries;
            word_length_ = config.word_length;
            max_word_count_ = config.word_count;

            word_list_ = GenerateWordList(config.dict_file);
            word_map_ = CreateWordMap();
            char_grid_ = GenerateContent();
        }

        // Returns ViewContent containing to be handled by ncurses. If cursor
        // is not on a word, returns whole char_grid_ as single element in vec,
        // else returns chars before cursor, chars in word cursor is on, and 
        // chars after word cursor is on as three elements.
        ViewContent View(Position cursor, Position offset)  {
            cursor = Position(cursor.y_ - offset.y_, cursor.x_ - offset.x_);

            // If cursor is not on a Word, just return char_grid_sections as vector
            if (!word_map_.count(cursor.y_)) {
                return {std::vector<std::string>{char_grid_},
                       status_col_,
                       tries_left_};
            }

            if (cursor.x_ != word_map_[cursor.y_].pos.x_) {
                return {std::vector<std::string>{char_grid_}, 
                        status_col_, 
                        tries_left_};
            }

            // If cursor is on a Word, char_grid_sections are:
            // [chars before Word, Word, chars after Word]
            std::vector<std::string> result_vector;
            Word selected_word = word_map_[cursor.y_];

            std::string before_word = 
            char_grid_.substr(0, selected_word.pos.LinearPos(grid_width_));
            result_vector.push_back(before_word);

            result_vector.push_back(selected_word.chars);
            
            std::string after_word =
            char_grid_.substr(
                selected_word.pos.LinearPos(grid_width_) + word_length_
                );
            result_vector.push_back(after_word);

            return {result_vector, 
                    status_col_, 
                    tries_left_};
        }

        // Called when user selects something. Changes properties of
        // Puzzle if necessary and passes an UpdateResult to Setstatus_col.
        bool Update(Position cursor, Position offset) {
            cursor = Position(cursor.y_ - offset.y_, cursor.x_ - offset.x_);
            if (--tries_left_ == 0) return SetStatus(UpdateResult::kLoss);

            if (word_map_.count(cursor.y_) && 
            word_map_[cursor.y_].pos.x_ == cursor.x_) {
                Word selected = word_map_[cursor.y_];
                last_selected_ = selected.chars;
                last_num_chars_matching_ = Authenticate(last_selected_);

                if (last_num_chars_matching_ == password_.length())
                    return SetStatus(UpdateResult::kWin);
                
                std::string before_word = 
                char_grid_.substr(0, selected.pos.LinearPos(grid_width_));
                std::string after_word =
                char_grid_.substr(
                    selected.pos.LinearPos(grid_width_) + word_length_);

                // Replaces word after it is selected
                std::string fill = "";
                for(int i = 0; i < word_length_; ++i) {
                    fill += ".";
                }

                char_grid_ = before_word + fill + after_word;

                word_map_.erase(cursor.y_);

                return SetStatus(UpdateResult::kOk);
            } else {
                return SetStatus(UpdateResult::kNoWord);
            }
        }

    private:
        std::string char_grid_;
        std::vector<std::string> status_col_;
        int tries_left_;
        int tries_;
        int max_word_count_ = 4;
        int grid_height_;
        int grid_width_;
        std::string password_;
        std::map<int, Word> word_map_;
        std::string last_selected_;
        int last_num_chars_matching_;
        int word_length_;
        std::vector<std::string> word_list_;


        // Generates list of 100 random words of appropriate length from 
        // dictionary file.
        std::vector<std::string> GenerateWordList(std::string dict_filename) {
            std::ifstream dict_file(dict_filename);
            std::string line;
            std::vector<int> line_nums;
            std::vector<std::string> word_list;

            // Populate list of locations of words of right length
            for(int lc = 0; getline(dict_file, line); ++lc) {
                if (line.length() == word_length_) {
                    line_nums.push_back(lc);
                }
            }
            
            // Clear ifstream's EOF and fail flags in order to reuse it and 
            // seek to start of file
            dict_file.clear();
            dict_file.seekg(0);

            srand(time(NULL));

            // List of line numbers or words that will be in Puzzle
            std::vector<int> word_list_lines;

            // Get max_word_count_ number of random line indexes from
            // line_nums.
            for (int i = 0; i < max_word_count_; ++i) {
                int ln;
                do {
                    ln = line_nums[rand() % line_nums.size()];
                }while(Contains(word_list_lines, ln));
                word_list_lines.push_back(ln);
            }

            std::sort(word_list_lines.begin(), word_list_lines.end());

            // Iterate through lines in dict_file. When line number found
            // in word_list_lines is encountered. Add line to word_list and
            // remove first element from word_list_lines (since it is ordered)
            // line numbers will be encountered in order.
            for (int ln = 0; word_list_lines.size() > 0; ++ln) {
                getline(dict_file, line);
                if (ln == word_list_lines[0]) {
                    word_list.push_back(line);
                    word_list_lines.erase(word_list_lines.begin());
                }
            }
            return word_list;
        }

        // Returns map of line indexes to word structs
        std::map<int, Word> CreateWordMap() {
            std::map<int, Word> new_word_map;
            std::vector<std::string> words_in_puzzle_;
            
            // Used to keep track of which lines have been assigned a Word
            std::vector<int> lines_used;

            srand(time(NULL));

            // Adds max_word_count_ unique Words to new_word_map
            for (int i = 0; i < max_word_count_; ++i) {
                std::string new_word;
                do {
                    new_word = word_list_[rand() % word_list_.size()];
                }
                while (Contains(words_in_puzzle_, new_word));
                words_in_puzzle_.push_back(new_word);

                // Select last random word to be answer
                password_ = new_word;
                
                int word_line_pos;
                do {
                    word_line_pos = rand() % grid_height_;
                }
                while (Contains(lines_used, word_line_pos));
                lines_used.push_back(word_line_pos);
        
                new_word_map[word_line_pos] = Word {
                    chars: new_word,
                    pos: Position(
                        word_line_pos, rand() % (grid_width_ - word_length_)
                        )
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

        // Sets status_col_ according to tries left. Returns true if game ending
        // event occurred, else false.
        bool SetStatus(UpdateResult result) {
            switch (result) {
                case UpdateResult::kOk:
                    status_col_.push_back(last_selected_);
                    status_col_.push_back(
                        std::to_string(last_num_chars_matching_) + 
                        "/" + 
                        std::to_string(password_.length()) + 
                        " correct.");
                    return false;
                case UpdateResult::kNoWord:
                    status_col_.push_back("Invalid password!");
                    return false;
                case UpdateResult::kWin:
                    status_col_.push_back("Access granted.");
                    return true;
                case UpdateResult::kLoss:
                    status_col_.push_back("Password was: " + password_);
                    status_col_.push_back("Access denied");
                    return true;
                default:
                    status_col_.push_back("How'd you get here?");
                    return true;
            }
        }

        // Returns the amount of letters (in the right place) in common with
        // Puzzle's password.
        int Authenticate(std::string selected) {
            int matching_letters = 0;
            for (int i = 0; i < password_.length(); ++i)
                matching_letters += (password_[i] == selected[i]);
            
            return matching_letters;
        }
};
