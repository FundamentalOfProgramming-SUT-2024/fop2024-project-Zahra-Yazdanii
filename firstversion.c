#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h> 
#include <stdbool.h>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MIN_MAP_WIDTH 100
#define MIN_MAP_HEIGHT 35
#define MAX_MAP_WIDTH 120
#define MAX_MAP_HEIGHT 45
#define MIN_ROOM_SIZE 8
#define MAX_ROOM_SIZE 12
#define MIN_ROOMS 6
#define MAX_ROOMS 8
#define MAX_HEALTH 100
#define COLOR_NORMAL_ROOM 1
#define COLOR_SPELL_ROOM 2
#define COLOR_TREASURE_ROOM 3
#define CORRIDOR_VISIBLE 5 
#define MAX_SPELLS 10  
#define MAX_FOOD 8  
#define MIN_FOOD 4 


typedef enum {    
    ROOM_NORMAL,   
    ROOM_SPELL,      
    ROOM_TREASURE 
} RoomType;

typedef struct {
    int x, y, width, height;
    RoomType type;
} Room;

Room rooms[100];
int roomCount = 0;

typedef struct {
    int from, to;
} Connection;

Connection connections[100]; 


typedef struct {
    int x, y;            
    int isActive;        
    int passwordShown;   
} PasswordButton;
PasswordButton buttons[10]; 

typedef struct {
    int x, y;            
    int isLocked;        
    char password[5];    
    int attemptsLeft;    
} Door;
Door doors[10];             

typedef struct {
    Room rooms[100];
    int roomCount;
    char map[120][120];
    int playerStartX, playerStartY; 
} Floor;
Floor floors[4];


typedef struct {
    int x, y;    
    char type;   
    int active;  
} Spell;
Spell spells[MAX_SPELLS];


typedef struct {
    int x, y;  
    char symbol;  
} Food;

Food foods[MAX_FOOD];

typedef struct {
    int roomCount;
    Room rooms[100];         
    char map[120][120];      
    int discovered[120][120];
    int playerX, playerY;    
    int playerHealth;        
    int hungerLevel;         
    int goldAmount;          
    //int collectedFood;       
    //int collectedSpells;     
} GameState;

GameState state;

int connectionCount = 0;
int doorCount = 0;
int buttonCount = 0;
int currentFloor = 0;
char map[120][120];
int discovered[120][120] = {0}; 
int showFullMap = 0; 
int adjacencyMatrix[120][120]; 
int MAP_HEIGHT,MAP_WIDTH;
int playerX;
int playerY;
char prevCell = '.';
int prevStairX = -1; 
int prevStairY = -1; 
int collectedHealth = 0;  
int collectedSpeed = 0;   
int collectedDamage = 0;  
int playerHealth = MAX_HEALTH;
int goldAmount=0;
int hungerLevel = 0;  
int moveCounter = 0;
int foodCount = 0;  
char currentUsername[50] = ""; 


void mainMenu();
void exitGame();
void settingsMenu();
void showLeaderboard();
void showProfile();
void playGame();
void loginMenu();
void loginAsGuest() ;
void forgotPassword() ;
void loginUser(); 
void gameMenu() ;
void continueOldGame();
void playNewGame();
int checkUserEmail(const char *username, const char *email, char *password);
int checkCredentials(const char *username, const char *password) ;
void createNewUser() ;
void trimNewline(char *str);
void createNewUser();
void generateRandomPassword(char *password, size_t length);
int isEmailValid(const char *email);
int isPasswordValid(const char *password);
void saveNewUser(const char *username, const char *password, const char *email) ;
int doesUserExist(const char *username);
void initializeMap();
int checkRoomOverlap(int x, int y, int width, int height);
void addRoom(int x, int y, int width, int height);
int connectRooms(int roomA, int roomB);
void printMapNcurses();
void movePlayer(char input);
void createLockedDoor();
void placeStaircase(char staircaseSymbol);
void printNewMap();
void generateNewFloor(int floorNumber);
void revealCorridor(int x, int y, int dx, int dy) ;
void interactWithSpell(char spellType, int spellX, int spellY);
void revealCurrentRoom();
int findRoomWithDoor() ;
void placeAmpersand(Room room) ;
void placepiller();
void placeTraps();
void saveGame();
void placeSpells();
void increaseHunger();
int findDeadEndRoom() ;
void generateNewFloor(int floorNumber) ;
void convertEntryRoomToSpellRoom();
void setupFirstFloor();
int start();
void revealCorridor(int startX, int startY, int dx, int dy);
void revealCurrentRoom();
void displayPlayerStats();
void placeFood() ;

int main() {
    setlocale(LC_ALL,"C.UTF-8");
    initscr();            
    cbreak();              
    noecho();             
    keypad(stdscr, TRUE); 

    mainMenu();           

    endwin();             
    return 0;
}


void saveGame() {
    if (currentUsername[0] == '\0') return; 

    char filename[100];
    snprintf(filename, sizeof(filename), "%s_save.dat", currentUsername);

    FILE *file = fopen(filename, "wb");
    if (!file) {
        mvprintw(LINES - 2, 0, "⚠ Error: Could not save the game!");
        refresh();
        return;
    }

    
    state.roomCount = roomCount;
    memcpy(state.rooms, rooms, sizeof(rooms));
    memcpy(state.map, map, sizeof(map));
    memcpy(state.discovered, discovered, sizeof(discovered));
    state.playerX = playerX;
    state.playerY = playerY;
    state.playerHealth = playerHealth;
    state.hungerLevel = hungerLevel;
    state.goldAmount = goldAmount;
    //state.collectedFood = collectedFood;
    //state.collectedSpells = collectedSpells;

    fwrite(&state, sizeof(GameState), 1, file); 
    fclose(file);

    mvprintw(LINES - 2, 0, "✅ Game saved successfully!");
    refresh();
}


/*void loadGame() {
    if (currentUsername[0] == '\0') {
        mvprintw(LINES - 2, 0, "⚠️ No user logged in! Cannot load.");
        refresh();
        return;
    }

    char filename[100];
    snprintf(filename, sizeof(filename), "%s_save.dat", currentUsername);

    FILE *file = fopen(filename, "rb");
    if (!file) {
        mvprintw(LINES - 2, 0, "⚠️ No saved game found!");
        refresh();
        return;
    }

    fread(&state, sizeof(GameState), 1, file); 
    fclose(file);

    roomCount = state.roomCount;
    memcpy(rooms, state.rooms, sizeof(rooms));
    memcpy(map, state.map, sizeof(map));
    memcpy(discovered, state.discovered, sizeof(discovered));
    playerX = state.playerX;
    playerY = state.playerY;
    playerHealth = state.playerHealth;
    hungerLevel = state.hungerLevel;
    goldAmount = state.goldAmount;
    map[playerY][playerX] = 'p';

   
    clear();
    revealCurrentRoom(); 
    printMapNcurses();   
    displayGold();
    displayPlayerStats();
    refresh();

    mvprintw(LINES - 2, 0, "✅ Game loaded successfully.");
    refresh();
}*/



