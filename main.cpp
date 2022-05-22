#include <iostream>
#include <ncurses.h>
#include "hacking_game.h"
#include <string>

void PrintFormatted(std::vector<std::string> contents) {
    if (contents.size() == 1) {
        printw(contents[0].c_str());
    } else {
        printw(contents[0].c_str());
        attron(COLOR_PAIR(1));
        printw(contents[1].c_str());
        attroff(COLOR_PAIR(1));
        printw(contents[2].c_str());
    }
}

int main() { 
    // Start ncurses window
    initscr();

    // Print string at cursor
    printw("Hello post-apocalypse\n");

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    // Sample content
    auto puzzle = Puzzle();

    CursorPos cursor = {0, 0};

    // Move cursor to start
    move(cursor.y_pos, cursor.x_pos);

    // Display puzzle initial contents
    std::vector<std::string> contents = puzzle.View(cursor);
    PrintFormatted(contents);
    move(cursor.y_pos, cursor.x_pos);


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
            cursor.y_pos = std::max(0, cursor.y_pos - 1);
            break;
        case 'j':
            cursor.y_pos = std::min(7, cursor.y_pos + 1);
            break;
        case 'h':
            cursor.x_pos = std::max(0, cursor.x_pos - 1);
            break;
        case 'l':
            cursor.x_pos = std::min(6, cursor.x_pos + 1);
            break;
        case 'q':
            goto exit_loop;
        }
        clear();
        
        contents = puzzle.View(cursor);
        move(0, 0);
        PrintFormatted(contents);
        move(cursor.y_pos, cursor.x_pos);
        refresh();
    }
    exit_loop: ;

    // End ncurses
    endwin();

    return 0;
}