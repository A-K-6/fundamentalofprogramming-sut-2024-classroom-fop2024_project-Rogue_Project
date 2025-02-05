// 403 171 146 
// Aeen Karimian

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <menu.h>
#include <form.h>
#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <sqlite3.h>
#include <stdbool.h>





#define MESSAGE_HEIGHT 3   // Height for the message bar
#define STATUS_WIDTH   20  // Width for the status display


// --- Constants ---
#define MESSAGE_HEIGHT 3     // Height for the message bar
#define STATUS_WIDTH   20    // Width for the status display

// Map dimensions.
#define MAP_WIDTH  80
#define MAP_HEIGHT 24

#define MAX_ROOMS 6
#define NUM_FLOORS 4      // Floors 1 to 4

// Field of view.
#define FOV_RADIUS 2

// How long (ms) after the last 'm' key press to continue showing full map.
#define MAP_SHOW_THRESHOLD 150

// --- Fallback definitions for shifted arrow keys if not defined ---
#ifndef KEY_SLEFT
#define KEY_SLEFT 393
#endif
#ifndef KEY_SRIGHT
#define KEY_SRIGHT 402
#endif
#ifndef KEY_SUP
#define KEY_SUP 337
#endif
#ifndef KEY_SDOWN
#define KEY_SDOWN 336
#endif

typedef struct {
    char *username;
    char *password;
    char *email;
    int Gold; 
    int Armor; 
    int Exp; 
    int HP; 
    int x, y;
    int color;  // Color pair number
} User;


User main_user = {.username ="Guest", .Armor = 60, .Exp = 0, .HP= 100, .Exp= 0};


/* Structure to hold a scoreboard entry */
typedef struct {
    char *username;
    int total_point;
    int total_gold;
    int total_played;
    char *first_time;
} ScoreEntry;






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



//actual game. 

void displayGame(); 

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
            /*pregame menu*/
            int pregame_option = pregame_menu();
            while (pregame_option != 4)
            {
                

                switch (pregame_option)
                {
                case 0:
                        // continue game. 
                        displayGame();    
                        clear(); 
                        endwin(); // ends the program. 
                        refresh();
                        refresh();
                        pregame_option = pregame_menu();
                        refresh(); 
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
                    endwin(); // ends the program. 
                    clear(); 
                    refresh();
                    pregame_option = pregame_menu();
                    clear(); 
                    refresh(); 
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
                // return ; 
                exitLoop = true;
                /* Clean up */

                delwin(pad);
                
                refresh();
                endwin(); 
                for (int i = 0; i < count; i++) {
                    free(entries[i].username);
                    free(entries[i].first_time);
                }
                free(entries);
                return; 
                break;

        }
    }
    return; 
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





// --- Global Variables ---
// Global flag and timer for full-map view.
int showFullMap = 0;
long last_m_time_ms = 0;

// Each floor has its own discovered grid.
bool discovered[NUM_FLOORS][MAP_HEIGHT][MAP_WIDTH] = { { { false } } };

// The maps for each floor.
char floors[NUM_FLOORS][MAP_HEIGHT][MAP_WIDTH];

// --- Structures ---
// Structure for room info.
typedef struct {
    int x, y;          // Top-left corner (including walls)
    int width, height; // Dimensions (including walls)
    int door_x, door_y;// Door coordinates on one wall
    int door_wall;     // 0=top, 1=bottom, 2=left, 3=right
} Room;

// Structure to hold all rooms in a floor.
typedef struct {
    Room rooms[MAX_ROOMS];
    int count;
} FloorRooms;

// Structure for player info.
typedef struct {
    int x, y;
    int color;  // Color pair number
} Player;

// --- Helper Functions ---

// Returns non-zero if the two rooms overlap.
int roomsOverlap(Room a, Room b) {
    return !(a.x + a.width + 1 < b.x ||
             b.x + b.width + 1 < a.x ||
             a.y + a.height + 1 < b.y ||
             b.y + b.height + 1 < a.y);
}

// Generate a random room.
Room generateRandomRoom() {
    Room room;
    int minSize = 6;
    int maxSizeW = 12;
    int maxSizeH = 12;
    
    room.width = minSize + rand() % (maxSizeW - minSize + 1);
    room.height = minSize + rand() % (maxSizeH - minSize + 1);
    
    room.x = rand() % (MAP_WIDTH - room.width - 1) + 1;
    room.y = rand() % (MAP_HEIGHT - room.height - 1) + 1;
    
    int wall = rand() % 4;
    room.door_wall = wall;
    switch (wall) {
        case 0:
            room.door_x = room.x + 1 + rand() % (room.width - 2);
            room.door_y = room.y;
            break;
        case 1:
            room.door_x = room.x + 1 + rand() % (room.width - 2);
            room.door_y = room.y + room.height - 1;
            break;
        case 2:
            room.door_x = room.x;
            room.door_y = room.y + 1 + rand() % (room.height - 2);
            break;
        case 3:
            room.door_x = room.x + room.width - 1;
            room.door_y = room.y + 1 + rand() % (room.height - 2);
            break;
    }
    return room;
}