void placeSpells() {
    int numSpells = 3 + rand() % (MAX_SPELLS - 3);  
    char spellTypes[3] = {'H', 'S', 'D'}; 

    for (int i = 0; i < numSpells; i++) {
        int x, y;
        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.');  

        spells[i].x = x;
        spells[i].y = y;
        spells[i].type = spellTypes[rand() % 3];  
        spells[i].active = 1;
        map[y][x] = spells[i].type; 
    }
}

void increaseHunger() {
    moveCounter++; 
    
    if (moveCounter >= 20) {  
        hungerLevel += 5;  
        if (hungerLevel > 100) hungerLevel = 100;  
        moveCounter = 0;  

       
        if (hungerLevel % 20 == 0) { 
            playerHealth -= 5;  
            if (playerHealth < 0) playerHealth = 0;
        }
    }
}
/*
void initRoomColors() {
start_color();
// تعریف جفت‌های رنگی:
init_pair(1, COLOR_WHITE, COLOR_BLACK);    // برای راهروها
init_pair(2, COLOR_YELLOW, COLOR_BLACK);     // برای دیوار یا فضای اتاق معمولی
init_pair(3, COLOR_MAGENTA, COLOR_BLACK);    // برای اتاق طلسم
init_pair(4, COLOR_CYAN, COLOR_BLACK);       // برای اتاق گنج
}


void ensureTreasureRooms() {
    if (currentFloor != 3) return;  // فقط در طبقه ۴ اجرا شود

    int treasureCount = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].type == ROOM_TREASURE)
            treasureCount++;
    }

    // اگر هیچ اتاق گنجی ساخته نشده، حتماً ۱ عدد درست کن
    if (treasureCount == 0 && roomCount > 0) {
        int randomRoom = rand() % roomCount;
        rooms[randomRoom].type = ROOM_TREASURE;
    }
}


// 📌 تابع پیدا کردن نزدیک‌ترین اتاق طلسم برای در مخفی
int findClosestSpellRoom(int x, int y) {
    int closestIndex = -1;
    float minDistance = 10000;

    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].type == ROOM_SPELL) {
            int centerX = rooms[i].x + rooms[i].width / 2;
            int centerY = rooms[i].y + rooms[i].height / 2;
            float distance = sqrt(pow(centerX - x, 2) + pow(centerY - y, 2));
            if (distance < minDistance) {
                minDistance = distance;
                closestIndex = i;
            }
        }
    }
    return closestIndex;
}


// 📌 تابع قرار دادن در مخفی از اتاق‌های بن‌بست به اتاق طلسم
void placeSecretDoors() {
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].type == ROOM_NORMAL && isRoomDeadEnd(rooms[i])) {
            int spellRoomIndex = findClosestSpellRoom(rooms[i].x, rooms[i].y);
            if (spellRoomIndex == -1) continue;  

            Room spellRoom = rooms[spellRoomIndex];
            int secretDoorX = rooms[i].x + rooms[i].width / 2;
            int secretDoorY = rooms[i].y + rooms[i].height / 2;
            map[secretDoorY][secretDoorX] = 'D'; // 'D' = در مخفی

            int targetX = spellRoom.x + spellRoom.width / 2;
            int targetY = spellRoom.y + spellRoom.height / 2;
            map[targetY][targetX] = 'D'; // در مخفی در اتاق طلسم
        }
    }
}*/


int findDeadEndRoom() {
    for (int i = 0; i < roomCount; i++) {
        int doorCount = 0;

        for (int y = rooms[i].y; y < rooms[i].y + rooms[i].height; y++) {
            for (int x = rooms[i].x; x < rooms[i].x + rooms[i].width; x++) {
                if (map[y][x] == '+') { 
                    doorCount++;
                }
            }
        }

        if (doorCount == 1) {
            return i;
        }
    }

    return -1;
}

void convertEntryRoomToSpellRoom() {
    for (int i = 0; i < roomCount; i++) {
       
        if (prevStairX >= rooms[i].x && prevStairX < rooms[i].x + rooms[i].width &&
            prevStairY >= rooms[i].y && prevStairY < rooms[i].y + rooms[i].height) {
            rooms[i].type = ROOM_SPELL;  
            return;
        }
    }
}

void setupFirstFloor() {
    
    for (int i = 0; i < roomCount; i++) {
        rooms[i].type = ROOM_NORMAL;
    }

    int spellRoomIndex = findDeadEndRoom();
    if (spellRoomIndex != -1) {
        rooms[spellRoomIndex].type = ROOM_SPELL;
    }
}

int start() {
    setlocale(LC_ALL,"C.UTF-8");
    start:
    srand(time(0)); 

   
    MAP_WIDTH = MIN_MAP_WIDTH + rand() % (MAX_MAP_WIDTH - MIN_MAP_WIDTH + 1);
    MAP_HEIGHT = MIN_MAP_HEIGHT + rand() % (MAX_MAP_HEIGHT - MIN_MAP_HEIGHT + 1);
    roomCount = 0;
    connectionCount = 0;
    memset(map, ' ', sizeof(map));
    memset(adjacencyMatrix, 0, sizeof(adjacencyMatrix));
    initializeMap();


    int numRooms = MIN_ROOMS + rand() % (MAX_ROOMS - MIN_ROOMS + 1);
    while (roomCount < numRooms) {
        int width = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
        int height = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
        int x = rand() % (MAP_WIDTH - width - 1) + 1;
        int y = rand() % (MAP_HEIGHT - height - 1);

        if (!checkRoomOverlap(x, y, width, height)) {
            addRoom(x, y, width, height);
        }
    }
    placeStaircase('^');
    placeSpells();
    placeTraps();
    
    float distanceMatrix[roomCount][roomCount];
    for (int i = 0; i < roomCount; i++) {
        for (int j = 0; j < roomCount; j++) {
            if (i == j) {
                distanceMatrix[i][j] = 0;
            } else {
                distanceMatrix[i][j] = sqrt(pow(rooms[i].x - rooms[j].x, 2) + pow(rooms[i].y - rooms[j].y, 2));
            }
        }
    }

    
    bool visited[roomCount];
    memset(visited, false, sizeof(visited));
    visited[0] = true;

    for (int edges = 0; edges < roomCount - 1; edges++) {
        float minDistance = 1000;
        int from = -1, to = -1;

        for (int i = 0; i < roomCount; i++) {
            if (visited[i]) {
                for (int j = 0; j < roomCount; j++) {
                    if (!visited[j] && distanceMatrix[i][j] < minDistance) {
                        minDistance = distanceMatrix[i][j];
                        from = i;
                        to = j;
                    }
                }
            }
        }

        if (from != -1 && to != -1) {
            connections[connectionCount++] = (Connection){from, to};
            visited[to] = true;
        }
    }

    
    for (int i = 0; i < connectionCount; i++) {
        int from = connections[i].from;
        int to = connections[i].to;

        
        if (!connectRooms(from, to)) {
            goto start; 
        }
    }

    
    playerX = rooms[0].x + 1;
    playerY = rooms[0].y + 1;
    map[playerY][playerX] = 'p';
    placepiller(); 
    
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(2, COLOR_WHITE, COLOR_BLACK); 
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK); 
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK); 
    init_pair(6, COLOR_GREEN, COLOR_BLACK); 
    memset(discovered, 0, sizeof(discovered)); 
    revealCurrentRoom(); 
    setupFirstFloor();
    placeFood();
    printMapNcurses();

    char input;
    while ((input = getch()) != 'q') { 
        movePlayer(input);
        increaseHunger() ;
        displayPlayerStats();
    }

    endwin();
    return 0;
}


