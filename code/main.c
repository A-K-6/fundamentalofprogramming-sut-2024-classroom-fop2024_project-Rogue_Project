#include <ncurses.h>
#include <menu.h>
#include <form.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>



int create_start_menu(); 
int new_user();


int main(){
        
    initscr(); // initial courses 

    cbreak(); /**/

    noecho(); /* enables no echo mode. */
    curs_set(0); /*off the curser to 0 to not show the curser*/
    keypad(stdscr, TRUE); /* enabels arrow keys */


    /* define height and width*/
    int height = LINES - 2;
    int width = COLS - 2;

    int option = 1000 ; 
    do
    {
        option = create_start_menu();

    } while (option != 5);
        

    endwin(); // ends the program. 
    return 0; 
}


int create_start_menu(){
    char *choices[] = {
        "New User", 
        "LogIn", 
        "Create Game", 
        "Load Game", 
        "Setting",
        "Exit",
        NULL
    }; 




    ITEM **items;
    int n_choices, i;

    n_choices = sizeof(choices) / sizeof(choices[0]) - 1;
    items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

    for(i = 0; i < n_choices; ++i) {
        items[i] = new_item(choices[i], NULL);
    }
    items[n_choices] = (ITEM *)NULL; 


    MENU *starting_menu;
    starting_menu = new_menu((ITEM **)items);

    set_menu_mark(starting_menu, " -- ");
    
    start_color(); /* Set collor for my menu. */
    init_pair(1, COLOR_RED, COLOR_BLACK);
    set_menu_fore(starting_menu, COLOR_PAIR(1) | A_REVERSE);
    set_menu_back(starting_menu, A_NORMAL);

    

    mvprintw(LINES -2, 0 , "Use arrow key to go up and down through the menu" );
    post_menu(starting_menu);
    refresh();


    int command; 



    while ((command = getch()) != '\n')
    {
        switch (command)
        {
        case KEY_DOWN:
            menu_driver(starting_menu, REQ_DOWN_ITEM); 
            break;
        case KEY_UP:
            menu_driver(starting_menu, REQ_UP_ITEM);
            break;
        }
    }

    ITEM *cur_item = current_item(starting_menu);
    int index = item_index(cur_item);
    unpost_menu(starting_menu); 
    free_menu(starting_menu);
    for (int i = 0; i < n_choices; i++)
    {
        free_item(items[i]);
    }    

    return index;
}