// Carve a room into the map.
void carveRoom(char map[MAP_HEIGHT][MAP_WIDTH], Room room) {
    int i, j;
    // Carve horizontal walls.
    for (i = room.x; i < room.x + room.width; i++) {
        map[room.y][i] = '_';
        map[room.y + room.height - 1][i] = '_';
    }
    // Carve vertical walls.
    for (j = room.y; j < room.y + room.height; j++) {
        map[j][room.x] = '|';
        map[j][room.x + room.width - 1] = '|';
    }
    // Carve floor.
    for (j = room.y + 1; j < room.y + room.height - 1; j++) {
        for (i = room.x + 1; i < room.x + room.width - 1; i++) {
            map[j][i] = '.';
        }
    }
    // Place the door.
    map[room.door_y][room.door_x] = '+';
}

// --- Corridor Carving ---
// A simple BFS corridor-carver.
typedef struct {
    int x, y;
} Point;

int carveCorridorPath(char map[MAP_HEIGHT][MAP_WIDTH], int sx, int sy, int ex, int ey) {
    bool visited[MAP_HEIGHT][MAP_WIDTH] = {0};
    Point predecessor[MAP_HEIGHT][MAP_WIDTH];
    for (int j = 0; j < MAP_HEIGHT; j++){
        for (int i = 0; i < MAP_WIDTH; i++){
            predecessor[j][i].x = -1;
            predecessor[j][i].y = -1;
        }
    }
    Point queue[MAP_WIDTH * MAP_HEIGHT];
    int front = 0, rear = 0;
    queue[rear++] = (Point){sx, sy};
    visited[sy][sx] = true;
    
    int directions[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
    bool found = false;
    
    while (front < rear) {
        Point current = queue[front++];
        if (current.x == ex && current.y == ey) {
            found = true;
            break;
        }
        for (int d = 0; d < 4; d++){
            int nx = current.x + directions[d][0];
            int ny = current.y + directions[d][1];
            if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT)
                continue;
            if (visited[ny][nx])
                continue;
            if (map[ny][nx] == ' ' || map[ny][nx] == '#' || map[ny][nx] == '+') {
                visited[ny][nx] = true;
                predecessor[ny][nx] = current;
                queue[rear++] = (Point){nx, ny};
            }
        }
    }
    if (!found)
        return 0;
    
    Point p = (Point){ex, ey};
    while (!(p.x == sx && p.y == sy)) {
        if (map[p.y][p.x] == ' ')
            map[p.y][p.x] = '#';
        p = predecessor[p.y][p.x];
    }
    return 1;
}

void drawCorridorBetweenRooms(char map[MAP_HEIGHT][MAP_WIDTH], Room room1, Room room2) {
    int sx = room1.door_x, sy = room1.door_y;
    int ex = room2.door_x, ey = room2.door_y;
    carveCorridorPath(map, sx, sy, ex, ey);
    map[room1.door_y][room1.door_x] = '+';
    map[room2.door_y][room2.door_x] = '+';
}

// --- Map Generator with Room List ---
// This function carves the map, records the rooms in FloorRooms,
// and accepts an optional forcedRoom (if non-NULL, it is carved first).
void generateMapWithRooms(char map[MAP_HEIGHT][MAP_WIDTH], FloorRooms *fr, Room *forcedRoom) {
    int i, j;
    // Clear the map.
    for (j = 0; j < MAP_HEIGHT; j++) {
        for (i = 0; i < MAP_WIDTH; i++) {
            map[j][i] = ' ';
        }
    }
    
    fr->count = 0;
    
    // If we have a forced room, carve it first.
    if (forcedRoom) {
        fr->rooms[fr->count++] = *forcedRoom;
        carveRoom(map, *forcedRoom);
    }
    
    // Generate additional random rooms.
    int attempts = 0;
    while (fr->count < MAX_ROOMS && attempts < 1000) {
        Room newRoom = generateRandomRoom();
        // If there is a forced room, ensure no overlap.
        if (forcedRoom && roomsOverlap(newRoom, *forcedRoom)) {
            attempts++;
            continue;
        }
        int overlaps = 0;
        for (i = 0; i < fr->count; i++) {
            if (roomsOverlap(newRoom, fr->rooms[i])) {
                overlaps = 1;
                break;
            }
        }
        if (!overlaps) {
            fr->rooms[fr->count++] = newRoom;
            carveRoom(map, newRoom);
        }
        attempts++;
    }
    
    // Connect adjacent rooms with corridors.
    for (i = 0; i < fr->count - 1; i++) {
        drawCorridorBetweenRooms(map, fr->rooms[i], fr->rooms[i+1]);
    }
}

