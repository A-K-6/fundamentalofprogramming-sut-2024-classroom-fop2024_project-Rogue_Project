#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

// --- Constants & Type Definitions ---

#define MAP_WIDTH       80
#define MAP_HEIGHT      24
#define MAX_ROOMS       10
#define NUM_FLOORS      4
#define MESSAGE_HEIGHT  3
#define STATUS_WIDTH    20
#define FOV_RADIUS      5

// Custom key codes for shifted arrow keys.
#define KEY_SUP     1001
#define KEY_SDOWN   1002
#define KEY_SLEFT   1003
#define KEY_SRIGHT  1004

typedef struct {
    int x, y, width, height;
    int door_wall;  // which wall has the door
    int door_x, door_y;
} Room;

typedef struct {
    Room rooms[MAX_ROOMS];
    int count;
} FloorRooms;

typedef struct {
    int x, y;
    int color;
    int Gold;
    int Armor;
    int Exp;
    int HP;
    char *username;
} Player;

// Global arrays for floors and discovered cells.
char floors[NUM_FLOORS][MAP_HEIGHT][MAP_WIDTH];
bool discovered[NUM_FLOORS][MAP_HEIGHT][MAP_WIDTH] = {{{0}}};

// Global variable for map display.
bool showFullMap = false;

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

// --- New: Place Gold Coins in a Room ---
// Randomly places 1–3 coins on floor cells.
void placeGoldInRoom(char map[MAP_HEIGHT][MAP_WIDTH], Room room) {
    int numGold = 1 + rand() % 3;
    for (int i = 0; i < numGold; i++) {
        int x = room.x + 1 + rand() % (room.width - 2);
        int y = room.y + 1 + rand() % (room.height - 2);
        if (map[y][x] == '.') {  // Only place coin on a floor.
            map[y][x] = '$';
        }
    }
}

void placeBlackGoldInRoom(char map[MAP_HEIGHT][MAP_WIDTH], Room room) {
    int numGold = 1 + rand() % 2;
    for (int i = 0; i < numGold; i++) {
        int x = room.x + 1 + rand() % (room.width - 2);
        int y = room.y + 1 + rand() % (room.height - 2);
        if (map[y][x] == '.') {  // Only place coin on a floor.
            map[y][x] = '*';
        }
    }
}
// --- New: Place Traps in a Room ---
// Randomly places 0–2 traps on floor cells.
void placeTrapInRoom(char map[MAP_HEIGHT][MAP_WIDTH], Room room) {
    int numTraps = rand() % 3; // 0, 1, or 2 traps.
    for (int i = 0; i < numTraps; i++) {
        int x = room.x + 1 + rand() % (room.width - 2);
        int y = room.y + 1 + rand() % (room.height - 2);
        if (map[y][x] == '.') {  // Only place trap on a floor cell.
            map[y][x] = '^';
        }
    }
}

// --- New: Place Food in a Room ---
// Randomly places food on a floor cell (0–1 food per room).
void placeFoodInRoom(char map[MAP_HEIGHT][MAP_WIDTH], Room room) {
    // 50% chance to place food.
    if (rand() % 2 == 0) {
        int x = room.x + 1 + rand() % (room.width - 2);
        int y = room.y + 1 + rand() % (room.height - 2);
        if (map[y][x] == '.') {  // Only place food on a floor.
            map[y][x] = 'f';
        }
    }
    
}

void placeFoodInRoom_d(char map[MAP_HEIGHT][MAP_WIDTH], Room room) {
    // 50% chance to place food.
    if (rand() % 2 == 0) {
        int x = room.x + 1 + rand() % (room.width - 2);
        int y = room.y + 1 + rand() % (room.height - 2);
        if (map[y][x] == '.') {  // Only place food on a floor.
            map[y][x] = 'd';
        }
    }
    
}