void initializeMap() 
{
    for (int i = 0; i < MAP_HEIGHT; i++) 
    {
        for (int j = 0; j < MAP_WIDTH; j++) 
        {
            if (i == 0 || i == MAP_HEIGHT - 1) 
            {
                map[i][j] = '-'; 
            } 
            else if (j == 0 || j == MAP_WIDTH - 1)
            {
                map[i][j] = '|'; 
            } 
            else 
            {
                map[i][j] = ' '; 
            }
        }
    }
}

int checkRoomOverlap(int x, int y, int width, int height) 
{
    for (int i = y - 1; i <= y + height; i++) {
        for (int j = x - 1; j <= x + width; j++) {
            if (i < 0 || i >= MAP_HEIGHT || j < 0 || j >= MAP_WIDTH || map[i][j] != ' ')
                return 1;
        }
    }
    return 0;
}

void addRoom(int x, int y, int width, int height) {

    rooms[roomCount++] = (Room){x, y, width, height};

    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            if (i == y || i == y + height - 1)
                map[i][j] = '-';
            else if (j == x || j == x + width - 1)
                map[i][j] = '|';
            else
                map[i][j] = '.';
        }
    }
}


int connectRooms(int roomA, int roomB) {
    if (adjacencyMatrix[roomA][roomB]) return 1; 

    int x1 = rooms[roomA].x + rooms[roomA].width / 2;
    int y1 = rooms[roomA].y + rooms[roomA].height / 2;
    int x2 = rooms[roomB].x + rooms[roomB].width / 2;
    int y2 = rooms[roomB].y + rooms[roomB].height / 2;

    int doorPlacedRoomA = 0;
    int doorPlacedRoomB = 0;
    int stuckCounter = 0;

    while (x1 != x2 || y1 != y2) {
        if (stuckCounter++ > 1000) {
            return 0; 
        }

        int prevX = x1, prevY = y1;
        if (rand() % 2) {
            if (x1 != x2) x1 += (x2 > x1) ? 1 : -1;
        } else {
            if (y1 != y2) y1 += (y2 > y1) ? 1 : -1;
        }

        if (map[y1][x1] == '|' || map[y1][x1] == '-') {
            if (!doorPlacedRoomA &&
                x1 >= rooms[roomA].x && x1 < rooms[roomA].x + rooms[roomA].width &&
                y1 >= rooms[roomA].y && y1 < rooms[roomA].y + rooms[roomA].height) {
                map[y1][x1] = '+';
                doorPlacedRoomA = 1; 
            } else if (!doorPlacedRoomB &&
                x1 >= rooms[roomB].x && x1 < rooms[roomB].x + rooms[roomB].width &&
                y1 >= rooms[roomB].y && y1 < rooms[roomB].y + rooms[roomB].height) {
                    map[y1][x1] = '+';
                doorPlacedRoomB = 1; 
            } else {
                x1 = prevX;
                y1 = prevY;
            }
        } else if (map[y1][x1] == ' ') {
            map[y1][x1] = '#';
        }
    }

    adjacencyMatrix[roomA][roomB] = adjacencyMatrix[roomB][roomA] = 1;
    return 1;
}

void movePlayer(char input) {
    int newX = playerX, newY = playerY;
    revealCurrentRoom(); 

    if (input == 'j') revealCorridor(newX, newY, 0, -1); 
    else if (input == 'k') revealCorridor(newX, newY, 0, 1); 
    else if (input == 'h') revealCorridor(newX, newY, -1, 0); 
    else if (input == 'l') revealCorridor(newX, newY, 1, 0); 
    else if (input == 'y') revealCorridor(newX, newY, -1, -1); 
    else if (input == 'u') revealCorridor(newX, newY, 1, -1); 
    else if (input == 'b') revealCorridor(newX, newY, -1, 1); 
    else if (input == 'n') revealCorridor(newX, newY, 1, 1); 
    else if (input == 'r' || input == 'R') { 
        showFullMap = 1; 
        printMapNcurses(); 
    }
    else if (input == 't' || input == 'T') { 
        showFullMap = 0; 
        printMapNcurses(); 
    }
    if (input == 'j') newY--;                 
    else if (input == 'k') newY++;            
    else if (input == 'h') newX--;            
    else if (input == 'l') newX++;            
    else if (input== 'y') { newX--; newY--; } 
    else if (input == 'u') { newX++; newY--; } 
    else if (input == 'b') { newX--; newY++; } 
    else if (input == 'n') { newX++; newY++; } 

    if (input == 's') saveGame();
    char nextCell = map[newY][newX];


    if (nextCell == '.' || nextCell == '#' || nextCell == '+' || 
        nextCell == 'H' || nextCell == 'S' || nextCell == 'D') {
        
        if (map[playerY][playerX] == 'p') {
            map[playerY][playerX] = prevCell;  
        }
        if (playerX == prevStairX && playerY == prevStairY) {
            map[playerY][playerX] = '<'; 
        }

        
        prevCell = nextCell;
        playerX = newX;
        playerY = newY;
        map[playerY][playerX] = 'p';

        
        if (nextCell == 'H' || nextCell == 'S' || nextCell == 'D') {
            mvprintw(MAP_HEIGHT + 1, 0, "You found a %c spell! Press Enter to collect.", nextCell);
            refresh();

            char spellInput = getch();
            if (spellInput == '\n') {
                if (nextCell == 'H') collectedHealth++;
                else if (nextCell == 'S') collectedSpeed++;
                else if (nextCell == 'D') collectedDamage++;
                map[playerY][playerX] = '.';
                prevCell = '.';  
            }
        } else {
            
            mvprintw(MAP_HEIGHT + 1, 0, "                                                ");
            refresh();
        }

        printMapNcurses();
    }
    
    else if (nextCell == '^') {
        map[playerY][playerX] = '<';
        printNewMap();
    }
}

