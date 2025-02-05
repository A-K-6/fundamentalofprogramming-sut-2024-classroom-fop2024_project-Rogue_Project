// 403 171 146 
// Aeen Karimian

#include <ncurses.h>
#include <menu.h>
#include <form.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <time.h>
#include <sqlite3.h>


#define MAP_HEIGHT 25
#define MAP_WIDTH 80
#define MAX_ROOMS 3
#define MAX_ATTEMPTS 20

typedef struct {
    char *username;
    char *password;
    char *email;
} User;


/* Structure to hold a scoreboard entry */
typedef struct {
    char *username;
    int total_point;
    int total_gold;
    int total_played;
    char *first_time;
} ScoreEntry;


typedef struct {
    int x, y; // Top-left corner of the room (within the map)
    int w, h;         // Width and height of the room
    int door_x, door_y; 
} Room;



User main_user = {.username ="Guest"};

int pregame_menu();

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
void display_game(); 
void displayScoreboard(sqlite3 *db, const char *currentUser); 


// for DAta_base
int createDatabase(sqlite3 **db); 
int userExists(sqlite3 *db, const char *username); 
int insertUser(sqlite3 *db, const char *username, const char *password, const char *email); 
int checkUserPassword(sqlite3 *db, const char *username, const char *password) ; 
User *getUserInfo(sqlite3 *db, const char *username);
int createScoreboardTable(sqlite3 *db) ; 
int updateScoreboardFirstTime(sqlite3 *db, const char *username);
sqlite3 *db;

// void init_map(char map[MAP_HEIGHT][MAP_WIDTH]);
// void draw_room(char map[MAP_HEIGHT][MAP_WIDTH], Room *room);
// void draw_corridor(char map[MAP_HEIGHT][MAP_WIDTH], int x1, int y1, int x2, int y2);
// bool rooms_overlap(Room r1, Room r2);
// void generate_map(char map[MAP_HEIGHT][MAP_WIDTH], Room rooms[], int *room_count);
// void display_game(char map[MAP_HEIGHT][MAP_WIDTH], int player_x, int player_y);
// void save_game(const char map[MAP_HEIGHT][MAP_WIDTH], int player_x, int player_y);




int split(const char *str, char delimiter, char **tokens, int max_tokens) ;
int main(){
    /* define height and width*/
    int height = LINES - 2;
    int width = COLS - 2;

    int option = 1000 ; 

    int rc = createDatabase(&db);
    // (Assuming you already have a createDatabase() for your users table.)
    rc = createScoreboardTable(db);

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
                        // continue game. 
                        display_game();        
                        break;            
                case 1:
                    /* Resume Game*/
                    break;

                case 2:
                    /* Initialize colors (if terminal supports them) */
                    if (has_colors()) {
                        start_color();
                        init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Gold for 1st
                        init_pair(2, COLOR_WHITE,  COLOR_BLACK);   // Silver for 2nd
                        init_pair(3, COLOR_RED,    COLOR_BLACK);   // Bronze for 3rd (or choose a custom color)
                        init_pair(4, COLOR_CYAN,   COLOR_BLACK);   // Default color for the rest
                    }
                    displayScoreboard(db, main_user.username);
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
    
    
    

    sqlite3_close(db);
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

    
    mvprintw(LINES -3 ,0 ,  main_user.username); 

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

    
    mvprintw(LINES -3 ,0 ,  main_user.username ); 

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


    // sqlite3 *db;
    // int rc = createDatabase(&db);
/* Function to insert a user into the database */
int insertUser(sqlite3 *db, const char *username, const char *password, const char *email) {
    // Use a parameterized query to avoid SQL injection
    const char *sqlInsert = "INSERT INTO users (username, password, email) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    
    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    
    // Bind the parameters to the statement
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, email, -1, SQLITE_STATIC);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    } else {
        printf("User inserted successfully!\n");
    }

    // Finalize the statement to release resources
    sqlite3_finalize(stmt);
    
    return rc;
}


