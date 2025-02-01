#include <ncurses.h>
#include <menu.h>
#include <form.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>
#include <stdio.h>



char *USER = "Guest";



int pregame_menu();
int get_user_info(const char *username, char *email, char *password) ;
int is_valid_email(const char *email) ;
int create_start_menu(); 
void create_new_user();
char *trim_whitespace(char *str);
int is_unique_username(const char *username);
int is_valid_password(const char *password); 
void save_user(const char *email, const char *username, const char *password);
void generate_random_password(FIELD *password_field);
void log_in();
int check_password(char *username, char *password_input);
void forget_passwrod(const char *user_name, FIELD *password_field);

int split(const char *str, char delimiter, char **tokens, int max_tokens) ;
int main(){
    /* define height and width*/
    int height = LINES - 2;
    int width = COLS - 2;

    int option = 1000 ; 
    
    do
    {
        option = create_start_menu();

        if (option == 0){
            mvprintw(0, 0 , "new_user");
            create_new_user();
        }
        else if(option == 1){
            log_in();
            
        }
        else if(option == 2){
            int pregame_option = pregame_menu();
            while (pregame_option != 4)
            {
                


                switch (pregame_option)
                {
                case 0:
                    /* new_game */
                    break;
                
                case 1:
                    /* Resume Game*/
                    break;

                case 2:
                    /* scorboard*/
                    break;
                case 3:
                    /* setting */
                    break;
                case 4:
                    /* exit*/
                    break;

                }
            }
            
        }
        else if(option == 3){
            printf("hi");
        }
        else if (option == 4)
        {
            printf("hi");
        }
        else{
            exit(1);
        }

    } while (option != 4);
    
    
    

    return 0; 
}


int create_start_menu(){
    char *choices[] = {
        "New User", 
        "LogIn", 
        "Pre-Game Menu", 
        // "New Game", 
        // "Resume Game", 
        // "Scoreboard"
        "Setting",
        "Exit",
        NULL
    }; 

    initscr(); // initial courses 

    cbreak(); /**/

    noecho(); /* enables no echo mode. */
    curs_set(0); /*off the curser to 0 to not show the curser*/
    keypad(stdscr, TRUE); /* enabels arrow keys */



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

    
    mvprintw(LINES -3 ,0 ,  USER); 

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
    endwin(); // ends the program. 
    return index;
}

int pregame_menu(){
    char *choices[] = {
        "New Game", 
        "Resume Game", 
        "Scoreboard",
        "Setting",
        "Exit",
        NULL
    }; 

    initscr(); // initial courses 

    cbreak(); /**/

    noecho(); /* enables no echo mode. */
    curs_set(0); /*off the curser to 0 to not show the curser*/
    keypad(stdscr, TRUE); /* enabels arrow keys */



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

    
    mvprintw(LINES -3 ,0 ,  USER); 

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
    endwin(); // ends the program. 
    return index;



}


void form_highlight(FORM *form, bool highlight) {
    FIELD *cur_field = current_field(form);
    if (highlight) {
        set_field_back(cur_field, A_STANDOUT);
    } else {
        set_field_back(cur_field, A_NORMAL | A_UNDERLINE);
    }
    wrefresh(form_win(form));
}

