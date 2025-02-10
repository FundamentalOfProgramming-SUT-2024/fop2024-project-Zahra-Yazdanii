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
#include <wchar.h>

#define MAP_WIDTH 110
#define MAP_HEIGHT 40
#define MIN_ROOM_SIZE 10
#define MAX_ROOM_SIZE 15
#define MIN_ROOMS 6
#define MAX_ROOMS 8
#define MAX_HEALTH 100
#define COLOR_NORMAL_ROOM 1
#define COLOR_SPELL_ROOM 2
#define COLOR_TREASURE_ROOM 3
#define CORRIDOR_VISIBLE 5 
#define MAX_SPELLS 10  
#define MAX_WEAPONS 20  
#define MAX_MONSTERS 10
#define MAX_PLAYERS 100
#define MAX_MONSTERS 10  


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
    char username[30];
    int gold;
    int score;
    int gamesPlayed;
    time_t firstGameTime;
} Player;
Player players[MAX_PLAYERS];

typedef struct {
    char symbol;  
    int x, y;     
    int health;   
    int chaseRange;
    bool active;  
} Monster;
Monster monsters[MAX_MONSTERS];  

typedef enum {
    FOOD_NORMAL = 1,  
    FOOD_SUPER = 2,   
    FOOD_MAGIC = 3,   
    FOOD_SPOILED = 4  
} FoodType;

typedef struct {
    int x, y;         
    FoodType type;    
    char symbol;      
} Food;

Food foods[60];  


typedef enum {
    WEAPON_MACE,      
    WEAPON_DAGGER,    
    WEAPON_SWORD,     
    WEAPON_WAND,      
    WEAPON_ARROW      
} WeaponType;

WeaponType equippedWeapon = WEAPON_MACE;

typedef struct {
    int x, y;         
    WeaponType type;  
    char symbol;     
    int damage;      
    int range;        
    bool isThrowable; 
    //int quantity;   
    bool droppedByPlayer;  
} Weapon;
Weapon weapons[MAX_WEAPONS];  

typedef struct {
    int roomCount;
    Room rooms[100];         
    char map[120][120];      
    int discovered[120][120];
    int playerX, playerY;    
    int playerHealth;        
    int hungerLevel;         
    int goldAmount;          
    int monsterCount;
    Monster monsters[MAX_MONSTERS];
    int foodCount;
    Food foods[60];

    int weaponCount;
    Weapon weapons[MAX_WEAPONS];

} GameState;

GameState state;



int playerCount = 0;
int connectionCount = 0;
int doorCount = 0;
int buttonCount = 0;
int currentFloor = 0;
wchar_t map[120][120];
int discovered[120][120] = {0}; 
int showFullMap = 0; 
int adjacencyMatrix[120][120]; 
//int MAP_HEIGHT,MAP_WIDTH;
int playerX;
int playerY;
char prevCell = '.';
int prevStairX = -1; 
int prevStairY = -1; 
int collectedHealth = 0;  
int collectedSpeed = 0;   
int collectedDamage = 0;
int collectedMACE = 1;
int collectedDAGGER = 0;
int collectedSWORD = 0;
int collectedWAND= 0 ;
int collectedARROW =0;
int playerHealth = 100;
int goldAmount=0;
int hungerLevel = 0;  
int moveCounter = 0;
int foodCount = 0;  
char currentUsername[50] = ""; 
bool visitedRoomsindex[100] = {false}; 
int currentRoomIndex = -1;
char itemUnderPlayer = '.';
int collectedNORMAL = 0;
int collectedSUPER = 0;
int collectedMAGIC = 0;
int collectedROTTON =0;
int playerColor = 1;  
int monsterCount = 0;


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
void placeFood();
void updatePlayerStats();
void showLeaderboard();
#define GOLD_SYMBOL '$'  
#define BLACK_GOLD_SYMBOL '*' 
#define MAX_DISPLAY 3





char foodSymbols[] = {'1', '2', '3', '1'};  




int weaponInventory[5] = {1, 0, 0, 0, 0};  

int weaponCount = 0;  

int dx[8] = {0,  0, -1, 1, -1,  1, -1, 1};
int dy[8] = {-1, 1,  0, 0, -1, -1,  1, 1};

int findRoomIndex(int x, int y) {
    for (int i = 0; i < roomCount; i++) {
        if (x >= rooms[i].x && x < rooms[i].x + rooms[i].width &&
            y >= rooms[i].y && y < rooms[i].y + rooms[i].height) {
            return i;
        }
    }
    return -1;  
}

void showMessage(char *message) {
    int startX = 114;  
    int startY = 5;    
    int boxWidth = 15; 
    int maxLines = 10; 
    
    /*for (int i = startY; i < startY + maxLines; i++) {
        mvprintw(i, startX, "%-*s", boxWidth, " ");
    }*/

      

    int line = 0;
    int messageLength = strlen(message);
    for (int i = 0; i < messageLength && line < maxLines; i += boxWidth) {
        char temp[boxWidth + 1];  
        strncpy(temp, message + i, boxWidth);
        temp[boxWidth] = '\0'; 

        mvprintw(startY + line, startX, "%-*s", boxWidth, temp);
        line++;
    }
    

    refresh();

    sleep(1);
    for (int i = startY; i < startY + maxLines; i++) {
        mvprintw(i, startX, "%-*s", boxWidth, " ");
    }
    refresh();
}

void drawStatusBox() {
    int startY = 40;  
    int startX = 0;
    int width = 131;   
    int height = 6;   
       attron(COLOR_PAIR(4));

    move(startY, startX);
    addch(ACS_ULCORNER);
    for (int j = 0; j < width - 2; j++) addch(ACS_HLINE);
    addch(ACS_URCORNER);

    
    for (int y = startY + 1; y < startY + height - 1; y++) {
        mvaddch(y, startX, ACS_VLINE);
        for (int j = 0; j < width - 2; j++) printw(" ");
        mvaddch(y, startX + width - 1, ACS_VLINE);
    }

    
    move(startY + height - 1, startX);
    addch(ACS_LLCORNER);
    for (int j = 0; j < width - 2; j++) addch(ACS_HLINE);
    addch(ACS_LRCORNER);
    attroff(COLOR_PAIR(4));

    refresh();
}

void drawVerticalMessageBox() {
    int startX = 112;   
    int startY = 0;    
    int width = 19;    
    int height = 40;   

    attron(COLOR_PAIR(4));
    move(startY, startX);
    addch(ACS_ULCORNER);
    for (int j = 0; j < width - 2; j++) addch(ACS_HLINE);
    addch(ACS_URCORNER);

    
    for (int y = startY + 1; y < startY + height - 1; y++) {
        mvaddch(y, startX, ACS_VLINE);
        for (int j = 0; j < width - 2; j++) printw(" ");
        mvaddch(y, startX + width - 1, ACS_VLINE);
    }

    
    move(startY + height - 1, startX);
    addch(ACS_LLCORNER);
    for (int j = 0; j < width - 2; j++) addch(ACS_HLINE);
    addch(ACS_LRCORNER);

        attron(A_BOLD);
        mvprintw(1, 114, "===============");
        mvprintw(2, 118, "MESSAGE");
        mvprintw(3, 114, "===============");
        attroff(A_BOLD);
    attroff(COLOR_PAIR(4));
    refresh();
}

void initializeWeapons() {
    weapons[WEAPON_MACE]   = (Weapon){0, 0, WEAPON_MACE,   'M',  5,  0, false, false};
    weapons[WEAPON_DAGGER] = (Weapon){0, 0, WEAPON_DAGGER, 'z', 12,  5, true,  false};
    weapons[WEAPON_SWORD]  = (Weapon){0, 0, WEAPON_SWORD,  'v', 10, 0, false, false};
    weapons[WEAPON_WAND]   = (Weapon){0, 0, WEAPON_WAND,   'W', 15, 10, true,  false};
    weapons[WEAPON_ARROW]  = (Weapon){0, 0, WEAPON_ARROW,  'A', 5,  5, true,  false};
}