void createLockedDoor() {
    while (1) {
        
        int roomIndex = rand() % roomCount;
        Room selectedRoom = rooms[roomIndex];

        
        int doorCount = 0;
        int doorPositions[100][2]; 
        for (int i = selectedRoom.y; i < selectedRoom.y + selectedRoom.height; i++) {
            for (int j = selectedRoom.x; j < selectedRoom.x + selectedRoom.width; j++) {
                if (map[i][j] == '+') {
                    doorPositions[doorCount][0] = i; 
                    doorPositions[doorCount][1] = j; 
                    doorCount++;
                }
            }
        }

        
        if (doorCount == 1) {
           
            bool playerInside = playerX >= selectedRoom.x && playerX < selectedRoom.x + selectedRoom.width &&
                                playerY >= selectedRoom.y && playerY < selectedRoom.y + selectedRoom.height;

            if (playerInside) {
                
                int lockedDoorY = doorPositions[0][0];
                int lockedDoorX = doorPositions[0][1];
                map[lockedDoorY][lockedDoorX] = '@'; 
                return; 
            } else {
                continue;
            }
        }

        
        if (doorCount > 1) {
            bool playerInside = playerX >= selectedRoom.x && playerX < selectedRoom.x + selectedRoom.width &&
                                playerY >= selectedRoom.y && playerY < selectedRoom.y + selectedRoom.height;

            if (playerInside) {
               
                int lockedDoorIndex = rand() % doorCount;
                int lockedDoorY = doorPositions[lockedDoorIndex][0];
                int lockedDoorX = doorPositions[lockedDoorIndex][1];
                map[lockedDoorY][lockedDoorX] = '@';
                return; 
            } else {
                
                int closestDoorIndex = -1;
                float closestDistance = 1e9;

                
                for (int i = 0; i < doorCount; i++) {int doorY = doorPositions[i][0];
                int doorX = doorPositions[i][1];
                    float distance = sqrt(pow(playerX - doorX, 2) + pow(playerY - doorY, 2));
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestDoorIndex = i;
                    }
                }

                
                for (int i = 0; i < doorCount; i++) {
                    if (i != closestDoorIndex) {
                        int lockedDoorY = doorPositions[i][0];
                        int lockedDoorX = doorPositions[i][1];
                        map[lockedDoorY][lockedDoorX] = '@'; 
                        return; 
                    }
                }
            }
        }
    }
}

void placeStaircase(char staircaseSymbol) {
    int roomIndex = rand() % roomCount;
    Room selectedRoom = rooms[roomIndex];

    int stairX = selectedRoom.x + 1 + rand() % (selectedRoom.width - 2); 
    int stairY = selectedRoom.y + 1 + rand() % (selectedRoom.height - 2); 

    map[stairY][stairX] = staircaseSymbol;

}

void printNewMap() {
    generateNewFloor(++currentFloor); 

    clear();

    printMapNcurses();
}

void convertTwoRoomsToTreasure() {
    if (currentFloor != 3) return;  

    mvprintw(MAP_HEIGHT + 2, 0, "Converting rooms to treasure...");
    refresh();
    getch(); 

    int treasureRooms = 0;
    while (treasureRooms < 2) {
        int randomIndex = rand() % roomCount;

        
        if (rooms[randomIndex].type == ROOM_NORMAL) {
            rooms[randomIndex].type = ROOM_TREASURE;
            treasureRooms++;
        }
    }
}

void generateNewFloor(int floorNumber) {
   
    MAP_WIDTH = MIN_MAP_WIDTH + rand() % (MAX_MAP_WIDTH - MIN_MAP_WIDTH + 1);
    MAP_HEIGHT = MIN_MAP_HEIGHT + rand() % (MAX_MAP_HEIGHT - MIN_MAP_HEIGHT + 1);

    
    roomCount = 0;
    connectionCount = 0;
    memset(map, ' ', sizeof(map));
    memset(discovered, 0, sizeof(discovered)); 
    memset(adjacencyMatrix, 0, sizeof(adjacencyMatrix));
    initializeMap();

    if (floorNumber > 0) {
        
        int prevRoomX = playerX - 3; 
        int prevRoomY = playerY - 3;
        int prevRoomWidth = 7;
        int prevRoomHeight = 7;

        
        if (prevRoomX < 1) prevRoomX = 1;
        if (prevRoomY < 1) prevRoomY = 1;
        if (prevRoomX + prevRoomWidth >= MAP_WIDTH) prevRoomX = MAP_WIDTH - prevRoomWidth - 1;
        if (prevRoomY + prevRoomHeight >= MAP_HEIGHT) prevRoomY = MAP_HEIGHT - prevRoomHeight - 1;

       
        addRoom(prevRoomX, prevRoomY, prevRoomWidth, prevRoomHeight);

        
        prevStairX = playerX;
        prevStairY = playerY;
        
        map[playerY][playerX] = 'p'; 
          
    }

    
    int numRooms = MIN_ROOMS + rand() % (MAX_ROOMS - MIN_ROOMS + 1);
    while (roomCount < numRooms) {
        int width = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
        int height = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
        int x = rand() % (MAP_WIDTH - width - 1) + 1;
        int y = rand() % (MAP_HEIGHT - height - 1);

        if (!checkRoomOverlap(x, y, width, height)) {
            addRoom(x, y, width, height);
        }
    }

    
    if (floorNumber < 3) {
        placeStaircase('^'); 
    }
    placeSpells();

   
    float distanceMatrix[roomCount][roomCount];
    for (int i = 0; i < roomCount; i++) {
        for (int j = 0; j < roomCount; j++) {
            if (i == j) {
                distanceMatrix[i][j] = 0;
            } else {
                distanceMatrix[i][j] = sqrt(pow(rooms[i].x - rooms[j].x, 2) + pow(rooms[i].y - rooms[j].y, 2));
            }
        }
    }

    
    bool visited[roomCount];
    memset(visited, false, sizeof(visited));
    visited[0] = true;

    for (int edges = 0; edges < roomCount - 1; edges++) {
        float minDistance = 1000;
        int from = -1, to = -1;

        for (int i = 0; i < roomCount; i++) {
            if (visited[i]) {
                for (int j = 0; j < roomCount; j++) {
                    if (!visited[j] && distanceMatrix[i][j] < minDistance) {
                        minDistance = distanceMatrix[i][j];
                        from = i;
                        to = j;
                    }
                }
            }
        }

        if (from != -1 && to != -1) {
            connections[connectionCount++] = (Connection){from, to};
            visited[to] = true;
        }
    }

    
    for (int i = 0; i < connectionCount; i++) {
        int from = connections[i].from;
        int to = connections[i].to;

        if (!connectRooms(from, to)) {
            generateNewFloor(floorNumber); 
            return;
        }
    }

    setupFirstFloor();
    convertEntryRoomToSpellRoom();
    convertTwoRoomsToTreasure();


}