void create_new_user(){
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input
    keypad(stdscr, TRUE); // Enable arrow keys and function keys

    FIELD *field[6]; // 3 input fields + 2 buttons + 1 NULL terminator

    field[0] = new_field(1, 30, 4, 20, 0, 0); // Email
    field[1] = new_field(1, 20, 6, 20, 0, 0); // Username
    field[2] = new_field(1, 20, 8, 20, 0, 0); // Password

    field[3] = new_field(1, 20, 10, 20, 0, 0); // Submit button
    field[4] = new_field(1, 30, 10, 42, 0, 0); // Generate Password button
    
    field[5] = NULL; // Terminator

    for (int i = 0; i < 3; i++) {
        set_field_back(field[i], A_UNDERLINE); // Show underline
        field_opts_off(field[i], O_AUTOSKIP);  // Don't jump to next field automatically
    }
    // Mask the password input
    // field_opts_off(field[2], O_PUBLIC);
    // Button fields
    for (int i = 3; i < 5; i++) {
        set_field_back(field[i], A_REVERSE); // Highlight buttons
        field_opts_off(field[i], O_EDIT);    // Make buttons non-editable
        field_opts_off(field[i], O_ACTIVE);  // Make buttons selectable
    }

    set_field_buffer(field[3], 0, " Submit(Enter)");
    set_field_buffer(field[4], 0, " Generate Password(F2)");

    FORM *my_form = new_form(field);
    post_form(my_form);
    refresh();

    // print lables next to the fields. 
    mvprintw(4, 10, "Email:");
    mvprintw(6, 10, "Username:");
    mvprintw(8, 10, "Password:");


    mvprintw(12, 10, "Use TAB to navigate. Press ENTER to select.");
    int ch;
    form_highlight(my_form, true); 
    while((ch = getch()) != KEY_F(1)) { // Use F1 to exit if needed
        switch(ch) {
            case KEY_DOWN:
            case 9: // TAB key
                form_highlight(my_form, false);
                form_driver(my_form, REQ_NEXT_FIELD);
                form_driver(my_form, REQ_END_LINE);
                form_highlight(my_form, true);
                break;
            case KEY_UP:
            case KEY_BTAB:
                form_highlight(my_form, false);
                form_driver(my_form, REQ_PREV_FIELD);
                form_driver(my_form, REQ_END_LINE);
                form_highlight(my_form, true);

                break;
            case KEY_ENTER:
            case 10: // Enter key
                {
                    char *email_input    = trim_whitespace(field_buffer(field[0], 0));
                    char *username_input = trim_whitespace(field_buffer(field[1], 0));
                    char *password_input = trim_whitespace(field_buffer(field[2], 0));
                    if (!is_valid_email(email_input)) {
                        mvprintw(14, 10, "Invalid email format.");
                    } else if (!is_unique_username(username_input)) {
                        mvprintw(14, 10, "Username already taken.");
                    } else if (!is_valid_password(password_input)) {
                        mvprintw(14, 10, "Password doesn't meet criteria.");
                    } else {
                        mvprintw(14, 10, "Registration successful!");
                        save_user(email_input, username_input, password_input);
                        
                        // Clean 
                        unpost_form(my_form);
                        free_form(my_form);
                        for (int i = 0; i < 3; i++) {
                            free_field(field[i]);
                        }

                        endwin();

                        return ; 
                    }
                }
                break;
            case KEY_BACKSPACE:
            case 127:
            case '\b':
                form_driver(my_form, REQ_DEL_PREV);
                break;
            case KEY_F(2):
                generate_random_password(field[2]);
                break;
            default:
                form_driver(my_form, ch);
                break;
        }
    }


    unpost_form(my_form);
    free_form(my_form);
    for (int i = 0; i < 3; i++) {
        free_field(field[i]);
    }
    endwin();
    return ; 
}

