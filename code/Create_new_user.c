#include <curses.h>
#include <string.h>
#define MAX_INPUT_LENGTH 80

int main() {
    initscr();           // Initialize ncurses
    cbreak();            // Enable cbreak mode
    noecho();             // Don't echo keyboard input
    curs_set(0);          // Hide cursor
    keypad(stdscr, TRUE); // Enable special keys

    box(stdscr, 0, 0);     // Draw border around window
    wmove(stdscr, 0, 0);     // Move cursor to top-left
    addstr("Enter your text here:"); // Add prompt
    refresh();

    char input[MAX_INPUT_LENGTH];
    int cursor_position = 0;
    bool editing_mode = false;

	int ch ;

    while((ch = getch()) != KEY_F(1)) { // Exit on F1 key press
        switch(ch) {
            case KEY_UP:
                if (cursor_position > 0)
                    cursor_position--;
                break;
            case KEY_DOWN:
                if (cursor_position < strlen(input))
                    cursor_position++;
                break;
            case '\n':
                editing_mode = false; // Exit edit mode
                break;
            default:
                if (!editing_mode) {
                    input[cursor_position] = ch;
                    wmove(stdscr, cursor_position + 1, cursor_position + 1);
                    addstr("_");
                    refresh();
                    editing_mode = true;
                }
        }
    }

    if (editing_mode) {
        wmove(stdscr, cursor_position + 1, cursor_position + 1);
        addstr(input);
    } else {
        addstr(" ");
    }
    refresh();
    endwin();

    return 0;
}