// --- Stair Position Helper ---
// Chooses a random floor cell (not a wall) inside a room.
// Assumes the room is at least 3x3.
void chooseStairPosition(Room room, int *sx, int *sy) {
    *sx = room.x + 1 + rand() % (room.width - 2);
    *sy = room.y + 1 + rand() % (room.height - 2);
}

// --- Player Vision and Display ---
void updateDiscovered(int floor, int player_x, int player_y) {
    for (int dy = -FOV_RADIUS; dy <= FOV_RADIUS; dy++) {
        for (int dx = -FOV_RADIUS; dx <= FOV_RADIUS; dx++) {
            int x = player_x + dx;
            int y = player_y + dy;
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                discovered[floor][y][x] = true;
            }
        }
    }
}

void displayMap(WINDOW *game_win, int floor, Player *player) {
    int i, j;
    for (j = 0; j < MAP_HEIGHT; j++) {
        for (i = 0; i < MAP_WIDTH; i++) {
            if (i == player->x && j == player->y) {
                wattron(game_win, COLOR_PAIR(player->color));
                mvwaddch(game_win, j, i, '@');
                wattroff(game_win, COLOR_PAIR(player->color));
            } else if (showFullMap || discovered[floor][j][i]) {
                mvwaddch(game_win, j, i, floors[floor][j][i]);
            } else {
                mvwaddch(game_win, j, i, ' ');
            }
        }
    }
    wrefresh(game_win);
}

// A helper to check if a cell is allowed for movement.
int isAllowedCell(char cell) {
    return (cell == '.' || cell == '#' || cell == '+' || cell == '<' || cell == '>' || cell == 'E');
    // 'E' denotes the end game room's marker.
}

// Get current time in milliseconds.
long getCurrentTimeMs() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

// --- Input Parsing ---
// This function reads a key and, if an escape sequence for a shifted arrow is detected,
// returns a corresponding key code (KEY_SUP, KEY_SDOWN, KEY_SLEFT, KEY_SRIGHT).
int readInput() {
    int ch = getch();
    if (ch == 27) { // Escape character; might be start of an escape sequence.
        // Use non-blocking getch() calls to try to read the full sequence.
        int seq[6];
        memset(seq, 0, sizeof(seq));
        // Try to read up to 5 more characters.
        for (int i = 0; i < 5; i++) {
            int next = getch();
            if (next == ERR)
                break;
            seq[i] = next;
        }
        // Many terminals send: ESC [ 1 ; 2 X  where X is A, B, C, or D.
        if (seq[0] == '[' && seq[1] == '1' && seq[2] == ';' && seq[3] == '2') {
            switch(seq[4]) {
                case 'A': return KEY_SUP;
                case 'B': return KEY_SDOWN;
                case 'C': return KEY_SRIGHT;
                case 'D': return KEY_SLEFT;
            }
        }
        // If not recognized, return the first character.
        return ch;
    }
    return ch;
}

// --- Floor Setup ---
// Global room lists for each floor.
FloorRooms floorRooms[NUM_FLOORS];
// Global variables to store the stair coordinates for each connection.
int stairA_x, stairA_y;  // Connects Floor 1 (up-stair) to Floor 2 (down-stair)
int stairB_x, stairB_y;  // Connects Floor 2 (up-stair) to Floor 3 (down-stair)
int stairC_x, stairC_y;  // Connects Floor 3 (up-stair) to Floor 4 (down-stair)
// We also store the end game room for floor 4.
Room endGameRoom;