void log_in(){
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input
    keypad(stdscr, TRUE); // Enable arrow keys and function keys

    FIELD *field[5]; 



    field[0] = new_field(1, 30, 4, 20, 0, 0); // Username    
    field[1] = new_field(1, 30, 6, 20, 0, 0); // Password

    field[2] = new_field(1, 20, 10, 20, 0, 0); // Submit button
    field[3] = new_field(1, 30, 10, 42, 0, 0); 

    field[4] = NULL; // Terminator

    for (int i = 0; i < 2; i++) {
        set_field_back(field[i], A_UNDERLINE); // Show underline
        field_opts_off(field[i], O_AUTOSKIP);  // Don't jump to next field automatically
    }

    for (int i = 2; i < 4; i++) {
        set_field_back(field[i], A_REVERSE); // Highlight buttons
        field_opts_off(field[i], O_EDIT);    // Make buttons non-editable
        field_opts_off(field[i], O_ACTIVE);  // Make buttons selectable
    }

    set_field_buffer(field[2], 0, " Submit(Enter)");
    set_field_buffer(field[3], 0, " FORGET_PASSWORD(F2)");
    
    FORM *my_form = new_form(field);
    post_form(my_form);
    refresh();

    mvprintw(4, 10, "Username:");
    mvprintw(6, 10, "Password:");
    mvprintw(12, 10, "Use TAB to navigate. Press ENTER to select.");

    int ch;
    form_highlight(my_form, true); 
    while((ch = getch()) != KEY_F(1)) { // Use F1 to exit if needed
        mvprintw(14, 10, "");

        switch(ch) {
            case KEY_DOWN:
            case 9: // TAB key
                form_highlight(my_form, false);
                form_driver(my_form, REQ_NEXT_FIELD);
                form_driver(my_form, REQ_END_LINE);
                form_highlight(my_form, true);
                break;
            case KEY_UP:
            case KEY_BTAB:
                form_highlight(my_form, false);
                form_driver(my_form, REQ_PREV_FIELD);
                form_driver(my_form, REQ_END_LINE);
                form_highlight(my_form, true);

                break;
            case KEY_ENTER:
            case 10: // Enter key
                {
                    char *username_input = trim_whitespace(field_buffer(field[0], 0));
                    char *password_input = trim_whitespace(field_buffer(field[1], 0));
                    if (is_unique_username(username_input)) {
                        move(14,0); clrtoeol(); 
                        mvprintw(14, 10, "Username does not exist");
                        refresh(); 
                    // if (!is_unique_username(username_input) == 1) {
                    //     mvprintw(10,10, "searching through the password");
                    //     mvprintw(14, 10, password);
                    }
                    else
                    {
                        char *password = malloc(100*sizeof(char));
                        char *email= malloc(100*sizeof(char));
                        get_user_info(username_input,email, password);
                        if(strcmp(password_input, password) == 0){
                            USER = malloc(100*sizeof(char));

                            strcpy(USER, username_input);

                            move(14,0); clrtoeol(); 

                            mvprintw(14,10, "loged in :)");

                            refresh();


                            unpost_form(my_form);
                            free_form(my_form);
                            for (int i = 0; i < 3; i++) {
                                free_field(field[i]);
                            }
                            endwin();
                            return ; 

                        }
                        else{
                            move(14,0); clrtoeol(); 
                            mvprintw(14,10, "wrong password");
                            refresh(); 

                        }
                    }
                }

                break;
            case KEY_BACKSPACE:
            case 127:
            case '\b':
                form_driver(my_form, REQ_DEL_PREV);
                break;
            case KEY_F(2):
                    char *username_input = trim_whitespace(field_buffer(field[0], 0));
                    char *password_input = trim_whitespace(field_buffer(field[1], 0));
                    if (is_unique_username(username_input)) {
                        move(14,0); clrtoeol(); 
                        mvprintw(14, 10, "Username does not exist");
                        refresh(); 
                    }
                    else
                    {
                        forget_passwrod(username_input, field[1]);
                    }
                break;
            default:
                form_driver(my_form, ch);
                break;
        }
    }


    unpost_form(my_form);
    free_form(my_form);
    for (int i = 0; i < 3; i++) {
        free_field(field[i]);
    }
    endwin();
    return ; 
    
}

int is_valid_email(const char *email) {
    return strstr(email, "@") && strstr(email, ".");
}

void save_user(const char *email, const char *username, const char *password) {
    FILE *file = fopen("users.txt", "a");
    fprintf(file, "%s|%s|%s\n", username, password,  email);
    fclose(file);
}


char *trim_whitespace(char *str) {
    char *end;

    while (*str == ' ' || *str == '\t') str++;

    if (*str == '\0') {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) end--;

    *(end + 1) = '\0';

    return str;
}


