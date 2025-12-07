#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue> 
#include <algorithm>
#include <iomanip>

using namespace std;

// =====================================================
//              FORWARD DECLARATIONS
// =====================================================
struct Player;
class MapGraph;
class ZombieSystem;
class Inventory;

// Global helper declarations
void useJunkAtCurrentNode(Player& player, Inventory& inv, ZombieSystem& zsys);
void usePebble(Player& player, Inventory& inv, MapGraph& map, ZombieSystem& zsys);
void useTwig(Player& player, Inventory& inv, MapGraph& map, ZombieSystem& zsys);
void useEnergyDrink(Player& player, Inventory& inv, bool& playerAlive);
void usePills(Player& player, Inventory& inv, bool& playerAlive);
void useFood(Player& player, Inventory& inv, string itemName, int hpGain);
void useCloth(Player& player, Inventory& inv);
void useAxeOnBridge(MapGraph& map, Player& player, Inventory& inv);

// =====================================================
//              UI HELPERS
// =====================================================
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause() {
    cout << "\n[Press Enter to continue...]";
    cin.ignore();
    cin.get();
}

void printSeparator() {
    cout << "==================================================================\n";
}

void printHeader(string title) {
    cout << "\n>> " << title << " <<\n";
    printSeparator();
}

void drawGameTitle() {
    clearScreen();
    cout << R"(
   _____ _    _ ______      _______      __     _      
  / ____| |  | |  __ \ \    / /_   _\ \    / / /\   | |     
 | (___ | |  | | |__) \ \  / /  | |  \ \  / / /  \  | |     
  \___ \| |  | |  _  / \ \/ /   | |   \ \/ / / /\ \ | |     
  ____) | |__| | | \ \  \  /   _| |_   \  / / ____ \| |____ 
 |_____/ \____/|_|  \_\  \/   |_____|   \/ /_/    \_\______|
    ZOMBIE OUTBREAK SIMULATION | DATA STRUCTURES PROJECT
    )" << "\n";
    printSeparator();
    cout << "Welcome survivor. Find the PIN and User ID to enter the Safe Zone.\n";
    pause();
}

// =====================================================
//              ENUMS AND STRUCTS
// =====================================================
enum Location {
    TOWN_HALL, HOME, OFFICE, LAB, GAS_STATION, PARK, BUS_STOP, STORE, SCHOOL, CAFE, POLICE_STATION,
    HOSPITAL,
    BRIDGE,
    SAFE_ZONE,
    COUNT // must stay last
};

struct Node {
    int vertex;
    Node* next;
    Node(int v) { vertex = v; next = NULL; }
};

// Short names for the Map
string getShortName(Location loc) {
    switch (loc) {
    case TOWN_HALL: return "TWN HALL";
    case HOME: return "  HOME  ";
    case OFFICE: return " OFFICE ";
    case LAB: return "  LAB   ";
    case GAS_STATION: return "GAS STN ";
    case PARK: return "  PARK  ";
    case BUS_STOP: return "BUS STOP";
    case STORE: return " STORE  ";
    case SCHOOL: return " SCHOOL ";
    case CAFE: return "  CAFE  ";
    case POLICE_STATION: return "POLICE  ";
    case HOSPITAL: return "HOSPITAL";
    case BRIDGE: return " BRIDGE ";
    case SAFE_ZONE: return "SAFEZONE";
    default: return "UNKNOWN ";
    }
}

string locationToString(Location loc) {
    switch (loc) {
    case TOWN_HALL: return "Town Hall";
    case HOME: return "Home";
    case OFFICE: return "Office";
    case LAB: return "Lab";
    case GAS_STATION: return "Gas Station";
    case PARK: return "Park";
    case BUS_STOP: return "Bus Stop";
    case STORE: return "Store";
    case SCHOOL: return "School";
    case CAFE: return "Cafe";
    case POLICE_STATION: return "Police Station";
    case HOSPITAL: return "Hospital";
    case BRIDGE: return "Bridge";
    case SAFE_ZONE: return "Safe Zone";
    default: return "Unknown";
    }
}

struct ItemProb {
    string name;
    double probability;
};

struct Player {
    Location currentLocation;
    int timeMinutes;
    int stamina;
    int speedBoostTimer; 
    int hp;
    bool isPoisoned;
    bool isScratched;
    int scratchMinuteBuffer;
    int clothEffectMinutesLeft;
    int adrenalineMovesLeft;
};

// =====================================================
//              INVENTORY CLASS
// =====================================================
struct InvNode {
    string name;
    string description;
    int quantity;
    InvNode* prev;
    InvNode* next;
    InvNode(const string& n, const string& d, int q)
        : name(n), description(d), quantity(q), prev(NULL), next(NULL) {}
};

class Inventory {
private:
    InvNode* head;
    InvNode* tail;
    InvNode* current;
    int capacity;
    int usedSlots;
    bool hasBackpack;

public:
    Inventory(int cap = 8) {
        head = tail = current = NULL;
        capacity = cap;
        usedSlots = 0;
        hasBackpack = false;
    }

    bool isEmpty() const { return head == NULL; }
    bool isFull() const { return usedSlots >= capacity; }
    int getCapacity() const { return capacity; }
    int getUsedSlots() const { return usedSlots; }
    
    void applyBackpack() {
        if (hasBackpack) {
            cout << ">> [Inventory] Capacity is already " << capacity << ".\n";
            return;
        }
        capacity = 12;
        hasBackpack = true;
        cout << ">> [Inventory] Backpack obtained! Capacity increased to 12 slots.\n";
    }

    bool contains(const string& itemName) {
        InvNode* temp = head;
        while (temp) {
            if (temp->name == itemName) return true;
            temp = temp->next;
        }
        return false;
    }

    void addItem(const string& name, const string& desc, int quantity = 1) {
        InvNode* temp = head;
        while (temp != NULL) {
            if (temp->name == name) {
                temp->quantity += quantity;
                cout << ">> [Inventory] Stacked more of " << name << ". New qty: " << temp->quantity << "\n";
                return;
            }
            temp = temp->next;
        }

        if (isFull()) {
            cout << ">> [Inventory] Full! Cannot add: " << name << "\n";
            return;
        }

        InvNode* node = new InvNode(name, desc, quantity);
        if (!head) { head = tail = node; }
        else { tail->next = node; node->prev = tail; tail = node; }
        usedSlots++;
        cout << ">> [Inventory] Added: " << name << " (x" << quantity << ")\n";
    }