char weaponSymbols[] = {'M', 'z', 'v', 'W', 'A'};  

int getMonsterMaxHealth(char type) {
    switch (type) {
        case 'D': return 5;   // Demon
        case 'F': return 10;  // Fire
        case 'G': return 15;  // Giant
        case 'S': return 20;  // Snake
        case 'U': return 30;  // Undead
        default: return 1;    
    }
}

void displayMonsterHealth(int monsterIndex) {
    int barLength = 20; 
    int maxHealth = getMonsterMaxHealth(monsters[monsterIndex].symbol); 
    int filledHealth = (monsters[monsterIndex].health * barLength) / maxHealth;

    
    mvprintw(MAP_HEIGHT + 2, 80, "Monster %c Health: [", monsters[monsterIndex].symbol);  

    
    attron(COLOR_PAIR(6)); 
    for (int i = 0; i < filledHealth; i++) {
        printw("#");
    }
    attroff(COLOR_PAIR(6)); 
    
    attron(COLOR_PAIR(5)); 
    for (int i = filledHealth; i < barLength; i++) {
        printw("-");
    }
    attroff(COLOR_PAIR(5)); 

    
    printw("] %d/%d", monsters[monsterIndex].health,maxHealth); 

    refresh();
}

bool isMonsterAt(int x, int y) {
    for (int i = 0; i < monsterCount; i++) {
        if (monsters[i].x == x && monsters[i].y == y) {
            return true;
        }
    }
    return false;
}

int getMonsterAt(int x, int y) {
    for (int i = 0; i < monsterCount; i++) {
        if (monsters[i].x == x && monsters[i].y == y) {
            return i;  
        }
    }
    return -1;  
}

void attackMelee() {
    int damage = (equippedWeapon == WEAPON_MACE) ? 5 : 10;  

    for (int d = 0; d < 8; d++) {  
        int tx = playerX + dx[d];
        int ty = playerY + dy[d];

        for (int j = 0; j < monsterCount; j++) {
            if (monsters[j].x == tx && monsters[j].y == ty) {
                monsters[j].health -= damage;
                displayMonsterHealth(j);

                if (monsters[j].health <= 0) {
                    map[monsters[j].y][monsters[j].x] = '.';  
                    displayMonsterHealth(j);
                    mvprintw(MAP_HEIGHT + 2, 80, "                                              ");  
                    showMessage("💀 Monster defeated!");
                    

                    for (int k = j; k < monsterCount - 1; k++) {
                        monsters[k] = monsters[k + 1];  
                    }
                    monsterCount--;  
                    return;  
                } else {
                    showMessage("🗡️ You hit a monster!");
                    displayMonsterHealth(j);
                }
            }
        }
    }
}

void attackRanged() {
    if (weaponInventory[equippedWeapon] <= 0) {
        showMessage("❌ No ammo left for this weapon!");
        return;
    }

    char input = getch();
    int dx = 0, dy = 0;

    if (input == 'h') dx = -1;   
    else if (input == 'l') dx = 1;  
    else if (input == 'j') dy = 1; 
    else if (input == 'k') dy = -1; 
    else if (input == 'y') { dx = -1; dy = -1; } 
    else if (input == 'u') { dx = 1; dy = -1; }  
    else if (input == 'b') { dx = -1; dy = 1; }  
    else if (input == 'n') { dx = 1; dy = 1; }   
    else {
        showMessage("❌ Invalid direction!");
        return;
    }

    int range = weapons[equippedWeapon].range;
    int damage = weapons[equippedWeapon].damage;
    int x = playerX, y = playerY;
    int lastValidX = x, lastValidY = y;

    weaponInventory[equippedWeapon]--;  

    for (int i = 0; i < range; i++) {
        x += dx;
        y += dy;

        if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
            showMessage("⚠️ The weapon flew off the map!");
            return;
        }

        if (map[y][x] == '#') {  
            showMessage("🔹 The weapon hit a wall and fell down!");
            if (map[lastValidY][lastValidX] == '.') {  
                map[lastValidY][lastValidX] = weaponSymbols[equippedWeapon];
            }
            return;
        } 

        if (isMonsterAt(x, y)) {  
            int monsterIndex = getMonsterAt(x, y);
            monsters[monsterIndex].health -= damage;
            displayMonsterHealth(monsterIndex);

            if (monsters[monsterIndex].health <= 0) {
                showMessage("💀 Monster defeated!");
                map[monsters[monsterIndex].y][monsters[monsterIndex].x] = '.';  
                mvprintw(MAP_HEIGHT + 2, 80, "                                              ");  
                for (int k = monsterIndex; k < monsterCount - 1; k++) {
                    monsters[k] = monsters[k + 1];  
                }
                monsterCount--;  
            } else {
                showMessage("🔥 You hit a monster!");
            }
            return;
        } 
        
        if (map[y][x] != '.') {  
            showMessage("🪶 The weapon fell on the ground.");
            map[lastValidY][lastValidX] = weaponSymbols[equippedWeapon];  
            return;
        }

        lastValidX = x;
        lastValidY = y;
    }

    showMessage("🪶 The weapon reached its max range and fell.");
    map[lastValidY][lastValidX] = weaponSymbols[equippedWeapon];  
}

void attack() {
    if (equippedWeapon == WEAPON_MACE || equippedWeapon == WEAPON_SWORD) {
        attackMelee();  
    } else {
        attackRanged();  
    }
}

void placeWeapons() {
    WeaponType weaponTypes[5] = {WEAPON_MACE, WEAPON_DAGGER, WEAPON_SWORD, WEAPON_WAND, WEAPON_ARROW};

    for (int i = 0; i < roomCount; i++) {
        Room room = rooms[i]; 
        int weaponCountInRoom = 0;

        
        if (room.type == ROOM_SPELL) continue;

        
        if (room.type == ROOM_NORMAL) {
            weaponCountInRoom = 1 + rand() % 3; 
        } 
        else if (room.type == ROOM_TREASURE) {
            weaponCountInRoom = 2 + rand() % 3;
        }

        int placedWeapons = 0;
        while (placedWeapons < weaponCountInRoom && weaponCount < MAX_WEAPONS) { 
            int x = room.x + 1 + rand() % (room.width - 2);
            int y = room.y + 1 + rand() % (room.height - 2);

            if (map[y][x] == '.') {  
                weapons[weaponCount].x = x;
                weapons[weaponCount].y = y;
                weapons[weaponCount].type = weaponTypes[rand() % 5];  
                weapons[weaponCount].symbol = weaponSymbols[weapons[weaponCount].type];

                map[y][x] = weapons[weaponCount].symbol;  
                weaponCount++;
                placedWeapons++;
            }
        }
    }
    initializeWeapons();
}

void activateMonstersInRoom() {
    for (int i = 0; i < roomCount; i++) {
        if (playerX >= rooms[i].x && playerX < rooms[i].x + rooms[i].width &&
            playerY >= rooms[i].y && playerY < rooms[i].y + rooms[i].height) {

            for (int j = 0; j < monsterCount; j++) {
                if (monsters[j].x >= rooms[i].x && monsters[j].x < rooms[i].x + rooms[i].width &&
                    monsters[j].y >= rooms[i].y && monsters[j].y < rooms[i].y + rooms[i].height) {
                    
                    monsters[j].active = true; 

                    switch (monsters[j].symbol) {
                        case 'G': monsters[j].chaseRange = 5; break;
                        case 'S': monsters[j].chaseRange = -1; break;
                        case 'U': monsters[j].chaseRange = 5; break;
                    }
                }
            }
            break;  
        }
    }
}