void generate_random_password(FIELD *password_field) {
    char password[20];
    const char *upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *lower = "abcdefghijklmnopqrstuvwxyz";
    const char *digits = "0123456789";
    int index = 0;

    // Ensure at least one of each required character type
    password[index++] = upper[rand() % strlen(upper)];
    password[index++] = lower[rand() % strlen(lower)];
    password[index++] = digits[rand() % strlen(digits)];
    // Fill the rest randomly
    for (; index < 7; index++) {
        int r = rand() % 3;
        if (r == 0)
            password[index] = upper[rand() % strlen(upper)];
        else if (r == 1)
            password[index] = lower[rand() % strlen(lower)];
        else
            password[index] = digits[rand() % strlen(digits)];
    }
    
    password[index] = '\0';
    set_field_buffer(password_field, 0 , password);
    
}


int is_unique_username(const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (!file) return 1; // If file doesn't exist, username is unique
    char line[256]; // Buffer to hold each line
    char input_trimmed[256];
    strncpy(input_trimmed, username, sizeof(input_trimmed));
    trim_whitespace(input_trimmed);

    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';
        // Prepare to store tokens
        char *tokens[3]; // username, email, password
        int num_tokens = split(line, '|', tokens, 3);

        if (num_tokens >= 1) {
            // Trim whitespace from the username
            char *stored_username = trim_whitespace(tokens[0]);

            // Compare with the provided username
            if (strcmp(input_trimmed, stored_username) == 0) {
                // Free allocated memory
                
                for (int i = 0; i < num_tokens; i++) {
                    free(tokens[i]);
                }
                fclose(file);
                return 0; // Username exists
            }
        }

        // Free allocated memory for tokens
        for (int i = 0; i < num_tokens; i++) {
            free(tokens[i]);
        }
    }

    fclose(file);
    return 1; // Username is unique
}


int is_valid_password(const char *password) {
    int length = strlen(password);
    int has_upper = 0, has_lower = 0, has_digit = 0;
    if (length < 7) return 0;

    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
    }
    return has_upper && has_lower && has_digit;
}



int get_user_info(const char *username, char *email, char *password) {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        return 0; // Failure
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove any trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Split the line into tokens
        char *tokens[3]; // Assuming username|email|password
        int num_tokens = split(line, '|', tokens, 3);

        if (num_tokens == 3) {
            char *stored_username = tokens[0];
            char *stored_email = tokens[2];
            char *stored_password = tokens[1];

            // Compare usernames
            if (strcmp(username, stored_username) == 0) {
                // Copy email and password to output variables
                strcpy(email, stored_email);
                strcpy(password, stored_password);

                // Free allocated memory
                for (int i = 0; i < num_tokens; i++) {
                    free(tokens[i]);
                }
                fclose(file);
                return 1; // Success
            }
        }

        // Free allocated memory
        for (int i = 0; i < num_tokens; i++) {
            free(tokens[i]);
        }
    }

    fclose(file);
    return 0; // User not found
}

void forget_passwrod(const char *user_name, FIELD *password_field){
    char *password = malloc(100*sizeof(char));
    char *email= malloc(100*sizeof(char));
    get_user_info(user_name,email, password);

    set_field_buffer(password_field, 0 , password);


}


int check_password(char *username, char *password_input)
{

}


// Split function
int split(const char *str, char delimiter, char **tokens, int max_tokens) {
    int token_count = 0;
    const char *start = str;
    const char *ptr = str;

    while (*ptr != '\0' && token_count < max_tokens - 1) { // Leave space for the last token
        if (*ptr == delimiter) {
            int length = ptr - start;
            tokens[token_count] = (char*)malloc(length + 1);
            strncpy(tokens[token_count], start, length);
            tokens[token_count][length] = '\0';

            // Move to the next token
            token_count++;
            start = ptr + 1;
        }
        ptr++;
    }

    // Add the last token
    if (token_count < max_tokens) {
        int length = ptr - start;
        tokens[token_count] = (char*)malloc(length + 1);
        strncpy(tokens[token_count], start, length);
        tokens[token_count][length] = '\0';
        token_count++;
    }

    return token_count;
}
