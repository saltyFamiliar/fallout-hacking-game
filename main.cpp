#include <iostream>
#include <ncurses.h>
#include "hacking_game.h"
#include <string>

int main() 
{ 
    // Start ncurses window
    initscr();

    // Print string at cursor
    printw("Hello post-apocalypse\n");

    // Sample content
    printw(generate_content(16, 8).c_str());

    // Update changes to window
    refresh();

    // Don't echo user input
    noecho();
    cbreak();

    // Loop, updating screen based on input until exit key is pressed
    while (true) {
        // Cursor position
        int y, x;
        getyx(stdscr, y, x);

        int ch = getch();
        switch (ch)
        {
        case 'k':
            move(y - 1, x);
            break;
        case 'j':
            move(y + 1, x);
            break;
        case 'h':
            move(y, x - 1);
            break;
        case 'l':
            move(y, x + 1);
            break;
        case 'q':
            goto exit_loop;
        }
        
        refresh();
    }
    exit_loop: ;

    // End ncurses
    endwin();

    return 0;
}