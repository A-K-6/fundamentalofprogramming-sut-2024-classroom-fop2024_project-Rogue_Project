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


int main_menu(); 
void new_user();
void login();
void game_menu();
void new_game();
void continue_game();
void leader_board();
void settings();



WINDOW *create_menu_window(int height, int width);
void cleanup(MENU *menu, ITEM **items, int n_items);

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);  // Hide cursor


    int option = 0; 



    while (option != 7)
    {
        option = main_menu();
        switch (option)
        {
            case 0: new_user(); break;
            case 1: login(); break;
            case 2: game_menu(); break;
            case 3: new_game(); break;
            case 4: continue_game(); break;
            case 5: leader_board(); break;
            case 6: settings(); break;
            case 7: 
                exit(-1);
        }
    
    }
    return 0;
}

/* Create centered menu window with border */
WINDOW *create_menu_window(int height, int width) {
    WINDOW *win = newwin(height, width, (LINES-height)/2, (COLS-width)/2);
    keypad(win, TRUE);
    box(win, 0, 0);
    return win;
}

/* Cleanup resources */
void cleanup(MENU *menu, ITEM **items, int n_items) {
    unpost_menu(menu);
    free_menu(menu);
    for(int i = 0; i < n_items; i++) {
        free_item(items[i]);
    }
    free(items);
}


void new_user() {
    char email[FIELD_LEN + 1] = {0};
    char username[FIELD_LEN + 1] = {0};
    char password[FIELD_LEN + 1] = {0};
    int current_field = 0; // 0-email, 1-username, 2-password, 3-create button
    int ch;
    bool done = false;

    // Enable special keys
    keypad(stdscr, TRUE);

    while (!done) {
        clear();
        mvprintw(0, 2, "User Registration Form (Press TAB to navigate)");
        
        // Print labels
        mvprintw(2, 2, "Email: ");
        mvprintw(3, 2, "Username: ");
        mvprintw(4, 2, "Password: ");
        
        // Print input fields
        mvprintw(2, 10, "%s", email);
        mvprintw(3, 10, "%s", username);
        
        // Print password as asterisks
        char obscured[FIELD_LEN + 1];
        memset(obscured, '*', strlen(password));
        obscured[strlen(password)] = '\0';
        mvprintw(4, 10, "%s", obscured);
        
        // Print create button
        mvprintw(6, 2, "[ Create User ]");

        // Highlight current field/button
        switch(current_field) {
            case 0: mvchgat(2, 10, strlen(email), A_REVERSE, 0, NULL); break;
            case 1: mvchgat(3, 10, strlen(username), A_REVERSE, 0, NULL); break;
            case 2: mvchgat(4, 10, strlen(password), A_REVERSE, 0, NULL); break;
            case 3: mvchgat(6, 2, 14, A_REVERSE, 0, NULL); break;
        }

        refresh();
        ch = getch();

        switch(ch) {
            case KEY_UP:
                current_field = (current_field > 0) ? current_field - 1 : 3;
                break;
            case KEY_DOWN:
            case 9: // TAB key
                current_field = (current_field + 1) % 4;
                break;
            case KEY_BACKSPACE:
            case 127:
            case 8:
                // Handle backspace
                switch(current_field) {
                    case 0: if (strlen(email) > 0) email[strlen(email)-1] = '\0'; break;
                    case 1: if (strlen(username) > 0) username[strlen(username)-1] = '\0'; break;
                    case 2: if (strlen(password) > 0) password[strlen(password)-1] = '\0'; break;
                }
                break;
            case 10: // Enter key
                if (current_field == 3) {
                    // Validate fields
                    if (strlen(email) == 0 || strlen(username) == 0 || strlen(password) == 0) {
                        mvprintw(8, 2, "Error: All fields are required!");
                        refresh();
                        getch();
                    } else {
                        // Check if username exists
                        FILE *file = fopen("users.txt", "r");
                        bool exists = false;
                        
                        if (file != NULL) {
                            char line[256];
                            while (fgets(line, sizeof(line), file)) {
                                char existing_user[FIELD_LEN + 1];
                                if (sscanf(line, "%49[^:]", existing_user) == 1) {
                                    if (strcmp(existing_user, username) == 0) {
                                        exists = true;
                                        break;
                                    }
                                }
                            }
                            fclose(file);
                        }

                        if (exists) {
                            mvprintw(8, 2, "Error: Username already exists!");
                            refresh();
                            getch();
                        } else {
                            // Save user data
                            file = fopen("users.txt", "a");
                            if (file == NULL) {
                                mvprintw(8, 2, "Error: Cannot save user!");
                            } else {
                                fprintf(file, "%s:%s:%s\n", username, email, password);
                                fclose(file);
                                mvprintw(8, 2, "User created successfully!");
                                done = true;
                            }
                            refresh();
                            getch();
                        }
                    }
                } else {
                    current_field = (current_field + 1) % 4;
                }
                break;
            default:
                if (current_field < 3 && isprint(ch)) {
                    char *target;
                    int max_len;
                    switch(current_field) {
                        case 0: target = email; max_len = FIELD_LEN; break;
                        case 1: target = username; max_len = FIELD_LEN; break;
                        case 2: target = password; max_len = FIELD_LEN; break;
                    }
                    if (strlen(target) < max_len) {
                        target[strlen(target)] = ch;
                        target[strlen(target)] = '\0';
                    }
                }
                break;
        }
    }

    // Wait for key press before returning to main menu
    getch();    
}