/*s is added by 8*/
void placeFoodInRoom_s(char map[MAP_HEIGHT][MAP_WIDTH], Room room) {
    // 50% chance to place food.
    if (rand() % 2 == 0) {
        int x = room.x + 1 + rand() % (room.width - 2);
        int y = room.y + 1 + rand() % (room.height - 2);
        if (map[y][x] == '.') {  // Only place food on a floor.
            map[y][x] = 's';
        }
    }
    
}
/* a is added 9*/
void placeFoodInRoom_a(char map[MAP_HEIGHT][MAP_WIDTH], Room room) {
    // 50% chance to place food.
    if (rand() % 2 == 0) {
        int x = room.x + 1 + rand() % (room.width - 2);
        int y = room.y + 1 + rand() % (room.height - 2);
        if (map[y][x] == '.') {  // Only place food on a floor.
            map[y][x] = 'a';
        }
    }
    
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
// Carves the map, records rooms in FloorRooms, and accepts an optional forced room.
void generateMapWithRooms(char map[MAP_HEIGHT][MAP_WIDTH], FloorRooms *fr, Room *forcedRoom) {
    int i, j;
    // Clear the map.
    for (j = 0; j < MAP_HEIGHT; j++) {
        for (i = 0; i < MAP_WIDTH; i++) {
            map[j][i] = ' ';
        }
    }
    
    fr->count = 0;
    
    // If there is a forced room, carve it first.
    if (forcedRoom) {
        fr->rooms[fr->count++] = *forcedRoom;
        carveRoom(map, *forcedRoom);
        placeGoldInRoom(map, *forcedRoom);
        placeBlackGoldInRoom(map, *forcedRoom);
        
        placeTrapInRoom(map, *forcedRoom);
        
        placeFoodInRoom(map, *forcedRoom);
        placeFoodInRoom_d(map, *forcedRoom);
        placeFoodInRoom_s(map, *forcedRoom);
        placeFoodInRoom_a(map, *forcedRoom);

    }
    
    // Generate additional random rooms.
    int attempts = 0;
    while (fr->count < MAX_ROOMS && attempts < 1000) {
        Room newRoom = generateRandomRoom();
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
            placeGoldInRoom(map, newRoom);
            placeBlackGoldInRoom(map, newRoom);
            placeTrapInRoom(map, newRoom);
            placeFoodInRoom(map, newRoom);
        }
        attempts++;
    }
    
    // Connect adjacent rooms with corridors.
    for (i = 0; i < fr->count - 1; i++) {
        drawCorridorBetweenRooms(map, fr->rooms[i], fr->rooms[i+1]);
    }
}

// --- Stair Position Helper ---
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
    return (cell == '.' || cell == '#' || cell == '+' ||
            cell == '<' || cell == '>' || cell == '$' ||
            cell == 'E' || cell == '^' || cell == 'f' ||
            cell == '*' || cell == 'd' || cell == 's' ||
            cell == 'a');
}

// --- Input Parsing ---
int readInput() {
    int ch = getch();
    if (ch == 27) { // Escape character.
        int seq[6] = {0};
        for (int i = 0; i < 5; i++) {
            int next = getch();
            if (next == ERR)
                break;
            seq[i] = next;
        }
        if (seq[0] == '[' && seq[1] == '1' && seq[2] == ';' && seq[3] == '2') {
            switch(seq[4]) {
                case 'A': return KEY_SUP;
                case 'B': return KEY_SDOWN;
                case 'C': return KEY_SRIGHT;
                case 'D': return KEY_SLEFT;
            }
        }
        return ch;
    }
    return ch;
}

// --- Global Variables for Floor Setup ---
FloorRooms floorRooms[NUM_FLOORS];
int stairA_x, stairA_y;  // Floor 1 <-> Floor 2
int stairB_x, stairB_y;  // Floor 2 <-> Floor 3
int stairC_x, stairC_y;  // Floor 3 <-> Floor 4
Room endGameRoom;

