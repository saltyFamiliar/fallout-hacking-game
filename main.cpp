#include <iostream>
#include <ncurses.h>
#include "hacking_game.h"
#include <string>

void PrintFormatted(ViewContent contents) {
    if (contents.char_grid.size() == 1) {
        printw(contents.char_grid[0].c_str());
    } else {
        printw(contents.char_grid[0].c_str());
        attron(COLOR_PAIR(1));
        printw(contents.char_grid[1].c_str());
        attroff(COLOR_PAIR(1));
        printw(contents.char_grid[2].c_str());
    }
    printw(contents.status.c_str());
}

#define WIDTH 20
#define HEIGHT 10
#define TRIES 4

int main() { 
    // Start ncurses window
    initscr();

    // Print string at cursor
    printw("Hello post-apocalypse\n");

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    Config config = {HEIGHT, WIDTH, 4};

    // Sample content
    auto puzzle = Puzzle(config);

    auto cursor = Position(0, 0);

    // Move cursor to start
    move(cursor.y_, cursor.x_);

    // Display puzzle initial contents
    ViewContent contents = puzzle.View(cursor);
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
            cursor.y_ = std::max(0, cursor.y_ - 1);
            break;
        case 'j':
            cursor.y_ = std::min(HEIGHT - 1, cursor.y_+ 1);
            break;
        case 'h':
            cursor.x_ = std::max(0, cursor.x_ - 1);
            break;
        case 'l':
            cursor.x_ = std::min(WIDTH - 1, cursor.x_ + 1);
            break;
        case 'e':
            puzzle.Update(cursor);
            break;
        case 'q':
            goto exit_loop;
        }
        clear();
        
        contents = puzzle.View(cursor);
        move(0, 0);
        PrintFormatted(contents);
        move(cursor.y_, cursor.x_);
        refresh();
    }
    exit_loop: ;

    // End ncurses
    endwin();

    return 0;
}