void revealCorridor(int startX, int startY, int dx, int dy) {
    int queue[MAP_WIDTH * MAP_HEIGHT][2]; 
    int front = 0, rear = 0;
    int distance[MAP_HEIGHT][MAP_WIDTH]; 

    memset(distance, -1, sizeof(distance)); 

    queue[rear][0] = startX;
    queue[rear][1] = startY;
    distance[startY][startX] = 0;
    rear++;

    while (front < rear) {
        int x = queue[front][0];
        int y = queue[front][1];
        int dist = distance[y][x];
        front++;

        if (dist >= CORRIDOR_VISIBLE) continue; 

        discovered[y][x] = 1;

        
        int nx = x + dx;
        int ny = y + dy;

        if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
            if ((map[ny][nx] == '#' || map[ny][nx] == '+') && distance[ny][nx] == -1) {
                distance[ny][nx] = dist + 1;
                queue[rear][0] = nx;
                queue[rear][1] = ny;
                rear++;
            }
        }

        
        int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (int d = 0; d < 4; d++) {
            if (dirs[d][0] == dx && dirs[d][1] == dy) continue; 

            int sideX = x + dirs[d][0];
            int sideY = y + dirs[d][1];

            if (sideX >= 0 && sideX < MAP_WIDTH && sideY >= 0 && sideY < MAP_HEIGHT) {
                if ((map[sideY][sideX] == '#' || map[sideY][sideX] == '+') && distance[sideY][sideX] == -1) {
                    distance[sideY][sideX] = dist + 1;
                    queue[rear][0] = sideX;
                    queue[rear][1] = sideY;
                    rear++;
                }
            }
        }
    }

    
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (distance[i][j] == -1 && discovered[i][j] == 0) { 
                discovered[i][j] = 0; 
            }
        }
    }
}

void revealCurrentRoom() {
    int roomIndex = findRoomWithDoor();
    if (roomIndex == -1) return; 

    Room currentRoom = rooms[roomIndex];

    for (int y = currentRoom.y; y < currentRoom.y + currentRoom.height; y++) {
        for (int x = currentRoom.x; x < currentRoom.x + currentRoom.width; x++) {
            discovered[y][x] = 1; 
        }
    }
}

int findRoomWithDoor() {
    for (int i = 0; i < roomCount; i++) {
        for (int y = rooms[i].y; y < rooms[i].y + rooms[i].height; y++) {
            for (int x = rooms[i].x; x < rooms[i].x + rooms[i].width; x++) {
                if (map[y][x] == 'p') {
                    return i; 
                }
            }
        }
    }
    return -1; 
}

void placeAmpersand(Room room) {
    int ampersandX = room.x + room.width - 2; 
    int ampersandY = room.y + room.height - 2;

    map[ampersandY][ampersandX] = '&'; 
}

void placepiller()
{
    for(int i=0;i<1+rand()%roomCount;i++){
        int randomRoomIndex = rand() % roomCount;    
        Room targetRoom = rooms[randomRoomIndex];  
    int mX = targetRoom.x + 1 + rand() % (targetRoom.width - 2);    
    int mY = targetRoom.y + 1 + rand() % (targetRoom.height - 2);    
    map[mY][mX] = 'O';}
}

void placeTraps() {
    for (int i = 0; i < roomCount; i++) {
        if (rand() % 10 < 1) {
            Room room = rooms[i];
            int trapX = room.x + 1 + rand() % (room.width - 2);
            int trapY = room.y + 1 + rand() % (room.height - 2);
            map[trapY][trapX] = 'T';  
        }
    }
}

/*void handleAmpersandInteraction(int playerX, int playerY) {
    
    if (map[playerY][playerX] == '&') {
        
        int password = rand() % 9000 + 1000;

       
        mvprintw(MAP_HEIGHT + 1, 0, "Password: %d", password);
        refresh();

        sleep(30);

        mvprintw(MAP_HEIGHT + 1, 0, "                     "); 
        refresh();
    }
}*/




void placeFood() {
    foodCount = MIN_FOOD + rand() % (MAX_FOOD - MIN_FOOD + 1); 

    char foodTypes[] = {'f', 'F', '*', '?'}; 
    for (int i = 0; i < foodCount; i++) {
        int x, y;
        do {
            x = rand() % MAP_WIDTH;
            y = rand() % MAP_HEIGHT;
        } while (map[y][x] != '.');  

        foods[i].x = x;
        foods[i].y = y;
        foods[i].symbol = foodTypes[rand() % 4]; 

        map[y][x] = foods[i].symbol; 
    }
}


void displayPlayerStats() {
    int barLength = 20; 
    int filledHealth = (playerHealth * barLength) / MAX_HEALTH;
    int filledHunger = (hungerLevel * barLength) / 100;

    
    mvprintw(MAP_HEIGHT + 1, 0, "Health: [");
    attron(COLOR_PAIR(6)); 
    for (int i = 0; i < filledHealth; i++) printw("#");
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(5)); 
    for (int i = filledHealth; i < barLength; i++) printw("-");
    attroff(COLOR_PAIR(5));
    printw("] %d%%", playerHealth);

    mvprintw(MAP_HEIGHT + 2, 0, "Hunger: [");
    attron(COLOR_PAIR(5)); 
    for (int i = 0; i < filledHunger; i++) printw("#");
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(2)); 
    for (int i = filledHunger; i < barLength; i++) printw("-");
    attroff(COLOR_PAIR(2));
    printw("] %d%%", hungerLevel);

    refresh();
}


void displayGold() {
    int centerX = MAP_WIDTH / 2; 
    mvprintw(MAP_HEIGHT + 1, centerX - 5, "\xF0\x9F\x92\xB0 Gold: %d", goldAmount);
    refresh();
}