void findMonstersInRoom() {
    for (int i = 0; i < roomCount; i++) {  
        Room room = rooms[i];         
        if (playerX >= room.x && playerX <= room.x + room.width &&
            playerY >= room.y && playerY <= room.y + room.height) {
                     
            for (int j = 0; j < monsterCount; j++) {  
                Monster monster = monsters[j];  

                
                if (monster.x >= room.x && monster.x <= room.x + room.width &&
                    monster.y >= room.y && monster.y <= room.y + room.height) {
                    
                    activateMonstersInRoom();
                    displayMonsterHealth(j);
                }
            }
        }
    }
}

void placeMonsters() {
     start_color();
     attron(COLOR_PAIR(6));
    
    char types[] = {'D', 'F', 'G', 'S', 'U'};  // Demon, Fire, Giant, Snake, Undead

    for (int i = 0; i < roomCount; i++) {  
        Room room = rooms[i]; 

        
        int monstersInRoom = 1 ;  

        for (int j = 0; j < monstersInRoom && monsterCount < MAX_MONSTERS; j++) {
            int mx = room.x + 1 + rand() % (room.width - 2);
            int my = room.y + 1 + rand() % (room.height - 2);

            
            if (map[my][mx] != '.') { 
                continue; 
            }

            
            char chosenType = types[rand() % 5];
            monsters[monsterCount].symbol = chosenType;
            monsters[monsterCount].x = mx;
            monsters[monsterCount].y = my;
            monsters[monsterCount].active = true;


            switch (chosenType) {
                case 'D': 
                    monsters[monsterCount].health = 5;
                    monsters[monsterCount].chaseRange = 4;
                    break;
                case 'F': 
                    monsters[monsterCount].health = 10;
                    monsters[monsterCount].chaseRange = 4;
                    break;
                case 'G': 
                    monsters[monsterCount].health = 15;
                    monsters[monsterCount].chaseRange = 5;
                    break;
                case 'S': 
                    monsters[monsterCount].health = 20;
                    monsters[monsterCount].chaseRange = -1; 
                    break;
                case 'U': 
                    monsters[monsterCount].health = 30;
                    monsters[monsterCount].chaseRange = 5;
                    break;
            }
            
            
            map[my][mx] = monsters[monsterCount].symbol;  
            monsterCount++;
            attroff(COLOR_PAIR(6));
        }
    }
}

void moveMonster(int index) {
    int mx = monsters[index].x;
    int my = monsters[index].y;

    if (mx < playerX) mx++;
    else if (mx > playerX) mx--;

    if (my < playerY) my++;
    else if (my > playerY) my--;

    if (map[my][mx] == '.') {
        map[monsters[index].y][monsters[index].x] = '.';  
        monsters[index].x = mx;
        monsters[index].y = my;
        map[my][mx] = monsters[index].symbol;
    }
}

int isMonsterInPlayerRoom(int monsterIndex) {
    for (int i = 0; i < roomCount; i++) {
        Room room = rooms[i];

       
        if (playerX >= room.x && playerX < room.x + room.width &&
            playerY >= room.y && playerY < room.y + room.height) {
            
            
            if (monsters[monsterIndex].x >= room.x && monsters[monsterIndex].x < room.x + room.width &&
                monsters[monsterIndex].y >= room.y && monsters[monsterIndex].y < room.y + room.height) {
                
                return 1; 
            }
        }
    }
    return 0; 
}

void moveMonsters() {
    for (int i = 0; i < monsterCount; i++) {
        int mx = monsters[i].x;
        int my = monsters[i].y;
        char type = monsters[i].symbol;

        if (!isMonsterInPlayerRoom(i)) continue;

        if (abs(mx - playerX) <= 1 && abs(my - playerY) <= 1) {
            int damage = 0;
            switch (type) {
                case 'D': damage = 4; break;
                case 'F': damage = 6; break;
                case 'G': damage = 8; break;
                case 'S': damage = 10; break;
                case 'U': damage = 12; break;
            }

            playerHealth -= damage;
            if (playerHealth < 0) playerHealth = 0;
            displayPlayerStats();  

           
            char *message = (char*)malloc(200 * sizeof(char)); 

            sprintf(message, "Monster %c attacked! - You lost %d percent of your health", type, damage);
            showMessage(message); 
            continue;
        }

        if (type == 'S') {  
            moveMonster(i);  
        } 
        else if (type == 'U') {  
            if (monsters[i].chaseRange > 0) {  
                moveMonster(i);
                monsters[i].chaseRange--;
            }
        } 
        else if (type == 'G' || type == 'F') {  
            if (monsters[i].chaseRange > 0) {
                moveMonster(i);
                monsters[i].chaseRange--;
            }
        } 
        else {  
            moveMonster(i);  
        }
    }
}

void fastMove(char direction) {
    int dx = 0, dy = 0;
    
    switch (direction) {
        case 'j': dy = -1; break;
        case 'k': dy = 1; break;
        case 'h': dx = -1; break;
        case 'l': dx = 1; break;
        case 'y': dx = -1; dy = -1; break;
        case 'u': dx = 1; dy = -1; break;
        case 'b': dx = -1; dy = 1; break;
        case 'n': dx = 1; dy = 1; break;
        default: return; 
    }

    while (1) {
        int nextX = playerX + dx;
        int nextY = playerY + dy;

        char nextCell = map[nextY][nextX];

        if (nextCell == '.' || nextCell == '#' || nextCell == '+') {
            map[playerY][playerX] = prevCell;  
            prevCell = nextCell;
            playerX = nextX;
            playerY = nextY;
            map[playerY][playerX] = 'p';

            revealCorridor(playerX, playerY, dx, dy); 
            printMapNcurses();
            napms(50); 
        } else {
            break; 
        }
    }
}

void placeFoods() {
    FoodType foodTypes[4] = {FOOD_NORMAL, FOOD_SUPER, FOOD_MAGIC, FOOD_SPOILED};

    for (int i = 0; i < roomCount; i++) {
        Room room = rooms[i]; 
        int foodCountInRoom = 0;

        if (room.type == ROOM_SPELL) {
            foodCountInRoom = 1 + rand() % 2;
        } 
        else if (room.type == ROOM_NORMAL) {
            foodCountInRoom = 2 + rand() % 2;
        }

        int placedFoods = 0;
        while (placedFoods < foodCountInRoom && foodCount < 60) { 
            int x = room.x + 1 + rand() % (room.width - 2);  
            int y = room.y + 1 + rand() % (room.height - 2); 

            if (map[y][x] == '.') {  
                foods[foodCount].x = x;
                foods[foodCount].y = y;
                foods[foodCount].type = foodTypes[rand() % 4];  
                foods[foodCount].symbol = foodSymbols[foods[foodCount].type - 1]; 

                map[y][x] = foods[foodCount].symbol;  
                foodCount++;
                placedFoods++;
            }
        }
    }
}

void placeGold() {
    for (int i = 0; i < roomCount; i++) {
        Room room = rooms[i];
        int goldCount = 0;
        int blackGoldCount = 0;

        
        if (room.type == ROOM_NORMAL) {
            goldCount = 2 + rand() % 3;  
            blackGoldCount = rand() % 2;  
        } else if (room.type == ROOM_TREASURE) {
            goldCount = 8 + rand() % 4;  
            blackGoldCount = 2 + rand() % 3;  
        }

    
        for (int j = 0; j < goldCount; j++) {
            int x = room.x + 1 + rand() % (room.width - 2);
            int y = room.y + 1 + rand() % (room.height - 2);
            map[y][x] = GOLD_SYMBOL;  
        }

        for (int j = 0; j < blackGoldCount; j++) {
            int x = room.x + 1 + rand() % (room.width - 2);
            int y = room.y + 1 + rand() % (room.height - 2);
            map[y][x] = BLACK_GOLD_SYMBOL;
        }
    }
}


