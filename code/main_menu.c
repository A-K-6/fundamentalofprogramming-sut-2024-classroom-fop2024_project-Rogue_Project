#include <stdlib.h>
#include <curses.h>
#include <menu.h>
#include <string.h>
#include <ctype.h>
#define FIELD_LEN 49

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))



char *Items[] = {
    "1. New User",
    "2. Login",
    "3. Before the Game Menu",
    "4. New Game",
    "5. Continue Last Game",
    "6. Leader Board",
    "7. Settings",
    "Exit"
};






int main(){

    
    initscr(); 
    cbreak();
    noecho();
    keypad(stdscr, true); 
    curs_set(0); 

    // Tool , Arz. 
    int height = LINES - 2 ; int width = COLS -2; 
    
    WINDOW *main_win = newwin(height, width, 1, 1); 


}