/*
 * Function: getUserInfo
 * ---------------------
 * Retrieves the user's information (username, password, email) from the "users" table
 * given a username.
 *
 *  db:       An open SQLite database connection.
 *  username: The username to search for.
 *
 *  returns: A pointer to a newly allocated User structure with the user's details if found;
 *           NULL if the user is not found or if an error occurs.
 *
 *  Note: The caller is responsible for freeing the allocated User structure and its members.
 */
User *getUserInfo(sqlite3 *db, const char *username) {
    const char *sql = "SELECT username, password, email FROM users WHERE username = ? LIMIT 1;";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement (getUserInfo): %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    // Bind the username parameter.
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Allocate memory for the User structure.
        User *user = malloc(sizeof(User));
        if (!user) {
            fprintf(stderr, "Memory allocation error\n");
            sqlite3_finalize(stmt);
            return NULL;
        }
        
        // Retrieve column values.
        const unsigned char *uname = sqlite3_column_text(stmt, 0);
        const unsigned char *pass  = sqlite3_column_text(stmt, 1);
        const unsigned char *email = sqlite3_column_text(stmt, 2);
        
        // Duplicate the strings so that the User structure owns its data.
        user->username = uname ? strdup((const char*)uname) : NULL;
        user->password = pass  ? strdup((const char*)pass)  : NULL;
        user->email    = email ? strdup((const char*)email) : NULL;
        
        sqlite3_finalize(stmt);
        return user;
    }
    
    // No matching record found.
    sqlite3_finalize(stmt);
    return NULL;
}

/*
 * Function: createDatabase
 * ------------------------
 * Creates (or opens) the SQLite database and creates the "users" table if it does not exist.
 */
int createDatabase(sqlite3 **db) {
    int rc = sqlite3_open("user_data.db", db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }

    // SQL command to create the users table
    const char *sqlCreateTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "password TEXT NOT NULL, "
        "email TEXT NOT NULL"
        ");";

    char *errMsg = NULL;
    rc = sqlite3_exec(*db, sqlCreateTable, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create table: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(*db);
        return rc;
    }
    return SQLITE_OK;
}


/*
 * Function: userExists
 * --------------------
 * Checks if the given username exists in the "users" table.
 *
 *  db:       An open SQLite database connection.
 *  username: The username to search for.
 *
 *  returns:  1 if the username exists, 0 otherwise.
 */
int userExists(sqlite3 *db, const char *username) {
    const char *sql = "SELECT 1 FROM users WHERE username = ? LIMIT 1;";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement (userExists): %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    // Bind the username parameter (index starts at 1)
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    int exists = (rc == SQLITE_ROW); // SQLITE_ROW means a record was found
    
    sqlite3_finalize(stmt);
    return exists;
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
                    } else if (userExists(db,username_input)) {
                        mvprintw(14, 10, "Username already taken.");
                    } else if (!is_valid_password(password_input)) {
                        mvprintw(14, 10, "Password doesn't meet criteria.");
                    } else {
                        mvprintw(14, 10, "Registration successful!");
                        insertUser(db,username_input, password_input, email_input);
                        
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

/*
 * Function: checkUserPassword
 * ---------------------------
 * Checks if the given username and password combination exists in the "users" table.
 *
 *  db:       An open SQLite database connection.
 *  username: The username to search for.
 *  password: The password to verify.
 *
 *  returns:  1 if the username and password match a record, 0 otherwise.
 */
int checkUserPassword(sqlite3 *db, const char *username, const char *password) {
    const char *sql = "SELECT 1 FROM users WHERE username = ? AND password = ? LIMIT 1;";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement (checkUserPassword): %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    // Bind the username and password parameters
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    int valid = (rc == SQLITE_ROW); // SQLITE_ROW indicates a match was found
    
    sqlite3_finalize(stmt);
    return valid;
}

/* 
 * Create the scoreboard table if it does not exist.
 * The table contains:
 *   - username      (unique, primary key)
 *   - total_point   (total points of the user)
 *   - total_gold    (total stored golds)
 *   - total_played  (the total games played)
 *   - first_time    (the time the user was first created)
 */
int createScoreboardTable(sqlite3 *db) {
    const char *sqlCreateTable =
        "CREATE TABLE IF NOT EXISTS scoreboard ("
        "username TEXT PRIMARY KEY, "
        "total_point INTEGER DEFAULT 0, "
        "total_gold INTEGER DEFAULT 0, "
        "total_played INTEGER DEFAULT 0, "
        "first_time TEXT"
        ");";
    
    char *errMsg = NULL;
    int rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create scoreboard table: %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }
    return SQLITE_OK;
}