void placeSpells() {
    char spellTypes[3] = {'h', 's', 'd'};

    for (int i = 0; i < roomCount; i++) {
        Room room = rooms[i]; 
        int spellCount = 0;

        if (room.type == ROOM_NORMAL) {
            spellCount = 1 + rand() % 3; 
        } else if (room.type == ROOM_TREASURE) {
            spellCount = 1 + rand() % 2; 
        } else if (room.type == ROOM_SPELL) {
            spellCount = 8 + rand() % 5; 
        }

        int placedSpells = 0;
        while (placedSpells < spellCount) { 
            int x = room.x + 1 + rand() % (room.width - 2);
            int y = room.y + 1 + rand() % (room.height - 2);

            if (map[y][x] == '.') { 
                spells[placedSpells].x = x;
                spells[placedSpells].y = y;
                spells[placedSpells].type = spellTypes[rand() % 3];
                spells[placedSpells].active = 1;
                map[y][x] = spells[placedSpells].type;
                placedSpells++; 
            }
        }
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

init_pair(1, COLOR_WHITE, COLOR_BLACK);    
init_pair(2, COLOR_YELLOW, COLOR_BLACK);     
init_pair(3, COLOR_MAGENTA, COLOR_BLACK);    
init_pair(4, COLOR_CYAN, COLOR_BLACK);       
}


void ensureTreasureRooms() {
    if (currentFloor != 3) return;  

    int treasureCount = 0;
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].type == ROOM_TREASURE)
            treasureCount++;
    }

    if (treasureCount == 0 && roomCount > 0) {
        int randomRoom = rand() % roomCount;
        rooms[randomRoom].type = ROOM_TREASURE;
    }
}


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


void placeSecretDoors() {
    for (int i = 0; i < roomCount; i++) {
        if (rooms[i].type == ROOM_NORMAL && isRoomDeadEnd(rooms[i])) {
            int spellRoomIndex = findClosestSpellRoom(rooms[i].x, rooms[i].y);
            if (spellRoomIndex == -1) continue;  

            Room spellRoom = rooms[spellRoomIndex];
            int secretDoorX = rooms[i].x + rooms[i].width / 2;
            int secretDoorY = rooms[i].y + rooms[i].height / 2;
            map[secretDoorY][secretDoorX] = 'D';

            int targetX = spellRoom.x + spellRoom.width / 2;
            int targetY = spellRoom.y + spellRoom.height / 2;
            map[targetY][targetX] = 'D';
        }
    }
}*/


void gameWin() {
    clear();
    
    attron(COLOR_PAIR(6));  
    mvprintw(LINES / 2 - 4, COLS / 2 - 13, "🏆🏆🏆🏆🏆🏆🏆🏆🏆🏆🏆🏆🏆");
    mvprintw(LINES / 2 - 3, COLS / 2 - 13, "🏆                        🏆");
    mvprintw(LINES / 2 - 2, COLS / 2 - 13, "🏆  🎉 YOU WIN! 🎉  🏆");
    mvprintw(LINES / 2 - 1, COLS / 2 - 13, "🏆  The dungeon bows  🏆");
    mvprintw(LINES / 2, COLS / 2 - 13, "🏆   before your might!  🏆");
    mvprintw(LINES / 2 + 1, COLS / 2 - 13, "🏆                        🏆");
    mvprintw(LINES / 2 + 2, COLS / 2 - 13, "🏆🏆🏆🏆🏆🏆🏆🏆🏆🏆🏆🏆🏆");
    attroff(COLOR_PAIR(6));

    attron(COLOR_PAIR(2));  
    mvprintw(LINES / 2 + 4, COLS / 2 - 15, "Press any key to see leaderboard...");
    attroff(COLOR_PAIR(2));

    refresh();
    getchar();

    showLeaderboard();  
}

void gameOver() {
    clear();
    
    attron(COLOR_PAIR(5));  
    mvprintw(LINES / 2 - 3, COLS / 2 - 10, "██████████████████");
    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "█                █");
    mvprintw(LINES / 2 - 1, COLS / 2 - 10, "█  💀 GAME OVER 💀  █");
    mvprintw(LINES / 2, COLS / 2 - 10, "█  You have fallen. █");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "█                █");
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "██████████████████");
    attroff(COLOR_PAIR(5));

    attron(COLOR_PAIR(3));  
    mvprintw(LINES / 2 + 4, COLS / 2 - 15, "Press any key to see leaderboard...");
    attroff(COLOR_PAIR(3));
    
    refresh();
    getchar();  
    
    showLeaderboard();  
}


void checkWinCondition() {
    int roomIndex = findRoomIndex(playerX, playerY);

    if (roomIndex != -1 && rooms[roomIndex].type == ROOM_TREASURE) {  
        if (goldAmount >= 50 ) {  
            gameWin();  
        } 
    }
}

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

int setupcolor(){
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(2, COLOR_WHITE, COLOR_BLACK); 
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK); 
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK); 
    init_pair(6, COLOR_GREEN, COLOR_BLACK); 
}

int start() {
    setlocale(LC_ALL,"C.UTF-8");
    start:
    srand(time(0)); 
    setupcolor();

    //MAP_WIDTH = MIN_MAP_WIDTH + rand() % (MAX_MAP_WIDTH - MIN_MAP_WIDTH + 1);
    //MAP_HEIGHT = MIN_MAP_HEIGHT + rand() % (MAX_MAP_HEIGHT - MIN_MAP_HEIGHT + 1);
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
    attron(COLOR_PAIR(playerColor));
    mvprintw(playerY, playerX, "P");
    attroff(COLOR_PAIR(playerColor));
    placepiller(); 
    
    
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    
    memset(discovered, 0, sizeof(discovered)); 
    revealCurrentRoom(); 
    setupFirstFloor();
    placeFoods();
    placeGold();
    placeWeapons();
    drawVerticalMessageBox();
    drawStatusBox();
    placeSpells();
    placeMonsters();
    printMapNcurses();
    char input;
    while ((input = getch()) != 'q') { 
        movePlayer(input);
        moveMonsters();
        increaseHunger() ;
        displayPlayerStats();
        if(playerHealth==0){
            updatePlayerStats(goldAmount);
            gameOver();
        }
        findMonstersInRoom();
      //  displayMonsterInfo();
    }

    endwin();
    return 0;
}