    void deleteNode(InvNode* node) {
        if (!node) return;
        cout << ">> [Inventory] Discarded: " << node->name << "\n";
        if (node->prev) node->prev->next = node->next;
        else head = node->next;
        if (node->next) node->next->prev = node->prev;
        else tail = node->prev;
        if (current == node) {
            if (node->next) current = node->next;
            else current = node->prev;
        }
        delete node;
        usedSlots--;
    }

    void deleteCurrent() {
        if (!current) { cout << "Nothing selected.\n"; return; }
        deleteNode(current);
    }

    void showCurrent() {
        if (!current) { cout << "     (No item selected)\n"; return; }
        cout << "\n   > SELECTED: " << current->name << " (x" << current->quantity << ")\n";
        cout << "     Desc: " << current->description << "\n";
    }

    void moveNext() {
        if (!current) current = head;
        else if (current->next) current = current->next;
        else cout << "   (End of list)\n";
    }

    void moveBack() {
        if (!current) current = head;
        else if (current->prev) current = current->prev;
        else cout << "   (Start of list)\n";
    }

    bool consumeOne(const string& itemName) {
        InvNode* temp = head;
        while (temp) {
            if (temp->name == itemName) {
                temp->quantity--;
                if (temp->quantity <= 0) deleteNode(temp);
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    bool consumeMany(const string& itemName, int count) {
        int remaining = count;
        InvNode* temp = head;
        while (temp && remaining > 0) {
            if (temp->name == itemName) {
                if (temp->quantity > remaining) {
                    temp->quantity -= remaining;
                    remaining = 0;
                }
                else {
                    remaining -= temp->quantity;
                    InvNode* toDelete = temp;
                    temp = temp->next;
                    deleteNode(toDelete);
                    continue;
                }
            }
            if (temp) temp = temp->next;
        }
        return remaining == 0;
    }

    int countItem(const string& itemName) const {
        int total = 0;
        InvNode* temp = head;
        while (temp) {
            if (temp->name == itemName) total += temp->quantity;
            temp = temp->next;
        }
        return total;
    }

    void listItemsWithIndex() {
        cout << "\n[Inventory] Items:\n";
        InvNode* temp = head;
        int idx = 1;
        while (temp) {
            cout << idx << ". " << temp->name << " (x" << temp->quantity << ")\n";
            temp = temp->next;
            idx++;
        }
        if (idx == 1) cout << "(empty)\n";
        cout << "\n";
    }

    void deleteByIndex(int index) {
        InvNode* temp = head;
        int idx = 1;
        while (temp && idx < index) { temp = temp->next; idx++; }
        if (!temp) return;
        deleteNode(temp);
    }

    void openMenu(Player& player, MapGraph& map, ZombieSystem& zsys, bool& playerAlive);
    InvNode* getCurrentNode() { return current; }
    void resetCurrent() { if(!current) current = head; }
};

// =====================================================
//              OTHER SYSTEMS
// =====================================================

struct MoveNodeDLL {
    Location locBefore;
    int timeBefore;
    MoveNodeDLL* prev;
    MoveNodeDLL* next;
    MoveNodeDLL(Location l, int t) : locBefore(l), timeBefore(t), prev(NULL), next(NULL) {}
};

class MoveLog {
private:
    MoveNodeDLL* head;
    MoveNodeDLL* tail;
public:
    MoveLog() { head = tail = NULL; }
    bool isEmpty() const { return tail == NULL; }
    void push(Location locBefore, int timeBefore) {
        MoveNodeDLL* node = new MoveNodeDLL(locBefore, timeBefore);
        if (!head) head = tail = node;
        else { tail->next = node; node->prev = tail; tail = node; }
    }
    bool pop(Location& locOut, int& timeOut) {
        if (!tail) return false;
        MoveNodeDLL* node = tail;
        locOut = node->locBefore;
        timeOut = node->timeBefore;
        tail = node->prev;
        if (tail) tail->next = NULL;
        else head = NULL;
        delete node;
        return true;
    }
};

// =====================================================
//                  ZOMBIE SYSTEM
// =====================================================
struct InfectionNode {
    Location loc;
    InfectionNode* left;
    InfectionNode* right;
    InfectionNode(Location l) : loc(l), left(nullptr), right(nullptr) {}
};

struct ZombieHorde {
    int id;
    Location currentLocation;
    int infectionRate;
    InfectionNode* treeNode;
    ZombieHorde(int _id, Location loc, int rate, InfectionNode* node)
        : id(_id), currentLocation(loc), infectionRate(rate), treeNode(node) {}
};

class ZombieSystem {
private:
    vector<ZombieHorde> hordes;
    int junkBlocks[COUNT];
    int distractionTurns[COUNT];
    int nextId;
    bool infected[COUNT];
    InfectionNode* infectionRoot;
    MapGraph* map; 

public:
    ZombieSystem(MapGraph* m); 

    void addInitialHorde(Location loc) {
        InfectionNode* node = new InfectionNode(loc);
        if (!infectionRoot) infectionRoot = node;
        ZombieHorde h(nextId++, loc, 5, node); // Starts at 5%
        hordes.push_back(h);
        infected[loc] = true;
    }

    void applyJunk(Location loc) {
        junkBlocks[loc] = 2;
        cout << ">> [Junk] The path is blocked. Zombies will be repelled for 2 turns.\n";
    }

    void applyDistraction(Location loc, int duration) {
        distractionTurns[loc] = duration;
        string type = (duration == 1) ? "Twig" : "Pebble";
        cout << ">> [" << type << "] You created a distraction at " << locationToString(loc) << ".\n";
        cout << "   Zombies will be attracted there for " << duration << " turn(s).\n";
    }

    int countHordesAt(Location loc) {
        int c = 0;
        for (auto& h : hordes) if (h.currentLocation == loc) c++;
        return c;
    }

    void removeAllHordesAt(Location loc) {
        for (int i = (int)hordes.size() - 1; i >= 0; --i) {
            if (hordes[i].currentLocation == loc) hordes.erase(hordes.begin() + i);
        }
    }

    void removeOneHordeAt(Location loc) {
        for (int i = 0; i < (int)hordes.size(); ++i) {
            if (hordes[i].currentLocation == loc) {
                cout << ">> [Combat] Horde " << hordes[i].id << " eliminated.\n";
                hordes.erase(hordes.begin() + i);
                return;
            }
        }
    }

    bool isHordeAt(Location loc) const {
        for (const auto& h : hordes) if (h.currentLocation == loc) return true;
        return false;
    }

    void simulateHour();
    void moveHordesToward(Location target);
    
    private:
    bool hasDistractionAnywhere();
    Location getStepTowardsDistraction(Location start);
    void moveHordeOneStep(ZombieHorde& zombie, vector<ZombieHorde>& newHordes);
    void checkInfection(ZombieHorde& zombie, vector<ZombieHorde>& newHordes);
};

// =====================================================
//              MAP GRAPH
// =====================================================
class MapGraph {
private:
    Node* adj[COUNT];
    vector<ItemProb> itemTable[COUNT];
    bool bridgeUnlocked;

    // Returns a 2-line string array for map display
    void getMapNodeVisual(Location loc, Player& p, ZombieSystem& zsys, string& line1, string& line2) {
        bool playerHere = (p.currentLocation == loc);
        int zombiesHere = zsys.countHordesAt(loc);
        
        // Line 1: Name centered
        line1 = "[" + getShortName(loc) + "]";

        // Line 2: Status
        if (playerHere && zombiesHere > 0) line2 = "  #!!#  ";
        else if (playerHere)               line2 = " [YOU]  ";
        else if (zombiesHere > 0)          line2 = "  !Z!   ";
        else                               line2 = "  ....  ";
    }

public:
    MapGraph() {
        for (int i = 0; i < COUNT; i++) adj[i] = NULL;
        bridgeUnlocked = false;
        buildDefaultMap();
        initItemProbabilities();
    }

    void addEdge(Location a, Location b) {
        int u = a, v = b;
        Node* newNode = new Node(v);
        newNode->next = adj[u]; adj[u] = newNode;
        Node* newNode2 = new Node(u);
        newNode2->next = adj[v]; adj[v] = newNode2;
    }

    // --- NEW PRETTY MAP ---
    void displayPrettyMap(Player& p, ZombieSystem& zsys) {
        clearScreen();
        printHeader("SURVIVAL MAP");

        // We prepare visual buffers for the nodes
        string nHome[2], nGas[2], nStore[2], nOffice[2], nPolice[2];
        string nPark[2], nHall[2], nCafe[2], nSchool[2], nHosp[2];
        string nBus[2], nLab[2], nBridge[2], nSafe[2];

        getMapNodeVisual(HOME, p, zsys, nHome[0], nHome[1]);
        getMapNodeVisual(GAS_STATION, p, zsys, nGas[0], nGas[1]);
        getMapNodeVisual(STORE, p, zsys, nStore[0], nStore[1]);
        getMapNodeVisual(OFFICE, p, zsys, nOffice[0], nOffice[1]);
        getMapNodeVisual(POLICE_STATION, p, zsys, nPolice[0], nPolice[1]);

        getMapNodeVisual(PARK, p, zsys, nPark[0], nPark[1]);
        getMapNodeVisual(TOWN_HALL, p, zsys, nHall[0], nHall[1]);
        getMapNodeVisual(CAFE, p, zsys, nCafe[0], nCafe[1]);
        getMapNodeVisual(SCHOOL, p, zsys, nSchool[0], nSchool[1]);
        getMapNodeVisual(HOSPITAL, p, zsys, nHosp[0], nHosp[1]);

        getMapNodeVisual(BUS_STOP, p, zsys, nBus[0], nBus[1]);
        getMapNodeVisual(LAB, p, zsys, nLab[0], nLab[1]);
        getMapNodeVisual(BRIDGE, p, zsys, nBridge[0], nBridge[1]);
        getMapNodeVisual(SAFE_ZONE, p, zsys, nSafe[0], nSafe[1]);

        // DRAWING
        cout << "      " << nHome[0] << "----" << nGas[0] << "----" << nStore[0] << "----" << nOffice[0] << "----" << nPolice[0] << "\n";
        cout << "      " << nHome[1] << "    " << nGas[1] << "    " << nStore[1] << "    " << nOffice[1] << "    " << nPolice[1] << "\n";
        cout << "         |          /      |           |           |           |\n";
        cout << "         |         /       |           |           |           |\n";
        cout << "      " << nPark[0] << "----" << nHall[0] << "----" << nCafe[0] << "----" << nSchool[0] << "----" << nHosp[0] << "\n";
        cout << "      " << nPark[1] << "    " << nHall[1] << "    " << nCafe[1] << "    " << nSchool[1] << "    " << nHosp[1] << "\n";
        cout << "         |                       |                                  \n";
        cout << "         |                       |                                  \n";
        cout << "      " << nBus[0] << "            " << nLab[0] << "----" << nBridge[0] << "====" << nSafe[0] << "\n";
        cout << "      " << nBus[1] << "            " << nLab[1] << "    " << nBridge[1] << "    " << nSafe[1] << "\n";
        
        cout << "\n";
        cout << "  LEGEND:\n";
        cout << "  [YOU] Current Loc   !Z! Zombie Present   #!!# Danger (Run!)\n";
        cout << "  ----  Path          ==== Barricade       .... Empty\n";
        printSeparator();
        pause();
    }

    Node* getNeighbors(Location loc) { return adj[loc]; }

    bool isConnected(Location from, Location to) {
        Node* temp = adj[from];
        while (temp != NULL) {
            if (temp->vertex == to) return true;
            temp = temp->next;
        }
        return false;
    }

    bool movePlayer(Player& player, Location dest, MoveLog& log, int moveCost) {
        if (!isConnected(player.currentLocation, dest)) {
            cout << "You cannot move from " << locationToString(player.currentLocation)
                << " to " << locationToString(dest) << ".\n";
            return false;
        }
        log.push(player.currentLocation, player.timeMinutes);
        player.currentLocation = dest;
        player.timeMinutes += moveCost;
        cout << "\n>> MOVING TO " << locationToString(dest) << "... (+" << moveCost << "m)\n";
        return true;
    }

    ItemProb* scavenge(Location loc) {
        double sum = 0;
        for (auto& ip : itemTable[loc]) sum += ip.probability;
        int roll = rand() % 100;
        double cumulative = 0;
        for (auto& ip : itemTable[loc]) {
            cumulative += ip.probability;
            if (roll < cumulative) return &ip;
        }
        return nullptr;
    }

    void removeItemChance(Location loc, ItemProb* item) {
        if (!item) return;
        item->probability = 0;
    }

    void unlockBridgeToSafeZone() {
        if (bridgeUnlocked) { cout << ">> Bridge is already cleared.\n"; return; }
        addEdge(BRIDGE, SAFE_ZONE);
        bridgeUnlocked = true;
        cout << ">> You chop down the barricade. The path to the Safe Zone is OPEN!\n";
    }

    bool isBridgeOpen() const { return bridgeUnlocked; }

private:
    void buildDefaultMap() {
        // Clear (if needed)
        for(int i=0; i<COUNT; i++) adj[i] = NULL;

        // Base Connections
        addEdge(TOWN_HALL, HOME); addEdge(TOWN_HALL, PARK); addEdge(TOWN_HALL, OFFICE); addEdge(TOWN_HALL, CAFE);
        addEdge(HOME, PARK); addEdge(HOME, GAS_STATION);
        addEdge(OFFICE, STORE); addEdge(OFFICE, POLICE_STATION);
        addEdge(LAB, HOSPITAL); addEdge(LAB, BRIDGE);
        addEdge(PARK, HOSPITAL); addEdge(CAFE, STORE);
        addEdge(BUS_STOP, PARK); addEdge(BUS_STOP, SCHOOL); addEdge(BUS_STOP, TOWN_HALL);
        addEdge(SCHOOL, STORE); addEdge(SCHOOL, CAFE);
        addEdge(STORE, GAS_STATION);
        addEdge(POLICE_STATION, HOSPITAL);

        // Requested Accessibility Changes
        addEdge(PARK, GAS_STATION);
        // addEdge(OFFICE, HOSPITAL); // Removed per previous prompt
        
        // 1. Reset
        for(int i=0; i<COUNT; i++) adj[i] = NULL;

        // 2. Standard Base
        addEdge(TOWN_HALL, HOME); addEdge(TOWN_HALL, PARK); addEdge(TOWN_HALL, OFFICE); addEdge(TOWN_HALL, CAFE);
        addEdge(HOME, PARK); addEdge(HOME, GAS_STATION);
        addEdge(OFFICE, STORE); addEdge(OFFICE, POLICE_STATION);
        addEdge(LAB, HOSPITAL); addEdge(LAB, BRIDGE);
        // addEdge(PARK, HOSPITAL); // Removing weird diagonals to make visual cleaner if needed, but keeping logic consistent with visual
        // Let's stick strictly to visual layout connections
        
        // Visual Layout Row 1: Home-Gas-Store-Office-Police
        addEdge(HOME, GAS_STATION);
        addEdge(GAS_STATION, STORE);
        addEdge(STORE, OFFICE);
        addEdge(OFFICE, POLICE_STATION);

        // Visual Layout Row 2: Park-Hall-Cafe-School-Hospital
        addEdge(PARK, TOWN_HALL);
        addEdge(TOWN_HALL, CAFE);
        addEdge(CAFE, SCHOOL);
        addEdge(SCHOOL, HOSPITAL);

        // Visual Verticals
        addEdge(HOME, PARK);
        addEdge(GAS_STATION, TOWN_HALL); // Park to Gas diagonal logic handled here via Town Hall or explicit diagonal below
        addEdge(STORE, CAFE);
        addEdge(OFFICE, SCHOOL);
        addEdge(POLICE_STATION, HOSPITAL);

        // Visual Row 3
        addEdge(PARK, BUS_STOP);
        addEdge(CAFE, LAB);
        addEdge(LAB, BRIDGE);

        // Explicit Diagonals mentioned in visual
        addEdge(PARK, GAS_STATION); 
    }

    void addItem(Location loc, const string& name, double prob) {
        itemTable[loc].push_back({ name, prob });
    }

    void initItemProbabilities() {
        addItem(HOME, "Bread", 30); addItem(HOME, "Pills", 20); addItem(HOME, "Apple", 20);
        addItem(HOME, "User ID", 15); addItem(HOME, "Car Keys", 5);
        addItem(GAS_STATION, "Petrol", 60); addItem(GAS_STATION, "Petrol", 60); addItem(GAS_STATION, "Petrol", 60); 
        addItem(GAS_STATION, "Cloth", 20); addItem(GAS_STATION, "Junk", 10);
        addItem(PARK, "Apple", 25); addItem(PARK, "Energy Drink", 15); addItem(PARK, "Twig", 15); addItem(PARK, "Pebble", 35);
        addItem(BUS_STOP, "Pebble", 70); addItem(BUS_STOP, "Junk", 15);
        addItem(OFFICE, "Pills", 20); addItem(OFFICE, "User ID", 40); addItem(OFFICE, "Junk", 30);
        addItem(STORE, "Bread", 20); addItem(STORE, "Apple", 15); addItem(STORE, "Energy Drink", 15); addItem(STORE, "Axe", 35); addItem(STORE, "Junk", 10);
        addItem(SCHOOL, "Apple", 15); addItem(SCHOOL, "Backpack", 30); addItem(SCHOOL, "Junk", 20); addItem(SCHOOL, "Book", 25);
        addItem(TOWN_HALL, "Bread", 40); addItem(TOWN_HALL, "Cloth", 25); addItem(TOWN_HALL, "Pebble", 20);
        addItem(CAFE, "Energy Drink", 45); addItem(CAFE, "Bread", 45); 
        addItem(POLICE_STATION, "Gun", 50); addItem(POLICE_STATION, "Ammo", 25); addItem(POLICE_STATION, "Energy Drink", 15);
        addItem(HOSPITAL, "Apple", 20); addItem(HOSPITAL, "Cloth", 30); addItem(HOSPITAL, "First Aid", 40);
        addItem(LAB, "PIN", 60); addItem(LAB, "Ammo", 15); addItem(LAB, "First Aid", 15); addItem(LAB, "Pebble", 10);
        addItem(BRIDGE, "Pebble", 25); addItem(BRIDGE, "Twig", 20); addItem(BRIDGE, "Ammo", 15); addItem(BRIDGE, "First Aid", 30);
    }
};

// =====================================================
//          ZOMBIE SYSTEM IMPLEMENTATION
// =====================================================
ZombieSystem::ZombieSystem(MapGraph* m) {
    map = m;
    nextId = 1;
    for (int i = 0; i < COUNT; ++i) {
        junkBlocks[i] = 0;
        distractionTurns[i] = 0;
        infected[i] = false;
    }
    infectionRoot = nullptr;
}

bool ZombieSystem::hasDistractionAnywhere() {
    for(int i=0; i<COUNT; i++) if(distractionTurns[i] > 0) return true;
    return false;
}

Location ZombieSystem::getStepTowardsDistraction(Location start) {
    if (distractionTurns[start] > 0) return start;
    int distance[COUNT];
    int parent[COUNT];
    bool visited[COUNT];
    for(int i=0; i<COUNT; i++) { distance[i] = 9999; visited[i] = false; parent[i] = -1; }
    queue<Location> q;
    q.push(start);
    visited[start] = true;
    distance[start] = 0;
    Location targetFound = COUNT;
    while(!q.empty()) {
        Location u = q.front(); q.pop();
        if (distractionTurns[u] > 0) { targetFound = u; break; }
        Node* n = map->getNeighbors(u);
        while(n) {
            Location v = (Location)n->vertex;
            if (!visited[v]) {
                visited[v] = true;
                distance[v] = distance[u] + 1;
                parent[v] = u;
                q.push(v);
            }
            n = n->next;
        }
    }
    if (targetFound == COUNT) return start;
    Location curr = targetFound;
    while(parent[curr] != -1 && parent[curr] != start) { curr = (Location)parent[curr]; }
    return curr;
}

void ZombieSystem::simulateHour() {
    cout << "\n--- ZOMBIES MOVE (1 HOUR) ---\n";
    vector<ZombieHorde> newHordes;
    for (auto& h : hordes) {
        moveHordeOneStep(h, newHordes);
    }
    for (auto& nh : newHordes) {
        hordes.push_back(nh);
    }
    int countAt[COUNT] = { 0 };
    for (auto& h : hordes) countAt[h.currentLocation]++;
    for (auto& h : hordes) {
        if (countAt[h.currentLocation] > 1) h.infectionRate = 5; 
    }
    for (int i = 0; i < COUNT; ++i) {
        if (junkBlocks[i] > 0) junkBlocks[i]--;
        if (distractionTurns[i] > 0) distractionTurns[i]--;
    }
}

void ZombieSystem::moveHordesToward(Location target) {
    cout << ">> [Scent] The wind shifts... Zombies smell you.\n";
    for (auto& zombie : hordes) {
        if (distractionTurns[zombie.currentLocation] > 0 || hasDistractionAnywhere()) {
            continue; 
        }
        if (zombie.currentLocation == target) continue;
        Node* temp = map->getNeighbors(zombie.currentLocation);
        while (temp) {
            Location neigh = (Location)temp->vertex;
            if (neigh == target && junkBlocks[neigh] == 0) {
                zombie.currentLocation = target;
                zombie.infectionRate += 2;
                if (zombie.infectionRate > 100) zombie.infectionRate = 100;
                cout << "   ! [Horde " << zombie.id << "] rushes to your location! (Infection: " << zombie.infectionRate << "%)\n";
                break;
            }
            temp = temp->next;
        }
    }
}

void ZombieSystem::moveHordeOneStep(ZombieHorde& zombie, vector<ZombieHorde>& newHordes) {
    if (distractionTurns[zombie.currentLocation] > 0) {
        cout << "   - [Horde " << zombie.id << "] is distracted by noise at " << locationToString(zombie.currentLocation) << ".\n";
        return;
    }
    if (hasDistractionAnywhere()) {
        Location step = getStepTowardsDistraction(zombie.currentLocation);
        if (step != zombie.currentLocation) {
            if (junkBlocks[step] == 0) {
                zombie.currentLocation = step;
                cout << "   - [Horde " << zombie.id << "] moves towards noise at " << locationToString(step) << ".\n";
                checkInfection(zombie, newHordes);
                return; 
            }
        }
    }
    zombie.infectionRate += 2; 
    if (zombie.infectionRate > 100) zombie.infectionRate = 100;

    int roll = rand() % 100;
    if (roll < 15) {
        cout << "   - [Horde " << zombie.id << "] is resting at " << locationToString(zombie.currentLocation) << " (Inf: " << zombie.infectionRate << "%).\n";
        return;
    }
    vector<Location> neighbors;
    Node* temp = map->getNeighbors(zombie.currentLocation);
    while (temp) {
        Location neigh = (Location)temp->vertex;
        if (junkBlocks[neigh] == 0) neighbors.push_back(neigh);
        temp = temp->next;
    }
    if (neighbors.empty()) return;
    int idx = rand() % neighbors.size();
    Location newLoc = neighbors[idx];
    zombie.currentLocation = newLoc;
    cout << "   - [Horde " << zombie.id << "] shambles to " << locationToString(newLoc) << " (Inf: " << zombie.infectionRate << "%).\n";
    checkInfection(zombie, newHordes);
}

void ZombieSystem::checkInfection(ZombieHorde& zombie, vector<ZombieHorde>& newHordes) {
    if (!infected[zombie.currentLocation]) {
        int infectRoll = rand() % 100;
        if (infectRoll < zombie.infectionRate) {
            cout << "     >> " << locationToString(zombie.currentLocation) << " HAS BEEN INFECTED!\n";
            infected[zombie.currentLocation] = true;
            zombie.infectionRate = 5;
            InfectionNode* child = new InfectionNode(zombie.currentLocation);
            if (!zombie.treeNode->left) zombie.treeNode->left = child;
            else if (!zombie.treeNode->right) zombie.treeNode->right = child;
            ZombieHorde splitHorde(nextId++, zombie.currentLocation, 5, child);
            newHordes.push_back(splitHorde);
            cout << "     >> WARNING: A new horde has spawned!\n";
        }
    }
}

// =====================================================
// GLOBAL ACTIONS IMPLEMENTATION
// =====================================================

void advanceZombies(ZombieSystem& zsys, int& zombieMinuteBuffer, int deltaMinutes) {
    zombieMinuteBuffer += deltaMinutes;
    while (zombieMinuteBuffer >= 60) {
        zsys.simulateHour();
        zombieMinuteBuffer -= 60;
    }
}

void applyTimeToPlayer(Player& player, int deltaMinutes, bool& playerAlive) {
    if (player.clothEffectMinutesLeft > 0) {
        player.clothEffectMinutesLeft -= deltaMinutes;
        if (player.clothEffectMinutesLeft < 0) player.clothEffectMinutesLeft = 0;
    }
    player.scratchMinuteBuffer += deltaMinutes;
    while (player.scratchMinuteBuffer >= 60) {
        player.scratchMinuteBuffer -= 60;
        if (player.isScratched && player.clothEffectMinutesLeft <= 0) {
            player.hp -= 5;
            cout << ">> [Status] Your scratches hurt. -5 HP\n";
        }
    }
    if (player.hp <= 0) {
        cout << ">> [Status] You succumb to your injuries...\n";
        playerAlive = false;
    }
}

void useJunkAtCurrentNode(Player& player, Inventory& inv, ZombieSystem& zsys) {
    if (inv.consumeOne("Junk")) {
        zsys.applyJunk(player.currentLocation);
    } else {
        cout << ">> You don't have any Junk.\n";
    }
}

void usePebble(Player& player, Inventory& inv, MapGraph& map, ZombieSystem& zsys) {
    if (!inv.consumeOne("Pebble")) { cout << ">> You don't have a pebble.\n"; return; }
    cout << "\n[Targeting] Where do you want to throw the pebble?\n";
    vector<Location> targets;
    targets.push_back(player.currentLocation);
    Node* n = map.getNeighbors(player.currentLocation);
    while (n) { targets.push_back((Location)n->vertex); n = n->next; }
    for (size_t i = 0; i < targets.size(); i++) {
        cout << i + 1 << ". " << locationToString(targets[i]);
        if (targets[i] == player.currentLocation) cout << " (Current Location)";
        cout << "\n";
    }
    int choice; cout << "Enter choice: "; cin >> choice;
    if (choice < 1 || choice > (int)targets.size()) { cout << ">> Wasted.\n"; return; }
    zsys.applyDistraction(targets[choice - 1], 2);
}

void useTwig(Player& player, Inventory& inv, MapGraph& map, ZombieSystem& zsys) {
    if (!inv.consumeOne("Twig")) { cout << ">> You don't have a twig.\n"; return; }
    cout << "\n[Targeting] Where do you want to snap the twig?\n";
    vector<Location> targets;
    targets.push_back(player.currentLocation);
    Node* n = map.getNeighbors(player.currentLocation);
    while (n) { targets.push_back((Location)n->vertex); n = n->next; }
    for (size_t i = 0; i < targets.size(); i++) {
        cout << i + 1 << ". " << locationToString(targets[i]);
        if (targets[i] == player.currentLocation) cout << " (Current Location)";
        cout << "\n";
    }
    int choice; cout << "Enter choice: "; cin >> choice;
    if (choice < 1 || choice > (int)targets.size()) { cout << ">> Wasted.\n"; return; }
    zsys.applyDistraction(targets[choice - 1], 1);
}

void useEnergyDrink(Player& player, Inventory& inv, bool& playerAlive) {
    if (!inv.consumeOne("Energy Drink")) { cout << "None left.\n"; return; }
    bool risk = (player.speedBoostTimer > 0);
    player.stamina += 50; if (player.stamina > 100) player.stamina = 100;
    player.speedBoostTimer += 30;
    cout << ">> [Energy Drink] Gulp. Speed Boost +30m.\n";
    if (risk) {
        if (rand() % 100 < 50) { cout << ">>> OVERDOSE! You die.\n"; playerAlive = false; }
        else cout << ">>> Dizzy, but alive.\n";
    }
}

void usePills(Player& player, Inventory& inv, bool& playerAlive) {
    if (!inv.consumeOne("Pills")) { cout << "None left.\n"; return; }
    bool risk = (player.speedBoostTimer > 0);
    player.stamina += 50; if (player.stamina > 100) player.stamina = 100;
    player.speedBoostTimer += 60;
    cout << ">> [Pills] Swallowed. Speed Boost +60m.\n";
    if (risk) {
        if (rand() % 100 < 60) { cout << ">>> OVERDOSE! You die.\n"; playerAlive = false; }
        else { cout << ">>> CRASH! Stamina drained.\n"; player.stamina = 0; player.speedBoostTimer = 0; }
    }
}

void useFood(Player& player, Inventory& inv, string itemName, int hpGain) {
    if (!inv.consumeOne(itemName)) { cout << ">> You don't have " << itemName << ".\n"; return; }
    player.hp += hpGain;
    if (player.hp > 100) player.hp = 100;
    cout << ">> [Food] You eat the " << itemName << ". HP +" << hpGain << " (Current: " << player.hp << ").\n";
}

void useCloth(Player& player, Inventory& inv) {
    if (!inv.consumeOne("Cloth")) { cout << "None left.\n"; return; }
    player.clothEffectMinutesLeft = 120;
    cout << ">> [Cloth] Wounds bandaged (protected for 2 hrs).\n";
}

void useAxeOnBridge(MapGraph& map, Player& player, Inventory& inv) {
    if (player.currentLocation != BRIDGE) { cout << ">> Must be at Bridge.\n"; return; }
    if (map.isBridgeOpen()) { cout << ">> Already open.\n"; return; }
    if (!inv.consumeOne("Axe")) { cout << ">> No Axe.\n"; return; }
    map.unlockBridgeToSafeZone();
}

void resolveZombieEncounter(Player& player, Inventory& inv, ZombieSystem& zsys, bool& playerAlive);
void useGunOnZombies(Player& player, Inventory& inv, ZombieSystem& zsys, bool& playerAlive) {
    if (!inv.contains("Gun")) { cout << ">> No Gun!\n"; return; }
    int hordes = zsys.countHordesAt(player.currentLocation);
    if (hordes == 0) { cout << ">> No zombies here.\n"; return; }
    int ammo = inv.countItem("Ammo");
    if (ammo == 0) { cout << ">> No Ammo!\n"; return; }
    cout << ">> Targets: " << hordes << " | Ammo: " << ammo << ".\n";
    if (ammo >= hordes) {
        inv.consumeMany("Ammo", hordes);
        zsys.removeAllHordesAt(player.currentLocation);
        cout << ">> [Gun] All zombies dead.\n";
    } else {
        inv.consumeOne("Ammo");
        zsys.removeOneHordeAt(player.currentLocation);
        cout << ">> [Gun] Killed one, out of ammo. You die.\n";
        playerAlive = false;
    }
}

void resolveZombieEncounter(Player& player, Inventory& inv, ZombieSystem& zsys, bool& playerAlive) {
    if (!playerAlive) return;
    int n = zsys.countHordesAt(player.currentLocation);
    if (n == 0) return;
    cout << "\n!! ENCOUNTER !! " << n << " zombie horde(s)!\n";
    if (inv.contains("Gun") && inv.countItem("Ammo") > 0) {
        cout << ">> [Reflex] You fire...\n";
        useGunOnZombies(player, inv, zsys, playerAlive);
    } else {
        cout << ">> [Combat] You have no means to fight. Dead.\n";
        playerAlive = false;
    }
}

// =====================================================
//              INVENTORY MENU IMPL
// =====================================================

void Inventory::openMenu(Player& player, MapGraph& map, ZombieSystem& zsys, bool& playerAlive) {
    if (!head) { cout << "\n>> [Inventory] Empty.\n"; return; }
    resetCurrent();
    char choice;
    do {
        clearScreen();
        printHeader("INVENTORY");
        showCurrent();
        printSeparator();
        cout << "[N] Next | [B] Back | [U] Use | [D] Discard | [E] Exit\nSelect: ";
        cin >> choice; choice = tolower(choice);
        switch (choice) {
        case 'n': moveNext(); break;
        case 'b': moveBack(); break;
        case 'd': deleteCurrent(); if (!current && isEmpty()) choice = 'e'; break;
        case 'u':
            if (!current) { cout << ">> Select first.\n"; break; }
            {
                string n = current->name;
                if (n == "Energy Drink") useEnergyDrink(player, *this, playerAlive);
                else if (n == "Pills") usePills(player, *this, playerAlive);
                else if (n == "Bread") useFood(player, *this, "Bread", 15);
                else if (n == "Apple") useFood(player, *this, "Apple", 5);
                else if (n == "Cloth") useCloth(player, *this);
                else if (n == "Junk") useJunkAtCurrentNode(player, *this, zsys);
                else if (n == "Pebble") usePebble(player, *this, map, zsys);
                else if (n == "Twig") useTwig(player, *this, map, zsys);
                else if (n == "Axe") useAxeOnBridge(map, player, *this);
                else if (n == "Gun") cout << ">> Equip automatically during combat. Keep Ammo handy!\n";
                else if (n == "Ammo") cout << ">> Used automatically with Gun during combat.\n";
                else if (n == "Car Keys" || n == "Petrol") cout << ">> Use at Home to escape via Car.\n";
                else if (n == "User ID" || n == "PIN") cout << ">> Use at Safe Zone to win.\n";
                else cout << ">> Cannot use here.\n";
            }
            if (!playerAlive) choice = 'e'; else pause();
            break;
        case 'e': break;
        }
    } while (choice != 'e' && playerAlive);
}

// =====================================================
//                 PLAYER ACTIONS
// =====================================================

void playerMove(MapGraph& map, Player& player, MoveLog& log, ZombieSystem& zsys, int& zombieMinuteBuffer, Inventory& inv, bool& playerAlive) {
    cout << "\n";
    printHeader("TRAVEL");
    cout << "You are at: " << locationToString(player.currentLocation) << "\n";
    cout << "Destinations:\n";
    Node* n = map.getNeighbors(player.currentLocation);
    vector<Location> options;
    int idx = 1;
    while (n != NULL) {
        cout << " " << idx << ". " << locationToString((Location)n->vertex) << "\n";
        options.push_back((Location)n->vertex); idx++; n = n->next;
    }
    cout << " 0. Cancel\nChoice: ";
    int choice; cin >> choice;
    if (choice == 0) return;
    if (choice < 1 || choice >(int)options.size()) return;

    int cost = 60;
    if (inv.isFull()) cost += 30;
    if (player.isPoisoned) cost += 30;
    
    // Adrenaline Event (15% chance if HP < 60)
    if (player.hp < 60 && player.adrenalineMovesLeft == 0 && (rand() % 100 < 15)) {
        cout << ">> [Adrenaline] Rush! Faster moves.\n"; player.adrenalineMovesLeft = 2;
    }

    if (player.adrenalineMovesLeft > 0) { cost /= 2; if (cost < 15) cost = 15; player.adrenalineMovesLeft--; }
    if (player.speedBoostTimer > 0) {
        cost /= 2; player.speedBoostTimer -= 30; if (player.speedBoostTimer < 0) player.speedBoostTimer = 0;
    }

    if (map.movePlayer(player, options[choice - 1], log, cost)) {
        resolveZombieEncounter(player, inv, zsys, playerAlive);
        if (!playerAlive) return;
        if (rand() % 100 < 5) zsys.moveHordesToward(player.currentLocation);
        advanceZombies(zsys, zombieMinuteBuffer, cost);
        applyTimeToPlayer(player, cost, playerAlive);
        if (playerAlive && zsys.isHordeAt(player.currentLocation)) resolveZombieEncounter(player, inv, zsys, playerAlive);
    }
}

void playerScavenge(MapGraph& map, Player& player, Inventory& inv, ZombieSystem& zsys, int& zBuf, bool& alive) {
    cout << "\n>> [Scavenge] Searching...\n";
    
    // 15% Chance to get Scratched
    if (!player.isScratched) {
        if (rand() % 100 < 15) {
            player.isScratched = true;
            cout << ">> [OUCH!] Cut your hand! You are SCRATCHED (-5 HP/hr).\n";
        }
    }

    int cost = 30;
    if (player.speedBoostTimer > 0) {
        cost /= 2; player.speedBoostTimer -= 15; if (player.speedBoostTimer < 0) player.speedBoostTimer = 0;
    }

    player.timeMinutes += cost;
    advanceZombies(zsys, zBuf, cost);
    applyTimeToPlayer(player, cost, alive);
    if (!alive) return;
    if (zsys.isHordeAt(player.currentLocation)) resolveZombieEncounter(player, inv, zsys, alive);
    if (!alive) return;

    ItemProb* found = map.scavenge(player.currentLocation);
    if (!found) { cout << ">> Found nothing.\n"; return; }
    
    cout << ">> Found: " << found->name << "!\n";
    if (found->name == "Gun") cout << "   (It's loaded with 1 Ammo)\n";

    if (found->name == "Backpack") {
        cout << "   Pick up? (P/L): "; char c; cin >> c;
        if (tolower(c) == 'p') { inv.applyBackpack(); map.removeItemChance(player.currentLocation, found); }
        return;
    }

    cout << "   Pick (P) or Leave (L)? "; char c; cin >> c;
    if (tolower(c) == 'p') {
        if (!inv.isFull()) { 
            inv.addItem(found->name, "Item.", 1); 
            if (found->name == "Gun") inv.addItem("Ammo", "Round.", 1);
            map.removeItemChance(player.currentLocation, found); 
        } else {
            cout << ">> Inventory Full. Swap (S)? "; char c2; cin >> c2;
            if (tolower(c2) == 's') {
                inv.listItemsWithIndex(); int idx; cout << "Del Index: "; cin >> idx;
                inv.deleteByIndex(idx);
                if (!inv.isFull()) { 
                    inv.addItem(found->name, "Item.", 1); 
                    if (found->name == "Gun") inv.addItem("Ammo", "Round.", 1);
                    map.removeItemChance(player.currentLocation, found); 
                }
            }
        }
    }
}

void playerRest(Player& p, Inventory& inv, ZombieSystem& zsys, int& zBuf, bool& alive) {
    cout << "\n>> [Rest] Taking a break...\n";
    p.timeMinutes += 60;
    advanceZombies(zsys, zBuf, 60);
    applyTimeToPlayer(p, 60, alive);
    if (alive && zsys.isHordeAt(p.currentLocation)) resolveZombieEncounter(p, inv, zsys, alive);
    if (alive) { p.stamina = min(100, p.stamina + 30); cout << ">> Stamina recovered.\n"; }
}

void undoLastMove(Player& p, MoveLog& log) {
    Location pl; int pt;
    if (log.pop(pl, pt)) { p.currentLocation = pl; p.timeMinutes = pt; cout << ">> [Undo] Time rewind.\n"; }
    else cout << ">> Nothing to undo.\n";
}

void tryCarEscape(Player& p, Inventory& inv, bool& won) {
    if (p.currentLocation != HOME) { cout << ">> Go Home first.\n"; return; }
    if (!inv.contains("Car Keys")) { cout << ">> Need Car Keys.\n"; return; }
    if (inv.countItem("Petrol") < 2) { cout << ">> Need 2 Petrol units.\n"; return; }
    cout << ">> [Car] Turning ignition...\n"; inv.consumeMany("Petrol", 2);
    if (rand() % 100 < 70) { cout << "\n*** CAR ESCAPE VICTORY! ***\n"; won = true; }
    else cout << ">> [Car] Engine dead.\n";
}

void checkWin(Player& p, Inventory& inv, bool& won) {
    if (!won && p.currentLocation == SAFE_ZONE && inv.contains("PIN") && inv.contains("User ID")) {
        cout << "\n*** SAFE ZONE VICTORY! ***\n"; won = true;
    }
}

void printHUD(Player& p, Inventory& inv) {
    cout << "\n+-------------------------------------------------------------+\n";
    cout << "| LOC: " << setw(15) << left << locationToString(p.currentLocation) 
         << " | TIME: " << setw(4) << p.timeMinutes << "m "
         << "| HP: " << setw(3) << p.hp << "% "
         << "| STM: " << setw(3) << p.stamina << "% |\n";
    cout << "| STATUS: ";
    if (p.isScratched) cout << "SCRATCHED "; else if (p.isPoisoned) cout << "POISONED "; else cout << "OK        ";
    cout << "     | INV: " << inv.getUsedSlots() << "/" << inv.getCapacity() << " slots       |\n";
    cout << "+-------------------------------------------------------------+\n";
}

int main() {
    srand(time(0));
    MapGraph map; Inventory inv(8); MoveLog log; ZombieSystem zsys(&map);
    int zBuf = 0; zsys.addInitialHorde(LAB);
    Player p = { TOWN_HALL, 0, 100, 0, 100, false, false, 0, 0, 0 };
    bool alive = true; bool won = false;

    drawGameTitle();

    char c;
    do {
        clearScreen();
        printHUD(p, inv);
        cout << " [1] Move         [2] Scavenge    [3] Rest\n";
        cout << " [M] Map          [G] Attack      [I] Inventory\n";
        cout << " [K] Car Escape   [U] Undo        [Q] Quit\n";
        cout << "\n >> Command: ";
        cin >> c;

        switch (c) {
        case '1': playerMove(map, p, log, zsys, zBuf, inv, alive); break;
        case '2': playerScavenge(map, p, inv, zsys, zBuf, alive); break;
        case '3': playerRest(p, inv, zsys, zBuf, alive); break;
        case 'm': case 'M': map.displayPrettyMap(p, zsys); break; 
        case 'g': case 'G': useGunOnZombies(p, inv, zsys, alive); break;
        case 'i': case 'I': inv.openMenu(p, map, zsys, alive); break;
        case 'k': case 'K': tryCarEscape(p, inv, won); break;
        case 'u': case 'U': undoLastMove(p, log); break;
        case 'q': case 'Q': break;
        default: cout << ">> Invalid Command.\n";
        }
        
        if (c != 'i' && c != 'm' && c != 'M' && c != 'q' && c != 'Q') pause();

        checkWin(p, inv, won);
        if (!alive || won) break;
    } while (c != 'q' && c != 'Q');

    if (!alive) cout << "\n>> GAME OVER. <<\n";
    return 0;
}