void printMapNcurses() {
    revealCurrentRoom();
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char cell = map[i][j];

            if (!discovered[i][j] && !showFullMap) { 
                mvaddch(i, j, ' '); 
                continue;
            }

            
            RoomType roomType = ROOM_NORMAL;
            for (int r = 0; r < roomCount; r++) {
                Room room = rooms[r];
                if (j >= room.x && j < room.x + room.width && i >= room.y && i < room.y + room.height) {
                    roomType = room.type;
                    break;
                }
            }

    if(cell == '#' ) {
    attron(COLOR_PAIR(2));  
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(2));


                int isFood = 0;
            for (int f = 0; f < foodCount; f++) {
                if (foods[f].x == j && foods[f].y == i) {
                    attron(COLOR_PAIR(3));  
                    mvaddch(i, j, foods[f].symbol);
                    attroff(COLOR_PAIR(3));
                    isFood = 1;
                    break;
                }
            }

            if (!isFood) {
                mvaddch(i, j, cell);
            }
} else {
   
    if(roomType == ROOM_NORMAL)
        attron(COLOR_PAIR(2));
    else if(roomType == ROOM_SPELL)
        attron(COLOR_PAIR(3));
    else if(roomType == ROOM_TREASURE)
        attron(COLOR_PAIR(6));
    
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(2));
    attroff(COLOR_PAIR(3));
    attroff(COLOR_PAIR(6));
}
        }
        
    }
    
    displayGold();
    displayPlayerStats();
    refresh();
}




int doesUserExist(const char *username) {
    FILE *file = fopen("users.txt", "r");
    if (!file) return 0;

    char line[256], storedUsername[100];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,]", storedUsername);
        if (strcmp(storedUsername, username) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}


void saveNewUser(const char *username, const char *password, const char *email) {
    FILE *file = fopen("users.txt", "a");
    if (!file) {
        mvprintw(20, 15, "Error: Unable to save user data!");
        refresh();
        return;
    }
    fprintf(file, "%s,%s,%s\n", username, password, email);
    fclose(file);
}


int isPasswordValid(const char *password) {
    if (strlen(password) < 7) return 0;

    int hasUpper = 0, hasLower = 0, hasDigit = 0;
    for (int i = 0; password[i]; i++) {
        if (isupper(password[i])) hasUpper = 1;
        if (islower(password[i])) hasLower = 1;
        if (isdigit(password[i])) hasDigit = 1;
    }

    return hasUpper && hasLower && hasDigit;
}

int isEmailValid(const char *email) {
    const char *at = strchr(email, '@');  
    const char *dot = strrchr(email, '.');  
    return (at && dot && (dot > at) && (at != email) && (*(dot + 1) != '\0'));
}


void generateRandomPassword(char *password, size_t length) {
    const char lower[] = "abcdefghijklmnopqrstuvwxyz";
    const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char digits[] = "0123456789";
    const char symbols[] = "!@#$%^&*";
    const char allChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    size_t lowerSize = strlen(lower);
    size_t upperSize = strlen(upper);
    size_t digitSize = strlen(digits);
    size_t symbolSize = strlen(symbols);
    size_t allSize = strlen(allChars);

    srand(time(NULL)); 

    
    password[0] = lower[rand() % lowerSize];  
    password[1] = upper[rand() % upperSize];  
    password[2] = digits[rand() % digitSize]; 
    password[3] = symbols[rand() % symbolSize]; 

    for (size_t i = 4; i < length; i++) {
        password[i] = allChars[rand() % allSize];
    }

    
    for (size_t i = 0; i < length; i++) {
        size_t j = rand() % length;
        char temp = password[i];
        password[i] = password[j];
        password[j] = temp;
    }

    password[length] = '\0';
}

void createNewUser() {
    char username[100] = "";
    char password[100] = "";
    char email[100] = "";

    int currentField = 0;  
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    while (1) {
        clear();

        
        attron(A_BOLD);
        mvprintw(1, 15, "===============================");
        mvprintw(2, 20, "CREATE NEW USER");
        mvprintw(3, 15, "===============================");
        attroff(A_BOLD);

        
        for (int i = 0; i < 3; i++) {
            int y = 7 + i * 3; 
            move(y, 15);
            addch(ACS_ULCORNER);
            for (int j = 0; j < 30; j++) addch(ACS_HLINE);
            addch(ACS_URCORNER);
            mvaddch(y + 1, 15, ACS_VLINE);
            for (int j = 0; j < 30; j++) printw(" ");
            mvaddch(y + 1, 45, ACS_VLINE);
            move(y + 2, 15);
            addch(ACS_LLCORNER);
            for (int j = 0; j < 30; j++) addch(ACS_HLINE);
            addch(ACS_LRCORNER);
        }

       
        mvprintw(6, 16, "Username:");
        mvprintw(9, 16, "Password:");
        mvprintw(12, 16, "Email:");

        
        mvprintw(8, 16, "%-29s", username);
        mvprintw(11, 16, "%-29s", password[0] ? password : "");
        mvprintw(14, 16, "%-29s", email);

        if (currentField == 3) {
            attron(A_BOLD | COLOR_PAIR(1));
            mvprintw(11, 48, "[Generate Password]");
            attroff(A_BOLD | COLOR_PAIR(1));
        } else {
            mvprintw(11, 48, "[Generate Password]");
        }

        if (currentField == 4) {
            attron(COLOR_PAIR(1));
            mvprintw(18, 20, "[ Submit ]");
            attroff(COLOR_PAIR(1));
        } else {
            mvprintw(18, 20, "[ Submit ]");
        }

        if (currentField == 5) {
            attron(COLOR_PAIR(2));
            mvprintw(18, 35, "[ Cancel ]");
            attroff(COLOR_PAIR(2));
        } else {
            mvprintw(18, 35, "[ Cancel ]");
        }

        refresh();

        if (currentField == 0) move(8, 16 + strlen(username));
        else if (currentField == 1) move(11, 16 + strlen(password)); 
        else if (currentField == 2) move(14, 16 + strlen(email));
        else move(0, 0); 

        ch = getch();

        if (ch == KEY_UP) {
            currentField = (currentField - 1 + 6) % 6;
        } else if (ch == KEY_DOWN) {
            currentField = (currentField + 1) % 6;
        } else if (ch == 10) { 
            if (currentField == 0) { 
                echo();
                mvgetnstr(8, 16, username, sizeof(username) - 1);
                noecho();
                currentField++; 
            } else if (currentField == 1) { 
                echo();
                mvgetnstr(11, 16, password, sizeof(password) - 1);
                noecho();
                currentField++; 
            } else if (currentField == 2) { 
                echo();
                mvgetnstr(14, 16, email, sizeof(email) - 1);
                noecho();
                currentField++; 
            } else if (currentField == 3) { 
                generateRandomPassword(password, 12); 
                mvprintw(19, 15, "Random password generated!");
                refresh();
                getch();
            } else if (currentField == 4) { 
                if (doesUserExist(username)) {
                    mvprintw(19, 15, "Error: Username already exists!");
                } else if (!isPasswordValid(password)) {
                    mvprintw(19, 15, "Error: Password must have 7+ chars, 1 uppercase, 1 lowercase, 1 digit.");
                } else if (!isEmailValid(email)) {
                    mvprintw(19, 15, "Error: Invalid email format!");
                } else {
                    saveNewUser(username, password, email);
                    mvprintw(19, 15, "User created successfully!");
                }
                refresh();
                getch();
                break;
            } else if (currentField == 5) { 
                break;
            }
        } else if (ch == KEY_BACKSPACE || ch == 127) { 
            if (currentField == 0 && strlen(username) > 0) {
                username[strlen(username) - 1] = '\0';
            } else if (currentField == 1 && strlen(password) > 0) {
                password[strlen(password) - 1] = '\0';
            } else if (currentField == 2 && strlen(email) > 0) {
                email[strlen(email) - 1] = '\0';
            }
        } else if (isprint(ch)) { 
            if (currentField == 0 && strlen(username) < sizeof(username) - 1) {
                username[strlen(username)] = ch;
                username[strlen(username) + 1] = '\0';
            } else if (currentField == 1 && strlen(password) < sizeof(password) - 1) {
                password[strlen(password)] = ch;
                password[strlen(password) + 1] = '\0';
            } else if (currentField == 2 && strlen(email) < sizeof(email) - 1) {
                email[strlen(email)] = ch;
                email[strlen(email) + 1] = '\0';
            }
        }
    }

    endwin();
}