void pickupItem() {

    if (itemUnderPlayer == 'h' || itemUnderPlayer == 'd' || itemUnderPlayer == 's') {
        if (itemUnderPlayer== 'h') {
            collectedHealth++;
            showMessage("You picked up a Health Spell!");
        } else if (itemUnderPlayer == 's') {
            collectedSpeed++;
            showMessage("You picked up a Speed Spell!");
        } else if (itemUnderPlayer == 'd') {
            collectedDamage++;
            showMessage("You picked up a Damage Spell!");
        }
        map[playerY][playerX]='.';
        itemUnderPlayer = '.';
        prevCell = '.';
        map[playerY][playerX]='p';
        printMapNcurses();
    }
    if (itemUnderPlayer == 'M' || itemUnderPlayer == 'z' || itemUnderPlayer == 'v'|| itemUnderPlayer == 'W'|| itemUnderPlayer == 'A') {
        if (itemUnderPlayer== 'M') {
            collectedMACE++;
            showMessage("You picked up a MACE!");
        } else if (itemUnderPlayer == 'z') {
            collectedDAGGER+=10;
            showMessage("You picked up a DAGGER !");
        } else if (itemUnderPlayer == 'v') {
            collectedSWORD+=100;
            showMessage("You picked up a SWORD!");
        } else if (itemUnderPlayer == 'W') {
            collectedWAND+=8;
            showMessage("You picked up a WAND");
        } else if (itemUnderPlayer == 'A') {
            collectedARROW+=20;
            showMessage("You picked up a ARROW!");
        }
        map[playerY][playerX]='.';
        itemUnderPlayer = '.';
        prevCell = '.';
        map[playerY][playerX]='p';
        printMapNcurses();
    }

    if (itemUnderPlayer == '1' || itemUnderPlayer == '2' || itemUnderPlayer == '3'|| itemUnderPlayer == '4') {
        if (itemUnderPlayer== '1') {
            collectedNORMAL++;
            showMessage("You picked up a normal food !");
        } else if (itemUnderPlayer == '2') {
            collectedSUPER++;
            showMessage("You picked up a super food !");
        } else if (itemUnderPlayer == '3') {
            collectedMAGIC++;
            showMessage("You picked up a magic food !");
        } /*else if (itemUnderPlayer == '4') {
            collectedROTTON++;
            showMessage("You picked up a rotton food !");
        } */
        map[playerY][playerX]='.';
        itemUnderPlayer = '.';
        prevCell = '.';
        map[playerY][playerX]='p';
        printMapNcurses();
    }
    
    
    
     /*else {
        showMessage("There's nothing to pick up here.");
    }*/
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

void useHealthSpell() {
    if (collectedHealth > 0) {
        collectedHealth--;
        showMessage("You used a Health Spell!");
        for (int i =0 ; i<10;i++){
        getchar();
        playerHealth+=1;
        displayPlayerStats();}
    
    } else {
        showMessage("No Health Spells left!");
    }
}

void useSpeedSpell() {
    if (collectedSpeed > 0) {
        collectedSpeed--;
        showMessage("You used a Speed Spell!");
        char input=getchar();
        fastMove(input);
    } else {
        showMessage("No Speed Spells left!");
    }
}

void useDamageSpell() {
    if (collectedDamage > 0) {
        collectedDamage--;
        showMessage("You used a Damage Spell!");
        for (int i =0 ; i<10;i++){
             getchar();
        weapons->damage*=2;}
    } else {
        showMessage("No Damage Spells left!");
    }
}

void showSpellMenu() {
    int highlight = 0; 
    int choice;
    int spellCount = 4; 

    WINDOW *spellWin = newwin(10, 40, 5, 10);
    box(spellWin, 0, 0);
    keypad(spellWin, TRUE);  

    while (1) {
        werase(spellWin);
        box(spellWin, 0, 0);
        mvwprintw(spellWin, 1, 2, "Spell Inventory");
        
        if (highlight == 0) wattron(spellWin, A_REVERSE);
        mvwprintw(spellWin, 3, 2, "Health Spell (%d)", collectedHealth);
        wattroff(spellWin, A_REVERSE);

        if (highlight == 1) wattron(spellWin, A_REVERSE);
        mvwprintw(spellWin, 4, 2, "Speed Spell (%d)", collectedSpeed);
        wattroff(spellWin, A_REVERSE);

        if (highlight == 2) wattron(spellWin, A_REVERSE);
        mvwprintw(spellWin, 5, 2, "Damage Spell (%d)", collectedDamage);
        wattroff(spellWin, A_REVERSE);

        if (highlight == 3) wattron(spellWin, A_REVERSE);
        mvwprintw(spellWin, 7, 2, "back to the game");
        wattroff(spellWin, A_REVERSE);

        wrefresh(spellWin);

        choice = getch();

        switch (choice) {
            case KEY_UP:
                highlight = (highlight - 1 + spellCount) % spellCount;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % spellCount;
                break;
            case '\n':  
                if (highlight == 0) useHealthSpell();
                else if (highlight == 1) useSpeedSpell();
                else if (highlight == 2) useDamageSpell();
                else if (highlight == 3) {  
                    delwin(spellWin);
                    printMapNcurses();
                    return;
                }
                break;
        }
    }
}

void showWeaponMenu() {
    int choice;
    WINDOW *weaponWin = newwin(14, 55, 5, 10);
    box(weaponWin, 0, 0);
    keypad(weaponWin, TRUE);  

    while (1) {
        werase(weaponWin);
        box(weaponWin, 0, 0);
        mvwprintw(weaponWin, 1, 2, "🎒 Weapon Inventory");
         if (equippedWeapon == -1) {
            mvwprintw(weaponWin, 2, 2, "🔻 Equipped: (None)");
        } else {
            //mvwprintw(weaponWin, 2, 2, "🔻 Equipped: %s", Weapon[equippedWeapon].weaponType);
        }
          

        mvwprintw(weaponWin, 3, 2, "🗡️ Melee Weapons:");
        mvwprintw(weaponWin, 4, 4, "⚒️ Mace (M) - DMG: 8 | RNG: 1 [%d]", collectedMACE);
        mvwprintw(weaponWin, 5, 4, "🗡️ Dagger (z) - DMG: 5 | RNG: 1 [%d]", collectedDAGGER);
        mvwprintw(weaponWin, 6, 4, "⚔️ Sword (v) - DMG: 10 | RNG: 1 [%d]", collectedSWORD);

        mvwprintw(weaponWin, 8, 2, "🏹 Ranged Weapons:");
        mvwprintw(weaponWin, 9, 4, "🪄 Wand (W) - DMG: 6 | RNG: 3 [%d]", collectedWAND);
        mvwprintw(weaponWin, 10, 4, "🪃 Arrow (A) - DMG: 4 | RNG: 5 [%d]", collectedARROW);

        mvwprintw(weaponWin, 12, 2, "🔄 Press 'w' to unequip weapon | Any key to exit");

        wrefresh(weaponWin);
        choice = getch();

        if (choice == 'w') {  
            if (equippedWeapon == -1) {
                showMessage("❌ You don't have an equipped weapon!");
            } else {
                showMessage("✔️ Weapon unequipped! Select a new one.");
                equippedWeapon = -1;
            }
        }
        else if ((choice == 'M' && collectedMACE > 0) ||
                 (choice == 'z' && collectedDAGGER > 0) ||
                 (choice == 'v' && collectedSWORD > 0) ||
                 (choice == 'W' && collectedWAND > 0) ||
                 (choice == 'A' && collectedARROW > 0)) {

            WeaponType newWeapon;
            if (choice == 'M') newWeapon = WEAPON_MACE;
            else if (choice == 'z') newWeapon = WEAPON_DAGGER;
            else if (choice == 'v') newWeapon = WEAPON_SWORD;
            else if (choice == 'W') newWeapon = WEAPON_WAND;
            else newWeapon = WEAPON_ARROW;

            if (equippedWeapon == newWeapon) {
                showMessage("⚠️ You already have this weapon equipped! Unequip it first.");
            } else {
                if (equippedWeapon != -1) {  
                    switch (equippedWeapon) {
                        case WEAPON_MACE: collectedMACE++; break;
                        case WEAPON_DAGGER: collectedDAGGER++; break;
                        case WEAPON_SWORD: collectedSWORD++; break;
                        case WEAPON_WAND: collectedWAND++; break;
                        case WEAPON_ARROW: collectedARROW++; break;
    }
}


                equippedWeapon = newWeapon;
                if (equippedWeapon == WEAPON_MACE) weaponInventory[equippedWeapon] = 1; 
                else if (weaponInventory[equippedWeapon] == 0) weaponInventory[equippedWeapon] = 10; 

            }
        }
        else {
            showMessage("❌ Invalid selection or weapon not available!");
        }

        delwin(weaponWin);
        printMapNcurses();
        return;
    }
}

void useRottenFood() {
    if (1) {
        collectedROTTON--;  
        playerHealth -= 10;  

        if (playerHealth < 0) 
            playerHealth = 0;  

        hungerLevel += 3;  
        if (hungerLevel > 100) 
            hungerLevel = 100;  

        showMessage("You ate rotten food! Health -10, Hunger +3.");
        displayPlayerStats();  
    } else {
        showMessage("You don't have any rotten food!");
    }
}

void useNormalFood() {
    int b= rand() % 2;
    if ( b ){
    if (collectedNORMAL > 0) {
        collectedNORMAL--;  
        playerHealth += 10;  

        if (playerHealth > MAX_HEALTH) 
            playerHealth = MAX_HEALTH;  

        hungerLevel -= 5;  
        if (hungerLevel < 0) 
            hungerLevel = 0;  

        showMessage("You ate normal food. Health +10, Hunger -5");
        displayPlayerStats();  
    } 
    else {
        showMessage("You don't have any normal food!");
    }
}

    else{
        useRottenFood();
    }

}

void useSuperFood() {
    if (collectedSUPER > 0) {
        collectedSUPER--;  
        playerHealth += 15;  

        if (playerHealth > MAX_HEALTH) 
            playerHealth = MAX_HEALTH;  

        hungerLevel -= 8;  
        if (hungerLevel < 0) 
            hungerLevel = 0;  

        showMessage("You ate superior food. Health +15, Hunger +8, Strength boosted!");
        displayPlayerStats();  
        //boostStrength();  
    } else {
        showMessage("You don't have any superior food!");
    }
}

void useMagicFood() {
    if (collectedMAGIC > 0) {
        collectedMAGIC--;  
        playerHealth += 10;  

        if (playerHealth > MAX_HEALTH) 
            playerHealth = MAX_HEALTH;  

        hungerLevel -= 10;  
        if (hungerLevel < 0) 
            hungerLevel = 0;  

        showMessage("You ate magic food. Health +10, Hunger +10, Speed boosted!");
        displayPlayerStats();  

        char direction = '\0';  

        while (true) {
            char input = getch(); 

            
            if (input == 'k' || input == 'j' || input == 'l' || input == 'h' || 
                input == 'y' || input == 'u' || input == 'b' || input == 'n') {
                direction = input;  
                break;  
            }
        }

        fastMove(direction);
    } 
    else {
        showMessage("You don't have any magic food!");
    }
}

void showFoodMenu() {
    int highlight = 0;  
    int choice;
    int foodCount = 5;  

    WINDOW *foodWin = newwin(12, 50, 5, 10);  
    box(foodWin, 0, 0);  
    keypad(foodWin, TRUE);  

    while (1) {
        werase(foodWin);  
        box(foodWin, 0, 0); 
        mvwprintw(foodWin, 1, 2, "Food Inventory");  

        
        if (highlight == 0) wattron(foodWin, A_REVERSE);
        mvwprintw(foodWin, 3, 2, "Normal Food 🍞 - %d", collectedNORMAL);
        wattroff(foodWin, A_REVERSE);

        
        if (highlight == 1) wattron(foodWin, A_REVERSE);
        mvwprintw(foodWin, 4, 2, "Super Food 🍗 - %d", collectedSUPER);
        wattroff(foodWin, A_REVERSE);

        
        if (highlight == 2) wattron(foodWin, A_REVERSE);
        mvwprintw(foodWin, 5, 2, "Magic Food 🍎🪄 - %d", collectedMAGIC);
        wattroff(foodWin, A_REVERSE);

        
        /*if (highlight == 3) wattron(foodWin, A_REVERSE);
        mvwprintw(foodWin, 6, 2, "Rotton Food 🥴 - %d", collectedROTTON);
        wattroff(foodWin, A_REVERSE);*/

        
        if (highlight == 4) wattron(foodWin, A_REVERSE);
        mvwprintw(foodWin, 9, 2, "Back to game");
        wattroff(foodWin, A_REVERSE);

        wrefresh(foodWin);  

        choice = getch();  
        switch (choice) {
            case KEY_UP:  
                highlight = (highlight - 1 + foodCount) % foodCount;
                break;
            case KEY_DOWN:  
                highlight = (highlight + 1) % foodCount;
                break;
            case '\n':  
                if (highlight == 0 && collectedNORMAL > 0) useNormalFood();
                else if (highlight == 1 && collectedSUPER > 0) useSuperFood();
                else if (highlight == 2 && collectedMAGIC > 0) useMagicFood();
                else if (highlight == 3 && collectedROTTON > 0) useRottenFood();
                 if (highlight == 4) {  
                    delwin(foodWin);
                    printMapNcurses();
                    return;
                }
                break;
        }
    }
}

void regenerateHealth() {
    static int regenCounter = 0;  

    if (hungerLevel == 0) {  
        regenCounter++;

        if (regenCounter % 5 == 0) {  
            if (playerHealth < MAX_HEALTH) {  
                playerHealth++;
                displayPlayerStats();  
                showMessage("You feel a strange force regenerating you...");
            }
        }
    } else {
        regenCounter = 0;  
    }
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
        showFullMap = !showFullMap; 
        printMapNcurses(); 
        return;
    }
    if (input == 'p' || input == 'P') {
        pickupItem();
        return;
    } 

    if (input == 'm' || input == 'M') {
        showSpellMenu();
        return;
    }
    if (input == 'w' || input == 'W') {
    showWeaponMenu();
    return;
}
if (input == 'z' || input == 'Z') {
        playerHealth = 100;
        displayPlayerStats();
        return;
    }

    if (input == 'd' || input == 'D') {
        showFoodMenu();
        return;
}

if (input == 'f' || input == 'F') {
    char direction = getch(); 
    fastMove(direction);
    return;
}
if (input == ' ') {
    attack();
    return;
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
    
    if (nextCell == '.' || nextCell == '#' || nextCell == '+') {
        int newRoom = findRoomIndex(newX, newY); 

        
        if (newRoom != -1 && newRoom != currentRoomIndex && ! visitedRoomsindex[newRoom]) {
            visitedRoomsindex[newRoom] = true; 
            currentRoomIndex = newRoom;
            if (currentRoomIndex !=findRoomIndex(playerX,playerY)){
            showMessage("You are about to enter in a new room!");
        }}
        map[playerY][playerX] = prevCell;  
        prevCell = map[newY][newX];
        playerX = newX;
        playerY = newY;
        map[playerY][playerX] = 'p';

        printMapNcurses();
    }
    



    if (nextCell == '.' || nextCell == '#' || nextCell == '+' || nextCell == '1' ||nextCell == '2' ||nextCell == '3' ||nextCell == '4' ||
        nextCell == 'h' || nextCell == 's' || nextCell == 'd' ||nextCell == '$' || nextCell == 'T'  || nextCell == '*'||nextCell == 'M' ||nextCell == 'z' ||nextCell == 'N' ||nextCell == 'W' ||nextCell == 'A' ||nextCell == 'v') {

        itemUnderPlayer = map[newY][newX];
        
        if(nextCell == '$' || nextCell == '*'){
            
        if (nextCell == '$') {  
            goldAmount += 10;  
            showMessage("You are going  to gain 10 gold");
        } 
        if (nextCell == 'T') {  
            playerHealth -= 4;  
            displayPlayerStats();
            refresh();
        
        } 
        else if (nextCell == '*') { 
            goldAmount += 50;
            showMessage("You are going  to gaine 50 gold");  
        }

        nextCell = '.';  
        
        }
        
        if (map[playerY][playerX] == 'p') {
            map[playerY][playerX] = prevCell;  
        }
        if (playerX == prevStairX && playerY == prevStairY) {
            map[playerY][playerX] = '<'; 
        }

        map[playerY][playerX] = prevCell;
        prevCell = itemUnderPlayer;
        prevCell = nextCell;
        playerX = newX;
        playerY = newY;
        map[playerY][playerX] = 'p';      

        printMapNcurses();
    }
    
    else if (nextCell == '^') {
        map[playerY][playerX] = '<';
        printNewMap();
    }
    if(playerHealth==0){
            updatePlayerStats(goldAmount);
            gameOver();
        }
         regenerateHealth();      
         checkWinCondition();  

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

    //clear();

    //printMapNcurses();
}

void convertTwoRoomsToTreasure() {
    if (currentFloor != 3) return;  

   // mvprintw(MAP_HEIGHT + 2, 0, "Converting rooms to treasure...");
   // refresh();
    //getch(); 

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

   // MAP_WIDTH = MIN_MAP_WIDTH + rand() % (MAX_MAP_WIDTH - MIN_MAP_WIDTH + 1);
    //MAP_HEIGHT = MIN_MAP_HEIGHT + rand() % (MAX_MAP_HEIGHT - MIN_MAP_HEIGHT + 1);

    clear();
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
    drawVerticalMessageBox();
    drawStatusBox();
    placeFoods();
    placeGold();
    placeWeapons();
    setupFirstFloor();
    convertEntryRoomToSpellRoom();
    convertTwoRoomsToTreasure();
    placeMonsters();
    printMapNcurses();
    showMessage("You have entered a new floor!");
    refresh();

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

bool isInsideRoom(int x, int y) {
    for (int r = 0; r < roomCount; r++) {
        Room room = rooms[r];
        if (x >= room.x + 1 && x < room.x + room.width - 1 && 
            y >= room.y + 1 && y < room.y + room.height - 1) {
            return true;
        }
    }
    return false;
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


void savePlayerData() {
    FILE *file = fopen("leaderboard.txt", "w");
    if (!file) {
        mvprintw(5, 5, "Error: Unable to save data!");
        getch();
        return;
    }

    fprintf(file, "%d\n", playerCount);
    for (int i = 0; i < playerCount; i++) {
        fprintf(file, "%s %d %d %d %ld\n", players[i].username, players[i].gold, players[i].score, players[i].gamesPlayed, players[i].firstGameTime);
    }
    fclose(file);
}

void loadPlayerData() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) {
        playerCount = 0;
        return;
    }

    fscanf(file, "%d", &playerCount);
    for (int i = 0; i < playerCount; i++) {
        fscanf(file, "%s %d %d %d %ld", players[i].username, &players[i].gold, &players[i].score, &players[i].gamesPlayed, &players[i].firstGameTime);
    }
    fclose(file);
}

int comparePlayers(const void *a, const void *b) {
    return ((Player*)b)->score - ((Player*)a)->score;
}

void updatePlayerStats(int goldAmount) {
    int found = 0;
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i].username, currentUsername) == 0) {
            players[i].gold += goldAmount;
            players[i].score = players[i].gold * 2;
            players[i].gamesPlayed++;
            found = 1;
            break;
        }
    }

    if (!found) {
        strcpy(players[playerCount].username, currentUsername);
        players[playerCount].gold = goldAmount;
        players[playerCount].score = goldAmount * 2;
        players[playerCount].gamesPlayed = 1;  
        players[playerCount].firstGameTime = time(NULL);
        playerCount++;
    }

    savePlayerData();
}

