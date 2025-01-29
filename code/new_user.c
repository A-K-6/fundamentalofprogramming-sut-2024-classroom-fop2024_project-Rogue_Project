#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <menu.h>
#include <form.h>
#include <regex.h>

#define USER_FILE "users.txt"
#define MIN_PASSWORD_LENGTH 7
#define EMAIL_REGEX "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"

// Validation functions
int isUsernameUnique(const char *username);
int isPasswordValid(const char *password);
int isEmailValid(const char *email);

void new_user() {
    FIELD *fields[5];
    FORM *form;
    WINDOW *form_win;
    int ch, rows, cols;
    FILE *fp;
    
    // Initialize form fields
    fields[0] = new_field(1, 30, 2, 15, 0, 0);
    fields[1] = new_field(1, 30, 4, 15, 0, 0);
    fields[2] = new_field(1, 30, 6, 15, 0, 0);
    fields[3] = new_field(1, 15, 8, 20, 0, 0);
    fields[4] = NULL;

    // Set field attributes
    set_field_back(fields[3], A_UNDERLINE);
    field_opts_off(fields[0], O_AUTOSKIP);
    field_opts_off(fields[1], O_AUTOSKIP);
    field_opts_off(fields[2], O_AUTOSKIP);
    
    // Create form
    form = new_form(fields);
    scale_form(form, &rows, &cols);
    
    // Create form window
    form_win = newwin(12, 50, (LINES-12)/2, (COLS-50)/2);
    keypad(form_win, TRUE);
    box(form_win, 0, 0);
    mvwprintw(form_win, 0, 2, " New User Registration ");
    
    // Set form labels
    mvwprintw(form_win, 2, 2, "Username: ");
    mvwprintw(form_win, 4, 2, "Password: ");
    mvwprintw(form_win, 6, 2, "Email:    ");
    mvwprintw(form_win, 8, 15, "[ Submit ]");
    
    set_form_win(form, form_win);
    set_form_sub(form, derwin(form_win, rows, cols, 1, 1));
    post_form(form);
    wrefresh(form_win);
    
    // Password field configuration
    set_field_buffer(fields[1], 0, "");
    field_opts_off(fields[1], O_PUBLIC);  // Hide password input
    
    // Form processing loop
    int valid = 0;
    while((ch = wgetch(form_win)) != KEY_F(1)) {
        switch(ch) {
            case KEY_DOWN:
                form_driver(form, REQ_NEXT_FIELD);
                form_driver(form, REQ_END_LINE);
                break;
            case KEY_UP:
                form_driver(form, REQ_PREV_FIELD);
                form_driver(form, REQ_END_LINE);
                break;
            case 10:  // Enter key
                if(current_field(form) == fields[3]) {
                    // Get field values
                    char *username = field_buffer(fields[0], 0);
                    char *password = field_buffer(fields[1], 0);
                    char *email = field_buffer(fields[2], 0);
                    
                    // Trim whitespace and newlines
                    username[strcspn(username, " \t\n")] = 0;
                    password[strcspn(password, " \t\n")] = 0;
                    email[strcspn(email, " \t\n")] = 0;

                    // Validate username
                    if(strlen(username) == 0) {
                        mvwprintw(form_win, 10, 2, "Username cannot be empty!");
                        wclrtoeol(form_win);
                        break;
                    }
                    
                    if(!isUsernameUnique(username)) {
                        mvwprintw(form_win, 10, 2, "Username already exists!");
                        wclrtoeol(form_win);
                        break;
                    }
                    
                    // Validate password
                    if(!isPasswordValid(password)) {
                        mvwprintw(form_win, 10, 2, "Password must have:");
                        mvwprintw(form_win, 11, 2, "- 7+ chars - 1 uppercase - 1 lowercase - 1 number");
                        wclrtoeol(form_win);
                        break;
                    }
                    
                    // Validate email
                    if(!isEmailValid(email)) {
                        mvwprintw(form_win, 10, 2, "Invalid email format!");
                        wclrtoeol(form_win);
                        break;
                    }
                    
                    // Save to file
                    fp = fopen(USER_FILE, "a");
                    if(fp) {
                        fprintf(fp, "%s:%s:%s\n", username, password, email);
                        fclose(fp);
                        valid = 1;
                    }
                    
                    if(valid) {
                        mvwprintw(form_win, 10, 2, "Registration successful!");
                        wrefresh(form_win);
                        wgetch(form_win);
                        goto cleanup;
                    }
                }
                break;
            default:
                form_driver(form, ch);
                break;
        }
        wrefresh(form_win);
    }

cleanup:
    unpost_form(form);
    free_form(form);
    for(int i=0; fields[i]; i++) free_field(fields[i]);
    delwin(form_win);
    touchwin(stdscr);
    refresh();
}

// Validation implementations
int isUsernameUnique(const char *username) {
    FILE *fp = fopen(USER_FILE, "r");
    if(!fp) return 1;  // File doesn't exist yet
    
    char line[256];
    while(fgets(line, sizeof(line), fp)) {
        char saved_user[50];
        if(sscanf(line, "%49[^:]", saved_user) == 1) {
            if(strcmp(saved_user, username) == 0) {
                fclose(fp);
                return 0;
            }
        }
    }
    fclose(fp);
    return 1;
}

int isPasswordValid(const char *password) {
    int len = strlen(password);
    if(len < MIN_PASSWORD_LENGTH) return 0;
    
    int has_upper = 0, has_lower = 0, has_digit = 0;
    for(int i=0; i<len; i++) {
        if(isupper(password[i])) has_upper = 1;
        if(islower(password[i])) has_lower = 1;
        if(isdigit(password[i])) has_digit = 1;
    }
    return has_upper && has_lower && has_digit;
}

int isEmailValid(const char *email) {
    regex_t regex;
    int ret;
    
    // Compile regular expression
    if(regcomp(&regex, EMAIL_REGEX, REG_EXTENDED)) {
        return 0;  // Failed to compile regex
    }
    
    // Execute regular expression
    ret = regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);
    
    return ret == 0;
}