// --- Floor Setup ---
// Generates floors with matching stair rooms and an end game room.
void setupFloors() {
    // Floor 1: no forced room.
    generateMapWithRooms(floors[0], &floorRooms[0], NULL);
    
    // Choose a room from floor 1 to be the stair room.
    Room stairRoom1;
    if (floorRooms[0].count > 0) {
        stairRoom1 = floorRooms[0].rooms[0];
        chooseStairPosition(stairRoom1, &stairA_x, &stairA_y);
        floors[0][stairA_y][stairA_x] = '<';
    }
    
    // Floor 2: force the room from floor 1.
    generateMapWithRooms(floors[1], &floorRooms[1], &stairRoom1);
    floors[1][stairA_y][stairA_x] = '>';
    
    // Choose another room from floor 2 for the next stair.
    Room stairRoom2;
    if (floorRooms[1].count > 1) {
        stairRoom2 = floorRooms[1].rooms[floorRooms[1].count - 1];
        chooseStairPosition(stairRoom2, &stairB_x, &stairB_y);
        floors[1][stairB_y][stairB_x] = '<';
    }
    
    // Floor 3: force the room from floor 2 that has the upward stair.
    generateMapWithRooms(floors[2], &floorRooms[2], &stairRoom2);
    floors[2][stairB_y][stairB_x] = '>';
    
    // Choose a room from floor 3 for the next upward stair.
    Room stairRoom3;
    if (floorRooms[2].count > 1) {
        stairRoom3 = floorRooms[2].rooms[floorRooms[2].count - 1];
        chooseStairPosition(stairRoom3, &stairC_x, &stairC_y);
        floors[2][stairC_y][stairC_x] = '<';
    }
    
    // Floor 4: force the room from floor 3 that has the upward stair.
    generateMapWithRooms(floors[3], &floorRooms[3], &stairRoom3);
    floors[3][stairC_y][stairC_x] = '>';
    
    // Designate an extra room on floor 4 as the end game room.
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
    
    mvwprintw(msg_win, 1, 1, "Arrows: move, Shift+arrows: run, m: toggle map, r/g/b/y/p/c: colors, F1: quit");
    wrefresh(msg_win);
    
    srand(time(NULL));
    setupFloors();
    
    // Initialize a sample player.
    Player player;
    player.username = strdup("PlayerOne");
    player.Gold = 50;
    player.Armor = 50;
    player.Exp = 1;
    player.HP = 100;
    player.color = 1;
    player.x = 0;
    player.y = 0;
    
    mvwprintw(status_win, 1, 1, "Name: %s", player.username);
    mvwprintw(status_win, 2, 1, "Gold: %d", player.Gold);
    mvwprintw(status_win, 3, 1, "Armor: %d", player.Armor);
    mvwprintw(status_win, 4, 1, "Exp: %d", player.Exp);
    mvwprintw(status_win, 5, 1, "HP: %d", player.HP);
    wrefresh(status_win);
    
    // Find an allowed cell on Floor 1.
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
    
    bool stairLock = false;
    int ch;
    bool gameEnded = false;
    
    while ((ch = readInput()) != KEY_F(1) && !gameEnded) {
        // Update status window.
        werase(status_win);
        box(status_win, 0, 0);
        mvwprintw(status_win, 1, 1, "Name: %s", player.username);
        mvwprintw(status_win, 2, 1, "Gold: %d", player.Gold);
        mvwprintw(status_win, 3, 1, "Armor: %d", player.Armor);
        mvwprintw(status_win, 4, 1, "Exp: %d", player.Exp);
        mvwprintw(status_win, 5, 1, "HP: %d", player.HP);
        mvwprintw(status_win, 8, 1, "m: toggle map");
        mvwprintw(status_win, 10, 1, "r/g/b/y/p/c: color");
        wrefresh(status_win);
        
        if (ch != ERR) {
            // Toggle full map display.
            if (ch == 'm') {
                showFullMap = !showFullMap;
            }
            else if (ch == KEY_SLEFT) {
                while (player.x - 1 >= 0 &&
                       isAllowedCell(floors[current_floor][player.y][player.x - 1])) {
                    player.x--;
                    updateDiscovered(current_floor, player.x, player.y);
                    werase(game_win);
                    box(game_win, 0, 0);
                    displayMap(game_win, current_floor, &player);
                    napms(30);
                }
                continue;
            } else if (ch == KEY_SRIGHT) {
                while (player.x + 1 < MAP_WIDTH &&
                       isAllowedCell(floors[current_floor][player.y][player.x + 1])) {
                    player.x++;
                    updateDiscovered(current_floor, player.x, player.y);
                    werase(game_win);
                    box(game_win, 0, 0);
                    displayMap(game_win, current_floor, &player);
                    napms(30);
                }
                continue;
            } else if (ch == KEY_SUP) {
                while (player.y - 1 >= 0 &&
                       isAllowedCell(floors[current_floor][player.y - 1][player.x])) {
                    player.y--;
                    updateDiscovered(current_floor, player.x, player.y);
                    werase(game_win);
                    box(game_win, 0, 0);
                    displayMap(game_win, current_floor, &player);
                    napms(30);
                }
                continue;
            } else if (ch == KEY_SDOWN) {
                while (player.y + 1 < MAP_HEIGHT &&
                       isAllowedCell(floors[current_floor][player.y + 1][player.x])) {
                    player.y++;
                    updateDiscovered(current_floor, player.x, player.y);
                    werase(game_win);
                    box(game_win, 0, 0);
                    displayMap(game_win, current_floor, &player);
                    napms(30);
                }
                continue;
            }
            
            // Regular one-cell movement and color changes.
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
            else if (ch == 'p') player.color = 5;
            else if (ch == 'c') player.color = 6;
            
            if (new_x >= 0 && new_x < MAP_WIDTH &&
                new_y >= 0 && new_y < MAP_HEIGHT &&
                isAllowedCell(floors[current_floor][new_y][new_x])) {
                player.x = new_x;
                player.y = new_y;
                updateDiscovered(current_floor, player.x, player.y);
                
                // --- Gold Coin Pickup ---
                if (floors[current_floor][player.y][player.x] == '$') {
                    player.Gold += 10;
                    floors[current_floor][player.y][player.x] = '.';
                }
                else if (floors[current_floor][player.y][player.x] == '*') {
                    player.Gold += 20;
                    floors[current_floor][player.y][player.x] = '.';
                }
                // --- Trap Trigger ---
                else if (floors[current_floor][player.y][player.x] == '^') {
                    player.HP -= 10;  // Deduct 10 HP.
                    floors[current_floor][player.y][player.x] = '.';
                }
                // --- Food Consumption ---
                else if (floors[current_floor][player.y][player.x] == 'f') {
                    player.HP += 10;  // Increase HP by 10.
                    floors[current_floor][player.y][player.x] = '.';
                }
                else if (floors[current_floor][player.y][player.x] == 'd') {
                    player.HP += 9;  // Increase HP by 10.
                    floors[current_floor][player.y][player.x] = '.';
                }
                else if (floors[current_floor][player.y][player.x] == 's') {
                    player.HP += 8;  // Increase HP by 10.
                    floors[current_floor][player.y][player.x] = 's';
                }
                else if (floors[current_floor][player.y][player.x] == 'a') {
                    player.HP += 7;  // Increase HP by 10.
                    floors[current_floor][player.y][player.x] = '.';
                }
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
    
    free(player.username);
}

int main() {
    displayGame();
    return 0;
}