void showLeaderboard() {
    loadPlayerData();
    qsort(players, playerCount, sizeof(Player), comparePlayers);

    initscr();
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(2, COLOR_RED, COLOR_BLACK);    
    init_pair(3, COLOR_BLUE, COLOR_BLACK);   
    init_pair(4, COLOR_WHITE, COLOR_BLACK);  

    clear();
    attron(A_BOLD);
    mvprintw(1, 10, "===========================================================");
    mvprintw(2, 25, "🏆 SCOREBOARD 🏆");
    mvprintw(3, 10, "===========================================================");
    attroff(A_BOLD);

    mvprintw(5, 2, "Rank | Username          | Title               | Score  | Gold  | Games | Experience");
    mvprintw(6, 2, "-------------------------------------------------------------------------------");

    for (int i = 0; i < playerCount; i++) {
        int y = 7 + i;
        char rankSymbol[5] = "";
        char title[20] = "Adventurer";
        int color = 4; 
        if (i == 0) { strcpy(rankSymbol, "👑"); strcpy(title, "Legendary Hero"); color = 1; } 
        else if (i == 1) { strcpy(rankSymbol, "🔥"); strcpy(title, "Master of Battle"); color = 2; } 
        else if (i == 2) { strcpy(rankSymbol, "⚡"); strcpy(title, "Stormbringer"); color = 3; } 

        int experience = (time(NULL) - players[i].firstGameTime) / (60 * 60 * 24);

        attron(COLOR_PAIR(color));
        if (strcmp(players[i].username, currentUsername) == 0) attron(A_UNDERLINE | A_BOLD);

        mvprintw(y, 2, "%-4d | %-16s %-2s | %-18s | %-6d | %-6d | %-5d | %-5d days", 
                 i + 1, players[i].username, rankSymbol, title, players[i].score, players[i].gold, players[i].gamesPlayed, experience);

        attroff(COLOR_PAIR(color));
        if (strcmp(players[i].username, currentUsername) == 0) attroff(A_UNDERLINE | A_BOLD);
    }

    getch();
    endwin();
}