/*
 * Function: updateScoreboardFirstTime
 * -----------------------------------
 * Given a username, this function checks the scoreboard table to see if the
 * "first_time" field is empty (or if the record does not yet exist).
 * If it is empty, it sets it to the current time.
 *
 *  db:       An open SQLite database connection.
 *  username: The username to check/update.
 *
 *  returns:  SQLITE_OK if the operation succeeded or no update was needed;
 *            Otherwise, an SQLite error code.
 */
int updateScoreboardFirstTime(sqlite3 *db, const char *username) {
    int rc;
    sqlite3_stmt *stmt = NULL;
    char currentTime[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    // Format current time as: YYYY-MM-DD HH:MM:SS
    strftime(currentTime, sizeof(currentTime), "%Y-%m-%d %H:%M:%S", tm_info);

    // Step 1: Check if the record exists and if "first_time" is empty.
    const char *sqlSelect = "SELECT first_time FROM scoreboard WHERE username = ? LIMIT 1;";
    rc = sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare SELECT statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);

    int updateAction = 0;
    if (rc == SQLITE_ROW) {
        const unsigned char *firstTimeText = sqlite3_column_text(stmt, 0);
        // If the "first_time" column is NULL or an empty string, we need to update it.
        if (!firstTimeText || strlen((const char *)firstTimeText) == 0) {
            updateAction = 1; // Update existing row.
        }
    } else {
        // No record exists for this username.
        updateAction = 2; // Insert a new row.
    }
    sqlite3_finalize(stmt);

    // Step 2: Depending on whether a row exists, update or insert.
    if (updateAction == 1) {
        // Update the existing row.
        const char *sqlUpdate = "UPDATE scoreboard SET first_time = ? WHERE username = ?;";
        rc = sqlite3_prepare_v2(db, sqlUpdate, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare UPDATE statement: %s\n", sqlite3_errmsg(db));
            return rc;
        }
        sqlite3_bind_text(stmt, 1, currentTime, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Failed to update first_time: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
    } else if (updateAction == 2) {
        // Insert a new row with default zeros for totals and current time for first_time.
        const char *sqlInsert =
            "INSERT INTO scoreboard (username, total_point, total_gold, total_played, first_time) "
            "VALUES (?, 0, 0, 0, ?);";
        rc = sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare INSERT statement: %s\n", sqlite3_errmsg(db));
            return rc;
        }
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, currentTime, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Failed to insert row with first_time: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);
        return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
    }
    
    // If updateAction was 0, that means a record exists and first_time is already set.
    return SQLITE_OK;
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
                    if (!userExists(db,username_input)) {
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
                        User *temp_user =   getUserInfo(db, username_input);
                        // get_user_info(username_input,email, password);
                        if(strcmp(password_input, temp_user->password) == 0){

                            main_user.username = malloc(strlen(username_input)* sizeof(char) + 2*sizeof(char)); 
                            strcpy(main_user.username , username_input);
                            updateScoreboardFirstTime(db,main_user.username);
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
                    if (!userExists(db, username_input)) {
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

    srand(time(NULL));


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



void forget_passwrod(const char *user_name, FIELD *password_field){
    char *password = malloc(100*sizeof(char));
    char *email= malloc(100*sizeof(char));
    User *tempuser = getUserInfo(db, user_name); 

    set_field_buffer(password_field, 0 ,tempuser->password);


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



/*
 * Function: displayScoreboard
 * ---------------------------
 * Queries the scoreboard table (sorted by total_point descending)
 * and displays the list in a scrollable ncurses pad. The first three entries
 * are given special color attributes (gold, silver, bronze), and if the
 * current user (currentUser) is found (and is not "Guest"), that row is highlighted.
 * The user may scroll with arrow keys and exit by pressing F1.
 *
 *  db:          Open SQLite database connection.
 *  currentUser: The username of the current user (or "Guest").
 */
void displayScoreboard(sqlite3 *db, const char *currentUser) {
    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT username, total_point, total_gold, total_played, first_time "
                      "FROM scoreboard ORDER BY total_point DESC;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        endwin();
        fprintf(stderr, "Failed to prepare scoreboard query: %s\n", sqlite3_errmsg(db));
        return;
    }

    /* Load all results into a dynamic array */
    ScoreEntry *entries = NULL;
    int count = 0;
    int capacity = 10;
    entries = malloc(capacity * sizeof(ScoreEntry));
    if (!entries) {
        endwin();
        fprintf(stderr, "Memory allocation error.\n");
        sqlite3_finalize(stmt);
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (count >= capacity) {
            capacity *= 2;
            ScoreEntry *temp = realloc(entries, capacity * sizeof(ScoreEntry));
            if (!temp) {
                endwin();
                fprintf(stderr, "Memory allocation error.\n");
                free(entries);
                sqlite3_finalize(stmt);
                return;
            }
            entries = temp;
        }
        /* Allocate and copy the fields */
        entries[count].username = strdup((const char *)sqlite3_column_text(stmt, 0));
        entries[count].total_point = sqlite3_column_int(stmt, 1);
        entries[count].total_gold  = sqlite3_column_int(stmt, 2);
        entries[count].total_played = sqlite3_column_int(stmt, 3);
        entries[count].first_time = strdup((const char *)sqlite3_column_text(stmt, 4));
        count++;
    }
    sqlite3_finalize(stmt);

    /* Create a pad to display the scoreboard.
       We'll make the pad height a bit larger than count + header rows. */
    int padHeight = count + 4;  // header + some margin
    int padWidth = COLS - 2;    // use nearly the full screen width

    WINDOW *pad = newpad(padHeight, padWidth);
    if (!pad) {
        endwin();
        fprintf(stderr, "Failed to create pad.\n");
        for (int i = 0; i < count; i++) {
            free(entries[i].username);
            free(entries[i].first_time);
        }
        free(entries);
        return;
    }

    /* Clear the pad and draw a header */
    werase(pad);
    wattron(pad, A_UNDERLINE | A_BOLD);
    mvwprintw(pad, 0, 0, "Rank   Username             Points   Gold   Games   First Time");
    wattroff(pad, A_UNDERLINE | A_BOLD);
    mvwhline(pad, 1, 0, ACS_HLINE, padWidth);

    /* Define color pairs for the top three ranks.  
       You should initialize these in your main ncurses initialization code.
       For example:
         init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Gold
         init_pair(2, COLOR_WHITE,  COLOR_BLACK);   // Silver
         init_pair(3, COLOR_RED,    COLOR_BLACK);   // Bronze  (or choose an appropriate color)
         init_pair(4, COLOR_CYAN,   COLOR_BLACK);   // For normal rows if desired.
    */
    int row;
    for (row = 0; row < count; row++) {
        int dispRow = row + 2;  // offset rows for header

        /* Decide on special attributes based on rank */
        if (row == 0) {
            wattron(pad, COLOR_PAIR(1) | A_BOLD);  // Gold - biggest
        } else if (row == 1) {
            wattron(pad, COLOR_PAIR(2));           // Silver - normal weight
        } else if (row == 2) {
            wattron(pad, COLOR_PAIR(3) | A_BOLD);    // Bronze - bold but not as much as first
        } else {
            wattron(pad, COLOR_PAIR(4));
        }

        /* If current user is not Guest and this is the current user, add extra emphasis */
        if (strcmp(currentUser, "Guest") != 0 && strcmp(entries[row].username, currentUser) == 0) {
            wattron(pad, A_BOLD);
        }

        /* Print the row (adjust the column widths as needed) */
        mvwprintw(pad, dispRow, 0, "%-5d  %-20s  %-7d  %-5d  %-7d  %-19s",
                  row + 1,
                  entries[row].username,
                  entries[row].total_point,
                  entries[row].total_gold,
                  entries[row].total_played,
                  entries[row].first_time);

        /* Turn off the attributes we set */
        wattroff(pad, A_BOLD | COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | COLOR_PAIR(4));

        /* Optionally, add a horizontal line after each row if desired:
           mvwhline(pad, dispRow + 1, 0, ACS_HLINE, padWidth);
        */
    }

    /* Display instructions at the bottom of the pad (or screen) */
    mvwprintw(pad, padHeight - 1, 0, "Use UP/DOWN to scroll, F1 to exit.");

    /* Initial pad display parameters */
    int padPos = 0;       // starting row in pad to display
    int scrHeight = LINES - 2; // number of rows available on the screen (adjust as needed)
    int scrWidth = COLS - 2;   // number of columns available

    keypad(stdscr, TRUE);
    int ch;
    bool exitLoop = false;
    while (!exitLoop) {
        /* Display portion of the pad in the center of the screen */
        prefresh(pad, padPos, 0, 1, 1, scrHeight, scrWidth);

        ch = getch();
        switch(ch) {
            case KEY_UP:
                if (padPos > 0)
                    padPos--;
                break;
            case KEY_DOWN:
                if (padPos < padHeight - scrHeight)
                    padPos++;
                break;
            case KEY_F(1):
                exitLoop = true;
                /* Clean up */
                delwin(pad);
                for (int i = 0; i < count; i++) {
                    free(entries[i].username);
                    free(entries[i].first_time);
                }
                free(entries);
                return; 
                break;
            default:
                break;
        }
    }

}


void generate_map(char map[MAP_HEIGHT][MAP_WIDTH + 1]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            // Place a wall on the borders; otherwise, a floor.
            if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
                map[i][j] = '#';
            } else {
                map[i][j] = '.';
            }
        }
        // Null-terminate each row for printing as a string.
        map[i][MAP_WIDTH] = '\0';
    }
}

void display_game() {
    // Initialize ncurses
    initscr();              // Start curses mode
    noecho();               // Don't echo input characters
    curs_set(FALSE);        // Hide the cursor
    keypad(stdscr, TRUE);   // Enable arrow keys and other special keys

    // Clear the screen and draw a border around the window
    clear();
    box(stdscr, 0, 0);

    // Generate the map
    char map[MAP_HEIGHT][MAP_WIDTH + 1];
    generate_map(map);

    // Calculate starting position to center the map on the screen.
    // Adjust these values based on your terminal size.
    int start_y = (LINES - MAP_HEIGHT) / 2;
    int start_x = (COLS - MAP_WIDTH) / 2;

    // Print the map on the screen.
    for (int i = 0; i < MAP_HEIGHT; i++) {
        mvprintw(start_y + i, start_x, "%s", map[i]);
    }

    // Optional: Add a title or instructions below the map.
    mvprintw(start_y + MAP_HEIGHT + 1, start_x, "Press any key to exit...");

    // Refresh the screen to show changes
    refresh();

    // Wait for a key press before exiting
    getch();

    // End ncurses mode and restore normal terminal behavior
    endwin();
}