// In this function we generate all floors.
// Floor 1 is generated normally. Then we choose a room from floor 1 to carry the stair up,
// force that room into floor 2 so that the corresponding down-stair appears in the same spot,
// and similarly for floors 2→3 and 3→4. In addition, on floor 4 we designate one extra room
// as the end game room.
void setupFloors() {
    // --- Floor 1: no forced room. ---
    generateMapWithRooms(floors[0], &floorRooms[0], NULL);
    
    // Choose a room from floor 1 to be the stair room.
    Room stairRoom1;
    if (floorRooms[0].count > 0) {
        stairRoom1 = floorRooms[0].rooms[0];  // You can choose randomly if desired.
        chooseStairPosition(stairRoom1, &stairA_x, &stairA_y);
        // Place the up-stair ('<') in floor 1.
        floors[0][stairA_y][stairA_x] = '<';
    }
    
    // --- Floor 2: force the room from floor 1. ---
    generateMapWithRooms(floors[1], &floorRooms[1], &stairRoom1);
    floors[1][stairA_y][stairA_x] = '>';
    
    // Choose a different room from floor 2 for the next upward stair.
    Room stairRoom2;
    if (floorRooms[1].count > 1) {
        stairRoom2 = floorRooms[1].rooms[floorRooms[1].count - 1];
        chooseStairPosition(stairRoom2, &stairB_x, &stairB_y);
        floors[1][stairB_y][stairB_x] = '<';
    }
    
    // --- Floor 3: force the room from floor 2 that contains the upward stair.
    generateMapWithRooms(floors[2], &floorRooms[2], &stairRoom2);
    floors[2][stairB_y][stairB_x] = '>';
    
    // Choose a room from floor 3 for the next upward stair.
    Room stairRoom3;
    if (floorRooms[2].count > 1) {
        stairRoom3 = floorRooms[2].rooms[floorRooms[2].count - 1];
        chooseStairPosition(stairRoom3, &stairC_x, &stairC_y);
        floors[2][stairC_y][stairC_x] = '<';
    }
    
    // --- Floor 4: force the room from floor 3 that contains the upward stair.
    generateMapWithRooms(floors[3], &floorRooms[3], &stairRoom3);
    floors[3][stairC_y][stairC_x] = '>';
    
    // Additionally, designate one extra room on floor 4 as the end game room.
    if (floorRooms[3].count > 1) {
        endGameRoom = floorRooms[3].rooms[floorRooms[3].count - 1];
        int ex, ey;
        chooseStairPosition(endGameRoom, &ex, &ey);
        floors[3][ey][ex] = 'E';
    }
}