void loadGame() {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_GREEN, COLOR_BLACK);

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

   
    memset(map, '.', sizeof(map));

    if (fread(&state, sizeof(GameState), 1, file) != 1) {
        mvprintw(LINES - 2, 0, "⚠️ خطا در بارگذاری اطلاعات سیو!");
        refresh();
        fclose(file);
        return;
    }

    fclose(file);

    roomCount = state.roomCount;
    memcpy(rooms, state.rooms, sizeof(rooms));

    
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map[i][j] = state.map[i][j];
        }
    }

    memcpy(discovered, state.discovered, sizeof(discovered));
    playerX = state.playerX;
    playerY = state.playerY;
    playerHealth = state.playerHealth;
    hungerLevel = state.hungerLevel;
    goldAmount = state.goldAmount;
    monsterCount = state.monsterCount;
    memcpy(monsters, state.monsters, sizeof(monsters));
    foodCount = state.foodCount;
    memcpy(foods, state.foods, sizeof(foods));
    weaponCount = state.weaponCount;
    memcpy(weapons, state.weapons, sizeof(weapons));

    
    prevCell = '.';

    clear();
    displayPlayerStats();
    refresh();
    drawStatusBox();
    drawVerticalMessageBox();
    refresh();
    
    printMapNcurses();


   /* for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
    printf("✅ Game loaded successfully.\n");*/

    char input;
    while ((input = getch()) != 'q') {
        movePlayer(input);
        moveMonsters();
        increaseHunger();
        displayPlayerStats();
        findMonstersInRoom();
    }

    refresh();
}

void saveGame() {
    if (currentUsername[0] == '\0') return;

    char filename[100];
    snprintf(filename, sizeof(filename), "%s_save.dat", currentUsername);

    FILE *file = fopen(filename, "wb");
    if (!file) {
        mvprintw(LINES - 2, 0, "⚠️ Error: Could not save the game!");
        refresh();
        return;
    }

    state.roomCount = roomCount;
    memcpy(state.rooms, rooms, sizeof(rooms));
    
    
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            state.map[i][j] = map[i][j];
        }
    }

    memcpy(state.discovered, discovered, sizeof(discovered));
    state.playerX = playerX;
    state.playerY = playerY;
    state.playerHealth = playerHealth;
    state.hungerLevel = hungerLevel;
    state.goldAmount = goldAmount;
    state.monsterCount = monsterCount;
    memcpy(state.monsters, monsters, sizeof(monsters));
    state.foodCount = foodCount;
    memcpy(state.foods, foods, sizeof(foods));
    state.weaponCount = weaponCount;
    memcpy(state.weapons, weapons, sizeof(weapons));

    fwrite(&state, sizeof(GameState), 1, file);
    fflush(file);  
    fclose(file);

    mvprintw(LINES - 2, 0, "✅ Game saved successfully!");
    refresh();
}