void trimNewline(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

int checkCredentials(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        return -1; 
    }

    char line[120]; 
    char storedUsername[30], storedPassword[30], storedEmail[50];

    char inputUsername[30], inputPassword[30];
    strncpy(inputUsername, username, sizeof(inputUsername) - 1);
    strncpy(inputPassword, password, sizeof(inputPassword) - 1);
    inputUsername[sizeof(inputUsername) - 1] = '\0';
    inputPassword[sizeof(inputPassword) - 1] = '\0';

    while (fgets(line, sizeof(line), file)) {
        
        trimNewline(line);

        if (sscanf(line, "%29[^,],%29[^,],%49[^\n]", storedUsername, storedPassword, storedEmail) == 3) {
            trimNewline(storedUsername);
            trimNewline(storedPassword);

            printf("Comparing:\nUser Input: '%s' | File Data: '%s'\n", inputUsername, storedUsername);
            printf("Password Input: '%s' | File Password: '%s'\n", inputPassword, storedPassword);

            if (strcmp(inputUsername, storedUsername) == 0 && strcmp(inputPassword, storedPassword) == 0) {
                fclose(file);
                return 1; 
            }
        }
    }

    fclose(file);
    return 0; 
}


int checkUserEmail(const char *username, const char *email, char *password) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return -1;  
    }

    char storedUser[30], storedEmail[50], storedPass[30];

    while (fscanf(file, "%[^,],%[^,],%[^\n]\n", storedUser, storedPass, storedEmail) != EOF) {
        if (strcmp(username, storedUser) == 0 && strcmp(email, storedEmail) == 0) {
            strcpy(password, storedPass);  
            fclose(file);
            return 1;  
        }
    }

    fclose(file);
    return 0;  
}

void playNewGame() {
    clear();
    start();
    refresh();
    getch();
}

void continueOldGame() {
    clear();
    //loadGame();
    refresh();
    getch();
}

void gameMenu() {
    const char *options[] = {
        "(*) Start New Game",
        "(*) Continue Previous Game"
    };
    const int numOptions = sizeof(options) / sizeof(options[0]);

    int choice = 0;
    int key;

    const int startRow = 8;   
    const int startCol = 10;  
    const int rowSpacing = 2; 

    while (1) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 15, "===============================");
        mvprintw(2, 20, "Game Menu");
        mvprintw(3, 15, "===============================");
        attroff(A_BOLD);
        mvprintw(5, 10, "Select an option:");

        
        for (int i = 0; i < numOptions; i++) {
            if (i == choice) {
                attron(A_REVERSE); 
                mvprintw(startRow + i * rowSpacing, startCol, "%s", options[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(startRow + i * rowSpacing, startCol, "%s", options[i]);
            }
        }

        key = getch(); 

        switch (key) {
            case KEY_UP:   
                choice = (choice - 1 + numOptions) % numOptions;
                break;
            case KEY_DOWN:  
                choice = (choice + 1) % numOptions;
                break;
            case 10: switch (choice) {
                    case 0: playNewGame(); break;
                    case 1: continueOldGame(); break;
                }
                break;
            case 27:        
                return;
        }
    }
}

void loginUser() {
    char username[30] = "";  
    char password[30] = "";  
    int currentField = 0;    
    int key;

    while (1) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 15, "===============================");
        mvprintw(2, 20, "Login Menu");
        mvprintw(3, 15, "===============================");
        attroff(A_BOLD);
        mvprintw(5, 5, "Enter your credentials:");

        
        for (int i = 0; i < 2; i++) {
            int y = 8 + i * 3;
            move(y, 15);
            addch(ACS_ULCORNER);
            for (int j = 0; j < 30; j++) addch(ACS_HLINE);
            addch(ACS_URCORNER);
            mvaddch(y + 1, 15, ACS_VLINE);
            for (int j = 0; j < 30; j++) printw(" ");
            mvaddch(y + 1, 45, ACS_VLINE);
            move(y + 2, 15);
            addch(ACS_LLCORNER);
            for (int j = 0; j < 30; j++) addch(ACS_HLINE);
            addch(ACS_LRCORNER);
        }

        
        mvprintw(7, 16, "Username:");
        mvprintw(10, 16, "Password:");

        
        if (currentField == 0) attron(A_REVERSE);
        mvprintw(9, 16, "%-29s", username);
        attroff(A_REVERSE);

        if (currentField == 1) attron(A_REVERSE);
        mvprintw(12, 16, "%-29s", password);
        attroff(A_REVERSE);

        mvprintw(15, 5, "Press Enter to submit, ESC to return to the menu");

        key = getch();

        switch (key) {
            case KEY_UP:    
                currentField = (currentField - 1 + 2) % 2;
                break;
            case KEY_DOWN:  
                currentField = (currentField + 1) % 2;
                break;
            case 10:        
                if (currentField == 0) {  
                    currentField = 1;    
                } else {
           int result = checkCredentials(username,password);
                    if (result == -1) {
                        mvprintw(16, 5, "Error: Could not open user file!");
                        getch();
                    } else if (result == 0) {
                        mvprintw(16, 5, "Error: Invalid Username or Password!");
                        getch();
                    } else {
                        clear();
                        mvprintw(5, 5, "Login successful!press any key to continue");
                        getch();
                        gameMenu();  
                        return;
                    }
                }
                break;
            case 27: 
                return;
            default: 
                if (currentField == 0 && strlen(username) < 29 && isprint(key)) {
                strcpy(currentUsername, username);
                    int len = strlen(username);
                    username[len] = key;
                    username[len + 1] = '\0';
                } else if (currentField == 1 && strlen(password) < 29 && isprint(key)) {
                    int len = strlen(password);
                    password[len] = key;
                    password[len + 1] = '\0';
                } else if (key == KEY_BACKSPACE || key == 127) { 
                    if (currentField == 0 && strlen(username) > 0) {
                        username[strlen(username) - 1] = '\0';
                    } else if (currentField == 1 && strlen(password) > 0) {
                        password[strlen(password) - 1] = '\0';
                    }
                }
                break;
        }
    }
}

