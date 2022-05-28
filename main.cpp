#include <iostream>
#include <ncurses.h>
#include <string>
#include <locale.h>
#include <iomanip>
#include "hacking_game.cpp"

#define WIDTH 32
#define HEIGHT 10
#define TRIES 4
#define WORD_LENGTH 5
#define DICT_FILE "dict.txt"
#define WORD_COUNT 6
#define PUZZLE_OFFSET_Y 2
#define PUZZLE_OFFSET_X 8
#define MEM_DIGITS 4

// Prints string section, moving cursor to next line on each newline encounter.
// Returns any chars left over in case string does end in newline. Function is 
// needed in order for puzzle to be printed according to PUZZLE_OFFSET on each 
// line. (Regular printw starts each line at x=0.)
std::string PrintGridSection(std::string section) {
    int temp_y;
    int temp_x;
    std::string char_line = "";
    for (int i = 0; i < section.length(); ++i) {
        auto next_char = section[i];
        char_line += next_char;

        if (next_char == '\n') {
            printw(char_line.c_str());
            getyx(stdscr, temp_y, temp_x);
            move(temp_y, PUZZLE_OFFSET_X);
            char_line = "";
        }
    }
    return char_line;
}

// Outputs ViewContent in a formatted manner. Displays memory address column.
void DisplayView(ViewContent contents) {
    int y;
    int x;
    getyx(stdscr, y, x);
    move(0, 0);
    
    clear();

    // Attempts left status
    std::string attempts_msg = "Attempt(s) Left:";
    for (int i = 0; i < contents.attempts_left; ++i) {
        attempts_msg += " ■";
    }
    printw(attempts_msg.c_str());
    
    // Print memory address column on left.
    size_t addr = (size_t)&contents.char_grid_sections;
    for (int i = 0; i < HEIGHT; ++i) {
        move(i + PUZZLE_OFFSET_Y, 0);

        std::stringstream stream;
        stream << std::hex << addr + (i * (WIDTH + 1));
        std::string addr_str(stream.str());
        
        addr_str = addr_str.substr(addr_str.length() - MEM_DIGITS);
        std::transform(addr_str.begin(), addr_str.end(), addr_str.begin(), toupper);

        printw(("0x" + addr_str).c_str());
    }

    move(PUZZLE_OFFSET_Y, PUZZLE_OFFSET_X);

    // Print first char_grid_section, saving anything left over to left_over.
    // If more than one grid_section in ViewContent, second section is assumed
    // to be word and is formatted accordingly.
    std::string left_over = PrintGridSection(contents.char_grid_sections[0]);
    if (contents.char_grid_sections.size() > 1) {
        printw(left_over.c_str());

        attron(COLOR_PAIR(1));
        printw(contents.char_grid_sections[1].c_str());
        attroff(COLOR_PAIR(1));

        PrintGridSection(contents.char_grid_sections[2]);
    }
    
    // Print status column starting from bottom of the puzzle
    for (int i = 0; i < contents.status_col.size(); ++i) {
        move((PUZZLE_OFFSET_Y + HEIGHT) - i - 1, PUZZLE_OFFSET_X + WIDTH + 2);
        printw(contents.status_col[contents.status_col.size() - i - 1].c_str());
    }

    move(y, x);
    refresh();
}

int main() { 
    // Necessary for unicode
    setlocale(LC_ALL, "");

    // Start ncurses window
    initscr();

    // Print string at cursor
    printw("Hello post-apocalypse\n");

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    Config config = {HEIGHT, 
                     WIDTH, 
                     TRIES, 
                     WORD_LENGTH,
                     WORD_COUNT,
                     DICT_FILE};

    // Sample content
    auto puzzle = Puzzle(config);

    // offset needed because puzzle does not start at 0, 0
    auto offset = Position(PUZZLE_OFFSET_Y, PUZZLE_OFFSET_X);
    auto cursor = Position(PUZZLE_OFFSET_Y, PUZZLE_OFFSET_X);

    // Move cursor to start
    move(cursor.y_, cursor.x_);

    // Display puzzle initial contents
    ViewContent contents = puzzle.View(cursor, offset);
    DisplayView(contents);
    move(cursor.y_, cursor.x_);

    // Update changes to window
    refresh();

    // Don't echo user input
    noecho();
    cbreak();

    // Loop, updating screen based on input until exit key is pressed
    while (true) {
        int ch = getch();

        // Match input and update cursor
        switch (ch) {
        case 'k':
            cursor.y_ = std::max(PUZZLE_OFFSET_Y, cursor.y_ - 1);
            break;
        case 'j':
            cursor.y_ = std::min(HEIGHT + PUZZLE_OFFSET_Y - 1, cursor.y_+ 1);
            break;
        case 'h':
            cursor.x_ = std::max(PUZZLE_OFFSET_X, cursor.x_ - 1);
            break;
        case 'l':
            cursor.x_ = std::min(WIDTH + PUZZLE_OFFSET_X - 1, cursor.x_ + 1);
            break;
        case '\n':
            if (puzzle.Update(cursor, offset)) {
                contents = puzzle.View(cursor, offset);
                DisplayView(contents);
                getch();
            } else {
                break;
            }
        case 'q':
            endwin();
            return 0;
        }
        move(cursor.y_, cursor.x_);
        contents = puzzle.View(cursor, offset);
        DisplayView(contents);
    }

    return 0;
}