void displayPlayerStats() {
    int barLength = 20; 
    int filledHealth = (playerHealth * barLength) / MAX_HEALTH;
    int filledHunger = (hungerLevel * barLength) / 100;

    
    mvprintw(MAP_HEIGHT + 2, 2, "Health: [");
    attron(COLOR_PAIR(6)); 
    for (int i = 0; i < filledHealth; i++) printw("#");
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(5)); 
    for (int i = filledHealth; i < barLength; i++) printw("-");
    attroff(COLOR_PAIR(5));
    printw("] %d %%", playerHealth);

    mvprintw(MAP_HEIGHT + 3, 2, "Hunger: [");
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
    mvprintw(MAP_HEIGHT + 2, centerX , "\xF0\x9F\x92\xB0 Gold: %d", goldAmount);
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

    
    if(cell == 'p' ) {
    attron(COLOR_PAIR(playerColor));  
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(playerColor));
} 


 else {
   
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

if(cell == '#' ) {
    attron(COLOR_PAIR(2));  
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(2));
}

if(cell == 'F' ||cell == 'S' ||cell == 'D' ||cell == 'U' ||cell == 'G'  ) {
    attron(COLOR_PAIR(5));  
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(5));
}


if(cell == '|' ||cell == '-' ) {
    attron(COLOR_PAIR(4));  
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(4));
}


if(cell == '$' ||cell == '*' ) {
    attron(COLOR_PAIR(1));  
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(1));
}

if(cell == 'M' ||cell == 'W' ||cell == 'A' ||cell == 'v' ||cell == 'z'  ) {
    attron(COLOR_PAIR(6));  
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(6));
}

if(cell == 'd' ||cell == 'h'||cell == 's'  ) {
    attron(COLOR_PAIR(3));  
    mvaddch(i, j, cell);
    attroff(COLOR_PAIR(3));
}

        }
        
    }
    
    displayGold();
    displayPlayerStats();
    refresh();
}
 
void chooseDifficulty() {
    const char *difficultyOptions[] = {"Easy", "Medium", "Hard"};
    int difficultyChoice = 2;
    int key;

    while (1) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 15, "===============================");
        mvprintw(2, 20, "Select Difficulty");
        mvprintw(3, 15, "===============================");
        attroff(A_BOLD);

        for (int i = 0; i < 3; i++) {
            int y = 6 + i * 2;
            if (i == difficultyChoice) attron(A_REVERSE);
            mvprintw(y, 18, "%s", difficultyOptions[i]);
            attroff(A_REVERSE);
        }

        mvprintw(12, 15, "[ ↑ / ↓ to change | ENTER to confirm | ESC to go back ]");

        key = getch();
        switch (key) {
            case KEY_UP:
                difficultyChoice = (difficultyChoice - 1 + 3) % 3;
                break;
            case KEY_DOWN:
                difficultyChoice = (difficultyChoice + 1) % 3;
                break;
            case 10: // ENTER
                if (difficultyChoice == 0) playerHealth = 100;
                else if (difficultyChoice == 1) playerHealth = 70;
                else if (difficultyChoice == 2) playerHealth = 50;
                return;
            case 27: // ESC
                return;
        }
    }
}

void chooseColor() {
    const char *colorOptions[] = {"Yellow", "White", "Magenta", "Cyan", "Red", "Green"};
    int colorChoice = 5;
    int key;

    while (1) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 15, "===============================");
        mvprintw(2, 20, "Select Player Color");
        mvprintw(3, 15, "===============================");
        attroff(A_BOLD);

        for (int i = 0; i < 6; i++) {
            int y = 6 + i * 2;
            if (i == colorChoice) attron(A_REVERSE);
            mvprintw(y, 18, "%s", colorOptions[i]);
            attroff(A_REVERSE);
        }

        mvprintw(18, 15, "[ ↑ / ↓ to change | ENTER to confirm | ESC to go back ]");

        key = getch();
        switch (key) {
            case KEY_UP:
                colorChoice = (colorChoice - 1 + 6) % 6;
                break;
            case KEY_DOWN:
                colorChoice = (colorChoice + 1) % 6;
                break;
            case 10: // ENTER
                playerColor = colorChoice + 1;
                return;
            case 27: // ESC
                return;
        }
    }
}

void settingsMenu() {
    int selectedOption = 0;
    int key;

    while (1) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 15, "===============================");
        mvprintw(2, 20, "Settings");
        mvprintw(3, 15, "===============================");
        attroff(A_BOLD);

        mvprintw(6, 18, "%s Difficulty", (selectedOption == 0) ? ">>" : "  ");
        mvprintw(8, 18, "%s Player Color", (selectedOption == 1) ? ">>" : "  ");
        mvprintw(10, 18, "%s Back", (selectedOption == 2) ? ">>" : "  ");

        key = getch();
        switch (key) {
            case KEY_UP:
                selectedOption = (selectedOption - 1 + 3) % 3;
                break;
            case KEY_DOWN:
                selectedOption = (selectedOption + 1) % 3;
                break;
            case 10: // ENTER
                if (selectedOption == 0) chooseDifficulty();
                else if (selectedOption == 1) chooseColor();
                else if (selectedOption == 2) return;
                break;
            case 27: // ESC
                return;
        }
    }
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

int isEmailValid(const char *email) {
    const char *at = strchr(email, '@');  
    const char *dot = strrchr(email, '.');  
    return (at && dot && (dot > at) && (at != email) && (*(dot + 1) != '\0'));
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

            //printf("Comparing:\nUser Input: '%s' | File Data: '%s'\n", inputUsername, storedUsername);
            //printf("Password Input: '%s' | File Password: '%s'\n", inputPassword, storedPassword);

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
    loadGame();
    refresh();
    getch();
}

void gameMenu() {
    const char *options[] = {
        "(*) Start New Game",
        "(*) Continue Previous Game",
        "(*) setting"
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
                    case 2: settingsMenu(); break;
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

            strcpy(currentUsername, username); 
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
    gameMenu();
    //start();
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
        "(*) Login as Guest ",   
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

void showProfile() {
    clear();
    mvprintw(5, 5, "%s", "This section will be generated soon: Profile");
    refresh();
    getch();
}

void showLeaderboards() {
    clear();
    showLeaderboard();
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
        "4. Scoreboard",
        "5. Exit"
    };
    const int numOptions = sizeof(options) / sizeof(options[0]);

    int choice = 0; 
    int key;

    const int startRow = 6;   
    const int startCol = 10;  
    const int rowSpacing = 1; 

    while (1) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 15, "===============================");
        mvprintw(2, 20, "Main Menu");
        mvprintw(3, 15, "===============================");
        attroff(A_BOLD);
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
                    case 3: showLeaderboards(); break;
                    case 4: exitGame(); return;
                }
                break;
        }
    }
}

int main() {
    setlocale(LC_ALL,"C.UTF-8");
    loadPlayerData();
    printf("%d", playerCount);
    initscr();            
    cbreak();              
    noecho();
    curs_set(0);             
    keypad(stdscr, TRUE); 
    setupcolor();
    mainMenu();           

    endwin();             
    return 0;
}