int main_menu() {
    

    int height = LINES - 2;
    int width = COLS - 2;
    WINDOW *main_win = newwin(height, width, 1, 1);
    mvprintw(LINES - 3, 3 , "hi");    
    box(main_win, 0, 0);
    wrefresh(main_win);

    // Initialize menu items
    int n_items = ARRAY_SIZE(Items);
    ITEM **items = (ITEM **)calloc(n_items + 1, sizeof(ITEM *));
    
    for(int i = 0; i < n_items; i++) {
        items[i] = new_item(Items[i], "");
    }
    items[n_items] = NULL;

    // Create menu
    MENU *menu = new_menu((ITEM **)items);
    int menu_width = 40;
    int menu_height = 12;
    WINDOW *menu_win = create_menu_window(menu_height, menu_width);
    
    set_menu_win(menu, menu_win);
    set_menu_sub(menu, derwin(menu_win, menu_height - 2, menu_width - 2, 1, 1));
    set_menu_mark(menu, "> ");
    post_menu(menu);
    
    // Add title
    mvwprintw(menu_win, 0, 2, " Main Menu ");
    wrefresh(menu_win);

    // Footer message
    mvprintw(LINES - 1, 2, "Use arrow keys to navigate, Enter to select, F1 to exit");
    refresh();

    int c;
    while((c = getch()) != KEY_F(1)) {
        switch(c) {
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                break;
            case KEY_LEFT:
                menu_driver(menu, REQ_LEFT_ITEM);
                break;
            case KEY_RIGHT:
                menu_driver(menu, REQ_RIGHT_ITEM);
                break;
            // case 10: 
            //     break; // Enter key
            case KEY_ENTER:
                ITEM *cur = current_item(menu);
                int index = item_index(cur);
                
                // Clear previous messages
                move(LINES - 2, 0);
                clrtoeol();
                
                // Handle menu selection
                switch(index) {
                    case 0: new_user(); break;
                    case 1: login(); break;
                    case 2: game_menu(); break;
                    case 3: new_game(); break;
                    case 4: continue_game(); break;
                    case 5: leader_board(); break;
                    case 6: settings(); break;
                    case 7: 
                        cleanup(menu, items, n_items);
                        delwin(main_win);
                        endwin();
                        return 7; 
                        break;  // Exit
                }
                break;
        }
    }
        refresh();
        wrefresh(menu_win);

}


void login() { /* ... */ }
void game_menu() { /* ... */ }
void new_game() { /* ... */ }
void continue_game() { /* ... */ }
void leader_board() { /* ... */ }
void settings() { /* ... */ }