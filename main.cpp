#include <iostream>
#include <ncurses.h>
#include "hacking_game.h"
#include <string>
#include <locale.h>

#define WIDTH 20
#define HEIGHT 10
#define TRIES 4
#define WORD_LENGTH 5
#define DICT_FILE "dict.txt"
#define WORD_COUNT 6
#define PUZZLE_OFFSET_Y 2
#define PUZZLE_OFFSET_X 0

void PrintFormatted(ViewContent contents) {
    int y;
    int x;
    getyx(stdscr, y, x);
    move(0, 0);
    
    clear();

    std::string attempts_msg = "Attempt(s) Left: ";
    for (int i = 0; i < contents.attempts_left; ++i) {
        attempts_msg += "■";
    }

    printw(attempts_msg.c_str());
    move(PUZZLE_OFFSET_Y, PUZZLE_OFFSET_X);

    // char_grid is given as either one whole section or 3. If 3, middle needs
    // to be highlighted to indicate cursor is at start of a word
    if (contents.char_grid_sections.size() == 1) {
        printw(contents.char_grid_sections[0].c_str());
    } else {
        printw(contents.char_grid_sections[0].c_str());
        attron(COLOR_PAIR(1));
        printw(contents.char_grid_sections[1].c_str());
        attroff(COLOR_PAIR(1));
        printw(contents.char_grid_sections[2].c_str());
    }
    printw("\n");
    
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
    auto cursor = Position(PUZZLE_OFFSET_Y, PUZZLE_OFFSET_Y);

    // Move cursor to start
    move(cursor.y_, cursor.x_);

    // Display puzzle initial contents
    ViewContent contents = puzzle.View(cursor, offset);
    PrintFormatted(contents);
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
                PrintFormatted(contents);
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
        PrintFormatted(contents);
    }


    return 0;
}