void loginAsGuest() {
    clear();
    start();
    getch();
}

void forgotPassword() {
    char username[30] = "";  
    char email[50] = "";     
    int currentField = 0;    
    int key;

    while (1) {
        clear();

       
        attron(A_BOLD);
        mvprintw(1, 15, "===================================");
        mvprintw(2, 20, "Forgot Password");
        mvprintw(3, 15, "===================================");
        attroff(A_BOLD);
        mvprintw(5, 5, "Enter your username and email:");

        
        for (int i = 0; i < 2; i++) {
            int y = 7 + i * 3; 
            move(y, 15);
            addch(ACS_ULCORNER);
            for (int j = 0; j < 30; j++) addch(ACS_HLINE);
            addch(ACS_URCORNER);
            mvaddch(y + 1, 15, ACS_VLINE);
            for (int j = 0; j < 30; j++) printw(" ");
            mvaddch(y + 1, 45, ACS_VLINE);
            move(y + 2, 15);
            addch(ACS_LLCORNER);
            for (int j = 0; j < 30; j++) addch(ACS_HLINE);
            addch(ACS_LRCORNER);
        }

        
        mvprintw(6, 16, "Username:");
        mvprintw(9, 16, "Email:");

        
        if (currentField == 0) attron(A_REVERSE);
        mvprintw(8, 16, "%-29s", username);
        attroff(A_REVERSE);

        if (currentField == 1) attron(A_REVERSE);
        mvprintw(11, 16, "%-29s", email);
        attroff(A_REVERSE);

       
        mvprintw(14, 5, "Press Enter to submit, ESC to return to the menu");

        
        key = getch();

        switch (key) {
            case KEY_UP:    
                currentField = (currentField - 1 + 2) % 2;
                break;
            case KEY_DOWN:
                currentField = (currentField + 1) % 2;
                break;
            case 10: {      
                char password[30];
                int result = checkUserEmail(username, email, password);
                if (result == -1) {
                    mvprintw(16, 5, "Error: Could not open user file!");
                    getch();
                } else if (result == 0) {
                    mvprintw(16, 5, "Error: Username or email is incorrect!");
                    getch();
                } else {
                    clear();
                    mvprintw(5, 5, "Your password is: %s", password);
                    getch();
                    return;
                }
                break;
            }
            case 27: 
                return;
            default: 
                if (currentField == 0 && strlen(username) < 29 && isprint(key)) {
                    int len = strlen(username);
                    username[len] = key;
                    username[len + 1] = '\0';
                } else if (currentField == 1 && strlen(email) < 49 && isprint(key)) {
                    int len = strlen(email);
                    email[len] = key;
                    email[len + 1] = '\0';
                } else if (key == KEY_BACKSPACE || key == 127) { 
                    if (currentField == 0 && strlen(username) > 0) {
                        username[strlen(username) - 1] = '\0';
                    } else if (currentField == 1 && strlen(email) > 0) {
                        email[strlen(email) - 1] = '\0';
                    }
                }
                break;
        }
    }
}


void loginMenu() {
    const char *options[] = {
        "(*) Login",        
        "(*) Guest Login",   
        "(*) Forgot Password" 
    };
    const int numOptions = sizeof(options) / sizeof(options[0]);

    int choice = 0; 
    int key;

    const int startRow = 8;   
    const int startCol = 10;  
    const int rowSpacing = 2; 

    while (1) {
        clear();
            attron(A_BOLD);
        mvprintw(1, 15, "===============================");
        mvprintw(2, 20, "Login Menu");
        mvprintw(3, 15, "===============================");
        attroff(A_BOLD);
        mvprintw(5, 10, "Select an option:");

        
        for (int i = 0; i < numOptions; i++) {
            if (i == choice) {
                attron(A_REVERSE); 
                mvprintw(startRow + i * rowSpacing, startCol, "%s", options[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(startRow + i * rowSpacing, startCol, "%s", options[i]);
            }
        }

        key = getch(); 

        switch (key) {
            case KEY_UP:    
                choice = (choice - 1 + numOptions) % numOptions;
                break;
            case KEY_DOWN:  
                choice = (choice + 1) % numOptions;
                break;
            case 10:        
                switch (choice) {
                    case 0: loginUser(); break;     
                    case 1: loginAsGuest(); break;    
                    case 2: forgotPassword(); break;  
                }
                break;
            case 27:       
                return;
        }
    }
}


void playGame() {
    clear();
    mvprintw(5, 5, "%s", "This section will be generated soon: Play Game");
    refresh();
    getch();
}

void showProfile() {
    clear();
    mvprintw(5, 5, "%s", "This section will be generated soon: Profile");
    refresh();
    getch();
}

void showLeaderboard() {
    clear();
    mvprintw(5, 5, "%s", "This section will be generated soon: Leaderboard");
    refresh();
    getch();
}

void settingsMenu() {
    clear();
    mvprintw(5, 5, "%s", "This section will be generated soon: Settings");
    refresh();
    getch();
}

void exitGame() {
    clear();
    mvprintw(5, 5, "%s", "Exiting the game...");
    refresh();
    getch();
}

void mainMenu() {
    const char *options[] = {
        "1. Create New User",
        "2. Login User",
        "3. Profile",
        "4. Leaderboard",
        "5. Settings",
        "6. Exit"
    };
    const int numOptions = sizeof(options) / sizeof(options[0]);

    int choice = 0; 
    int key;

    const int startRow = 3;   
    const int startCol = 10;  
    const int rowSpacing = 1; 

    while (1) {
        clear();
        mvprintw(1, 5, "%s", "Main Menu:");
        for (int i = 0; i < numOptions; i++) {
            if (i == choice) {
                attron(A_REVERSE); 
                mvprintw(startRow + i * rowSpacing, startCol, "%s", options[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(startRow + i * rowSpacing, startCol, "%s", options[i]);
            }
        }

        key = getch(); 

        switch (key) {
            case KEY_UP:    
                choice = (choice - 1 + numOptions) % numOptions;
                break;
            case KEY_DOWN:  
                choice = (choice + 1) % numOptions;
                break;
            case 10:        
                switch (choice) {
                    case 0: createNewUser(); break;
                    case 1: loginMenu(); break;
                    case 2: showProfile(); break;
                    case 3: showLeaderboard(); break;
                    case 4: settingsMenu(); break;
                    case 5: exitGame(); return;
                }
                break;
        }
    }
}