// --- Main Game Display ---
void displayGame() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(50);  // Wait up to 50 ms for input.
    
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_BLUE,    COLOR_BLACK);
        init_pair(4, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN,    COLOR_BLACK);
    }
    
    refresh();
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    
    int game_height = screen_height - MESSAGE_HEIGHT;
    int game_width = screen_width - STATUS_WIDTH;
    
    WINDOW *msg_win = newwin(MESSAGE_HEIGHT, screen_width, 0, 0);
    box(msg_win, 0, 0);
    wrefresh(msg_win);
    
    WINDOW *game_win = newwin(game_height, game_width, MESSAGE_HEIGHT, 0);
    box(game_win, 0, 0);
    wrefresh(game_win);
    
    WINDOW *status_win = newwin(game_height, STATUS_WIDTH, MESSAGE_HEIGHT, game_width);
    box(status_win, 0, 0);
    wrefresh(status_win);
    
    mvwprintw(msg_win, 1, 1, "Arrow keys: move, Shift+arrow: run, colors: r/g/b/y/m/c, hold 'm' for full map, F1 to quit.");
    wrefresh(msg_win);
    
    mvwprintw(status_win, 1, 1, "Name: Player1");
    mvwprintw(status_win, 2, 1, "Gold: 100");
    mvwprintw(status_win, 3, 1, "Armor: 50");
    mvwprintw(status_win, 4, 1, "Exp: 0");
    mvwprintw(status_win, 5, 1, "HP: 100");
    wrefresh(status_win);
    
    // Seed the random generator.
    srand(time(NULL));
    
    // Generate floors with matching stair rooms and designate the end game room.
    setupFloors();
    
    // Place the player on Floor 1 at an allowed cell (avoid stair cells).
    Player player = {.x= 0, .y = 0, .color = 1};
    bool foundPlayer = false;
    for (int j = 0; j < MAP_HEIGHT && !foundPlayer; j++) {
        for (int i = 0; i < MAP_WIDTH && !foundPlayer; i++) {
            if (isAllowedCell(floors[0][j][i]) &&
                floors[0][j][i] != '<' && floors[0][j][i] != '>') {
                player.x = i;
                player.y = j;
                foundPlayer = true;
            }
        }
    }
    
    int current_floor = 0;  // Start on Floor 1.
    updateDiscovered(current_floor, player.x, player.y);
    
    // Stair lock prevents immediate re-triggering.
    bool stairLock = false;
    int ch;
    bool gameEnded = false;
    
    while ((ch = readInput()) != KEY_F(1) && !gameEnded) {
        if (ch != ERR) {
            // Check for shifted arrow keys and run in that direction.
            if (ch == 'm') {
                last_m_time_ms = getCurrentTimeMs();
                showFullMap = 1;}
            else if (ch == KEY_SLEFT) {
                while (player.x - 1 >= 0 && isAllowedCell(floors[current_floor][player.y][player.x - 1])) {
                    player.x--;
                    updateDiscovered(current_floor, player.x, player.y);
                    werase(game_win);
                    box(game_win, 0, 0);
                    displayMap(game_win, current_floor, &player);
                    napms(30);
                }
                continue;
            } else if (ch == KEY_SRIGHT) {
                while (player.x + 1 < MAP_WIDTH && isAllowedCell(floors[current_floor][player.y][player.x + 1])) {
                    player.x++;
                    updateDiscovered(current_floor, player.x, player.y);
                    werase(game_win);
                    box(game_win, 0, 0);
                    displayMap(game_win, current_floor, &player);
                    napms(30);
                }
                continue;
            } else if (ch == KEY_SUP) {
                while (player.y - 1 >= 0 && isAllowedCell(floors[current_floor][player.y - 1][player.x])) {
                    player.y--;
                    updateDiscovered(current_floor, player.x, player.y);
                    werase(game_win);
                    box(game_win, 0, 0);
                    displayMap(game_win, current_floor, &player);
                    napms(30);
                }
                continue;
            } else if (ch == KEY_SDOWN) {
                while (player.y + 1 < MAP_HEIGHT && isAllowedCell(floors[current_floor][player.y + 1][player.x])) {
                    player.y++;
                    updateDiscovered(current_floor, player.x, player.y);
                    werase(game_win);
                    box(game_win, 0, 0);
                    displayMap(game_win, current_floor, &player);
                    napms(30);
                }
                continue;
            }
            
            // Regular one-cell movement.
            int new_x = player.x, new_y = player.y;
            switch(ch) {
                case KEY_UP:    new_y--; break;
                case KEY_DOWN:  new_y++; break;
                case KEY_LEFT:  new_x--; break;
                case KEY_RIGHT: new_x++; break;
            }
            if (ch == 'r')      player.color = 1;
            else if (ch == 'g') player.color = 2;
            else if (ch == 'b') player.color = 3;
            else if (ch == 'y') player.color = 4;
            else if (ch == 'p') player.color = 5;  // 'm' is also used for full map.
            else if (ch == 'c') player.color = 6;
            
            if (new_x >= 0 && new_x < MAP_WIDTH &&
                new_y >= 0 && new_y < MAP_HEIGHT &&
                isAllowedCell(floors[current_floor][new_y][new_x])) {
                player.x = new_x;
                player.y = new_y;
                updateDiscovered(current_floor, player.x, player.y);
            }
        }
        
        // Floor switching logic.
        char tile = floors[current_floor][player.y][player.x];
        if (!stairLock && (tile == '<' || tile == '>')) {
            if (tile == '<') { // Up-stair.
                if (current_floor < NUM_FLOORS - 1) {
                    current_floor++;
                    updateDiscovered(current_floor, player.x, player.y);
                    stairLock = true;
                }
            } else if (tile == '>') { // Down-stair.
                if (current_floor > 0) {
                    current_floor--;
                    updateDiscovered(current_floor, player.x, player.y);
                    stairLock = true;
                }
            }
        } else if (tile != '<' && tile != '>') {
            stairLock = false;
        }
        
        // Check if on the end game room (on floor 4).
        if (current_floor == NUM_FLOORS - 1 && floorRooms[NUM_FLOORS - 1].count > 1) {
            if (player.x >= endGameRoom.x && player.x < endGameRoom.x + endGameRoom.width &&
                player.y >= endGameRoom.y && player.y < endGameRoom.y + endGameRoom.height) {
                gameEnded = true;
            }
        }
        
        long now_ms = getCurrentTimeMs();
        if (showFullMap && (now_ms - last_m_time_ms > MAP_SHOW_THRESHOLD)) {
            showFullMap = 0;
        }
        
        werase(game_win);
        box(game_win, 0, 0);
        displayMap(game_win, current_floor, &player);
        
        werase(msg_win);
        box(msg_win, 0, 0);
        mvwprintw(msg_win, 1, 1, "Last key: %d, Floor: %d (F1 to quit)", ch, current_floor + 1);
        wrefresh(msg_win);
    }
    
    delwin(msg_win);
    delwin(game_win);
    delwin(status_win);
    endwin();
    
    if (gameEnded) {
        printf("Congratulations! You've reached the end game room. Game Over.\n");
    }
}
