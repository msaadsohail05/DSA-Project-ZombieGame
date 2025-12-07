#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

// ---------- LOCATIONS ----------
enum Location {
    TOWN_HALL, HOME, OFFICE, LAB, PETROL_STATION, PARK, BUS_STOP, STORE, SCHOOL, CAFE, POLICE_STATION,
    HOSPITAL,
    BRIDGE,
    SAFE_ZONE,
    COUNT // must stay last
};

// ---------- LINKED LIST NODE FOR ADJACENCY ----------
struct Node {
    int vertex;
    Node* next;

    Node(int v) {
        vertex = v;
        next = NULL;
    }
};

// ---------- HELPER: LOCATION TO STRING ----------
string locationToString(Location loc) {
    switch (loc) {
    case TOWN_HALL: return "Town Hall";
    case HOME: return "Home";
    case OFFICE: return "Office";
    case LAB: return "Lab";
    case PETROL_STATION: return "Petrol Station";
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

// ---------- ITEM PROBABILITY STRUCT ----------
struct ItemProb {
    string name;
    double probability; // percentage
};

// ---------- PLAYER ----------
struct Player {
    Location currentLocation;
    int timeMinutes;     // total time passed
    int stamina;         // 0–100
};

// =====================================================
//              INVENTORY (DOUBLY LINKED LIST)
// =====================================================
struct InvNode {
    string name;
    string description;
    int quantity;
    InvNode* prev;
    InvNode* next;

    InvNode(const string& n, const string& d, int q)
        : name(n), description(d), quantity(q), prev(NULL), next(NULL) {
    }
};

class Inventory {
private:
    InvNode* head;
    InvNode* tail;
    InvNode* current;    // for scrolling
    int capacity;        // max slots
    int usedSlots;       // how many item types (nodes)
    bool hasBackpack;    // track if backpack already applied

public:
    Inventory(int cap = 8) {
        head = tail = current = NULL;
        capacity = cap;
        usedSlots = 0;
        hasBackpack = false;
    }

    bool isEmpty() const {
        return head == NULL;
    }

    bool isFull() const {
        return usedSlots >= capacity;
    }

    int getCapacity() const { return capacity; }
    int getUsedSlots() const { return usedSlots; }

    // Called when Backpack is obtained
    void applyBackpack() {
        if (hasBackpack) {
            cout << "[Inventory] Backpack already applied. Capacity is already " << capacity << ".\n";
            return;
        }
        capacity = 12;
        hasBackpack = true;
        cout << "[Inventory] Backpack obtained! Capacity increased to 12 slots.\n";
    }

    // Check if an item already exists
    bool contains(const string& itemName) {
        InvNode* temp = head;
        while (temp) {
            if (temp->name == itemName) return true;
            temp = temp->next;
        }
        return false;
    }

    void addItem(const string& name, const string& desc, int quantity = 1) {
        // Try to merge with existing item
        InvNode* temp = head;
        while (temp != NULL) {
            if (temp->name == name) {
                temp->quantity += quantity;
                cout << "[Inventory] Stacked more of " << name
                    << ". New qty: " << temp->quantity << "\n";
                return;
            }
            temp = temp->next;
        }

        // New item type
        if (isFull()) {
            cout << "[Inventory] Inventory is full, cannot add new item type: " << name << "\n";
            return;
        }

        InvNode* node = new InvNode(name, desc, quantity);
        if (!head) {
            head = tail = node;
        }
        else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        usedSlots++;
        cout << "[Inventory] Added new item: " << name << " (x" << quantity << ")\n";
    }

    void deleteNode(InvNode* node) {
        if (!node) return;

        cout << "[Inventory] Deleting item: " << node->name << "\n";

        if (node->prev)
            node->prev->next = node->next;
        else
            head = node->next;

        if (node->next)
            node->next->prev = node->prev;
        else
            tail = node->prev;

        if (current == node) {
            if (node->next) current = node->next;
            else current = node->prev;
        }

        delete node;
        usedSlots--;
    }

    void deleteCurrent() {
        if (!current) {
            cout << "[Inventory] Nothing selected to delete.\n";
            return;
        }
        deleteNode(current);
    }

    void showCurrent() {
        if (!current) {
            cout << "[Inventory] (No item selected)\n";
            return;
        }
        cout << "\n--- CURRENT ITEM ---\n";
        cout << "Name: " << current->name << "\n";
        cout << "Quantity: " << current->quantity << "\n";
        cout << "Description: " << current->description << "\n";
        cout << "--------------------\n";
    }

    void moveNext() {
        if (!current) {
            current = head;
        }
        else if (current->next) {
            current = current->next;
        }
        else {
            cout << "[Inventory] Already at last item.\n";
        }
    }

    void moveBack() {
        if (!current) {
            current = head;
        }
        else if (current->prev) {
            current = current->prev;
        }
        else {
            cout << "[Inventory] Already at first item.\n";
        }
    }

    // Show all items with indexes (for swap selection)
    void listItemsWithIndex() {
        cout << "\n[Inventory] Items:\n";
        InvNode* temp = head;
        int idx = 1;
        while (temp) {
            cout << idx << ". " << temp->name << " (x" << temp->quantity << ")\n";
            temp = temp->next;
            idx++;
        }
        if (idx == 1) {
            cout << "(empty)\n";
        }
        cout << "\n";
    }

    // Delete by 1-based index (for swap)
    void deleteByIndex(int index) {
        InvNode* temp = head;
        int idx = 1;
        while (temp && idx < index) {
            temp = temp->next;
            idx++;
        }
        if (!temp) {
            cout << "[Inventory] Invalid index.\n";
            return;
        }
        deleteNode(temp);
    }

    // Open inventory UI, DOES NOT change time
    void openMenu() {
        if (!head) {
            cout << "\n[Inventory] Your inventory is empty.\n";
            return;
        }

        if (!current) current = head;

        char choice;
        do {
            showCurrent();
            cout << "[N] Next  | [B] Back  | [D] Delete  | [E] Exit\n";
            cout << "Enter choice: ";
            cin >> choice;
            choice = tolower(choice);

            switch (choice) {
            case 'n':
                moveNext();
                break;
            case 'b':
                moveBack();
                break;
            case 'd':
                deleteCurrent();
                if (!current && isEmpty()) {
                    cout << "[Inventory] Inventory is now empty.\n";
                    choice = 'e'; // auto exit
                }
                break;
            case 'e':
                cout << "[Inventory] Closing inventory.\n";
                break;
            default:
                cout << "Invalid choice.\n";
            }
        } while (choice != 'e');
    }

    // Consume one unit of an item (e.g. "Axe", "Junk", "Ammo").
    // Returns true if one was consumed, false if item not found.
    bool consumeOne(const string& itemName) {
        InvNode* temp = head;
        while (temp) {
            if (temp->name == itemName) {
                temp->quantity--;
                if (temp->quantity <= 0) {
                    deleteNode(temp);  // already defined
                }
                return true;
            }
            temp = temp->next;
        }
        return false; // item not present
    }

    // Count total quantity of a specific item in inventory
    int countItem(const string& itemName) const {
        int total = 0;
        InvNode* temp = head;
        while (temp) {
            if (temp->name == itemName) {
                total += temp->quantity;
            }
            temp = temp->next;
        }
        return total;
    }

    // Consume N units of an item; returns true if fully consumed, false if not enough
    bool consumeMany(const string& itemName, int count) {
        int remaining = count;
        InvNode* temp = head;
        while (temp && remaining > 0) {
            if (temp->name == itemName) {
                if (temp->quantity > remaining) {
                    temp->quantity -= remaining;
                    remaining = 0;
                } else {
                    remaining -= temp->quantity;
                    InvNode* toDelete = temp;
                    temp = temp->next;
                    deleteNode(toDelete);
                    continue; // skip temp = temp->next below
                }
            }
            if (temp) temp = temp->next;
        }
        return remaining == 0;
    }
};

// =====================================================
//              UNDO MOVE LOG (DOUBLY LINKED LIST)
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
    MoveNodeDLL* tail; // we treat tail as top of stack

public:
    MoveLog() {
        head = tail = NULL;
    }

    bool isEmpty() const {
        return tail == NULL;
    }

    // Store state BEFORE move
    void push(Location locBefore, int timeBefore) {
        MoveNodeDLL* node = new MoveNodeDLL(locBefore, timeBefore);
        if (!head) {
            head = tail = node;
        }
        else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        // cout << "[MoveLog] Pushed state: " << locationToString(locBefore)
        //      << ", time " << timeBefore << "\n";
    }

    // Pop last state, returns true if successful and writes into parameters
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
//                      MAP (GRAPH)
// =====================================================
class MapGraph {
private:
    Node* adj[COUNT];                  // adjacency list
    vector<ItemProb> itemTable[COUNT]; // item probabilities for each node
    bool bridgeUnlocked;          //  to check if the bridge to safe zone is unlocked

public:
    MapGraph() {
        for (int i = 0; i < COUNT; i++)
            adj[i] = NULL;

        bridgeUnlocked = false;

        buildDefaultMap();
        initItemProbabilities();
    }

    void addEdge(Location a, Location b) {
        int u = a, v = b;

        Node* newNode = new Node(v);
        newNode->next = adj[u];
        adj[u] = newNode;

        Node* newNode2 = new Node(u);
        newNode2->next = adj[v];
        adj[v] = newNode2;
    }

    void printMap() {
        cout << "=== GAME MAP ===\n\n";
        for (int i = 0; i < COUNT; i++) {
            cout << locationToString((Location)i) << " -> ";
            Node* temp = adj[i];
            while (temp != NULL) {
                cout << locationToString((Location)temp->vertex);
                if (temp->next != NULL) cout << ", ";
                temp = temp->next;
            }
            cout << "\n";
        }
        cout << "\n";
    }

    Node* getNeighbors(Location loc) {
        return adj[loc];
    }

    bool isConnected(Location from, Location to) {
        Node* temp = adj[from];
        while (temp != NULL) {
            if (temp->vertex == to) return true;
            temp = temp->next;
        }
        return false;
    }

    // ---------- PLAYER MOVE (1 hour = 60 min) ----------
    bool movePlayer(Player& player, Location dest, MoveLog& log) {
        if (!isConnected(player.currentLocation, dest)) {
            cout << "You cannot move from "
                << locationToString(player.currentLocation)
                << " to " << locationToString(dest)
                << " (not directly connected).\n";
            return false;
        }

        // Save current state BEFORE move for undo
        log.push(player.currentLocation, player.timeMinutes);

        player.currentLocation = dest;
        player.timeMinutes += 60;

        cout << "\n[Move] You moved to " << locationToString(dest)
            << ". +60 minutes.\n";
        cout << "Total time: " << player.timeMinutes
            << " minutes | Stamina: " << player.stamina << "\n\n";

        return true;
    }

    // ---------- SCAVENGE: returns pointer to found item or nullptr (nothing) ----------
    ItemProb* scavenge(Location loc) {
        double sum = 0;
        for (auto& ip : itemTable[loc]) sum += ip.probability;

        int roll = rand() % 100; // 0–99
        double cumulative = 0;
        for (auto& ip : itemTable[loc]) {
            cumulative += ip.probability;
            if (roll < cumulative) {
                return &ip; // found this item
            }
        }

        // Fell into "nothing" region (100 - sum)
        return nullptr;
    }

    // After picking an item: reduce its chance to 0% → that probability becomes "nothing"
    void removeItemChance(Location loc, ItemProb* item) {
        if (!item) return;
        cout << "[Scavenge] Removing further chance of finding: " << item->name << " at "
            << locationToString(loc) << "\n";
        item->probability = 0;
    }

    void printLocationItems(Location loc) {
        cout << "Scavenge table for " << locationToString(loc) << ":\n";
        double sum = 0;
        for (auto& ip : itemTable[loc]) {
            cout << " - " << ip.name << " : " << ip.probability << "%\n";
            sum += ip.probability;
        }
        cout << "Total = " << sum << "% | Nothing = " << (100.0 - sum) << "%\n\n";
    }

    void unlockBridgeToSafeZone() {
        if (bridgeUnlocked) {
            cout << "[Map] The path from Bridge to Safe Zone is already open.\n";
            return;
        }
        addEdge(BRIDGE, SAFE_ZONE);
        bridgeUnlocked = true;
        cout << "[Map] You chopped down the barricade at the Bridge.\n";
        cout << "      The path to the Safe Zone is now open!\n";
    }

    bool isBridgeOpen() const {
        return bridgeUnlocked;
    }

private:
    void buildDefaultMap() {
        addEdge(TOWN_HALL, HOME);
        addEdge(TOWN_HALL, PARK);
        addEdge(TOWN_HALL, OFFICE);
        addEdge(TOWN_HALL, CAFE);

        addEdge(HOME, PARK);
        addEdge(HOME, PETROL_STATION);

        addEdge(OFFICE, STORE);
        addEdge(OFFICE, POLICE_STATION);

        addEdge(LAB, HOSPITAL);
        addEdge(LAB, BRIDGE);

        addEdge(PARK, HOSPITAL);
        addEdge(CAFE, STORE);

        addEdge(BUS_STOP, PARK);
        addEdge(BUS_STOP, SCHOOL);
        addEdge(BUS_STOP, TOWN_HALL);

        addEdge(SCHOOL, STORE);
        addEdge(SCHOOL, CAFE);

        addEdge(STORE, PETROL_STATION);

        addEdge(POLICE_STATION, HOSPITAL);

        // BRIDGE -> SAFE_ZONE is locked until Axe is used
        // addEdge(BRIDGE, SAFE_ZONE);
    }

    void addItem(Location loc, const string& name, double prob) {
        itemTable[loc].push_back({ name, prob });
    }

    void initItemProbabilities() {
        // ---------- HOME ----------
        addItem(HOME, "Bread", 19);
        addItem(HOME, "Pills", 10);
        addItem(HOME, "Apple", 10);   // represents the apple group
        addItem(HOME, "User ID", 10);
        addItem(HOME, "Car Keys", 1);
        // Sum = 50% → Nothing = 50%

        // ---------- PETROL STATION ----------
        addItem(PETROL_STATION, "Petrol", 12);
        addItem(PETROL_STATION, "Cloth", 20);

        // ---------- PARK ----------
        addItem(PARK, "Apple", 10);
        addItem(PARK, "Energy Drink", 5);
        addItem(PARK, "Coin", 15);
        addItem(PARK, "Twig", 10);

        // ---------- BUS STOP ----------
        addItem(BUS_STOP, "Coin", 10);
        addItem(BUS_STOP, "Pebble", 15);

        // ---------- OFFICE ----------
        addItem(OFFICE, "Pills", 10);
        addItem(OFFICE, "User ID", 35);
        addItem(OFFICE, "Junk", 15);

        // ---------- STORE ----------
        addItem(STORE, "Bread", 10);
        addItem(STORE, "Apple", 10);
        addItem(STORE, "Energy Drink", 10);
        addItem(STORE, "Axe", 35);
        addItem(STORE, "Junk", 5);

        // ---------- SCHOOL ----------
        addItem(SCHOOL, "Apple", 5);
        addItem(SCHOOL, "Backpack", 30);
        addItem(SCHOOL, "Junk", 5);
        addItem(SCHOOL, "Book", 15);

        // ---------- TOWN HALL ----------
        addItem(TOWN_HALL, "Coin", 5);
        addItem(TOWN_HALL, "Bread", 10);
        addItem(TOWN_HALL, "Cloth", 10);
        addItem(TOWN_HALL, "Pebble", 15);

        // ---------- CAFE ----------
        addItem(CAFE, "Energy Drink", 20);
        addItem(CAFE, "Bread", 15);
        addItem(CAFE, "Coin", 5);

        // ---------- POLICE STATION ----------
        addItem(POLICE_STATION, "Gun", 50);
        addItem(POLICE_STATION, "Ammo", 10);
        addItem(POLICE_STATION, "Energy Drink", 10);

        // ---------- HOSPITAL ----------
        addItem(HOSPITAL, "Apple", 15);
        addItem(HOSPITAL, "Cloth", 15);
        addItem(HOSPITAL, "First Aid", 20);

        // ---------- LAB ----------
        addItem(LAB, "PIN", 60);
        addItem(LAB, "Ammo", 5);
        addItem(LAB, "First Aid", 10);
        addItem(LAB, "Pebble", 10);

        // ---------- BRIDGE ----------
        addItem(BRIDGE, "Pebble", 10);
        addItem(BRIDGE, "Twig", 10);
        addItem(BRIDGE, "Ammo", 5);
        addItem(BRIDGE, "First Aid", 20);

        // SAFE_ZONE: nothing
    }
};


// =====================================================
//                  ZOMBIE SYSTEM (MINIMAL)
// =====================================================

struct ZombieHorde {
    int id;
    Location currentLocation;
    int infectionRate; // %
};

class ZombieSystem {
private:
    MapGraph* map;
    vector<ZombieHorde> hordes;
    int junkBlocks[COUNT];   // Junk protection duration per node (in zombie moves)
    int nextId;

public:
    ZombieSystem(MapGraph* m) {
        map = m;
        nextId = 1;
        for (int i = 0; i < COUNT; ++i) junkBlocks[i] = 0;
    }

    void addInitialHorde(Location loc) {
        ZombieHorde h;
        h.id = nextId++;
        h.currentLocation = loc;
        h.infectionRate = 10; // start 10%
        hordes.push_back(h);
        cout << "[Zombie] Created horde " << h.id
            << " at " << locationToString(loc)
            << " (infection 10%)\n";
    }

    // Apply Junk on a node: block it for next 2 zombie moves
    void applyJunk(Location loc) {
        junkBlocks[loc] = 2;
        cout << "[Junk] " << locationToString(loc)
            << " will repel zombies for the next 2 moves.\n";
    }

    // Call this whenever 1 game hour passes
    void simulateHour() {
        cout << "\n=== ZOMBIES MOVE (1 HOUR) ===\n";

        for (auto& h : hordes) {
            moveHordeOneStep(h);
        }

        // Decrease Junk timers
        for (int i = 0; i < COUNT; ++i) {
            if (junkBlocks[i] > 0) junkBlocks[i]--;
        }
    }

    // ----- COMBAT HELPERS -----
    int countHordesAt(Location loc) {
        int c = 0;
        for (auto &h : hordes) {
            if (h.currentLocation == loc) c++;
        }
        return c;
    }

    void removeAllHordesAt(Location loc) {
        for (int i = (int)hordes.size() - 1; i >= 0; --i) {
            if (hordes[i].currentLocation == loc) {
                hordes.erase(hordes.begin() + i);
            }
        }
    }

    void removeOneHordeAt(Location loc) {
        for (int i = 0; i < (int)hordes.size(); ++i) {
            if (hordes[i].currentLocation == loc) {
                cout << "[Combat] Horde " << hordes[i].id << " was killed.\n";
                hordes.erase(hordes.begin() + i);
                return;
            }
        }
    }

private:
    void moveHordeOneStep(ZombieHorde& zombie) {
        cout << "[Horde " << zombie.id << "] At "
            << locationToString(zombie.currentLocation)
            << " | Infection: " << zombie.infectionRate << "%\n";

        int roll = rand() % 100;

        // 15% chance to rest
        if (roll < 15) {
            cout << "  -> Resting. Infection unchanged.\n\n";
            return;
        }

        // Gather neighbors that are NOT Junk-blocked
        vector<Location> neighbors;
        Node* temp = map->getNeighbors(zombie.currentLocation);
        while (temp) {
            Location neigh = (Location)temp->vertex;
            if (junkBlocks[neigh] == 0) {
                neighbors.push_back(neigh);
            }
            temp = temp->next;
        }

        if (neighbors.empty()) {
            cout << "  -> All neighboring paths blocked by Junk. Horde stays.\n";
            cout << "     Infection paused (no +5).\n\n";
            return;
        }

        int idx = rand() % neighbors.size();
        Location newLoc = neighbors[idx];

        zombie.currentLocation = newLoc;
        zombie.infectionRate += 5;

        cout << "  -> Moved to " << locationToString(newLoc)
            << ". Infection now " << zombie.infectionRate << "%\n\n";
    }
};


// Keep track of how many minutes have passed for zombie movement
void advanceZombies(ZombieSystem& zsys, int& zombieMinuteBuffer, int deltaMinutes) {
    zombieMinuteBuffer += deltaMinutes;
    while (zombieMinuteBuffer >= 60) {
        zsys.simulateHour();
        zombieMinuteBuffer -= 60;
    }
}


void useJunkAtCurrentNode(Player& player, Inventory& inv, ZombieSystem& zsys) {
    if (inv.consumeOne("Junk")) {
        cout << "[Item] You scatter Junk at "
            << locationToString(player.currentLocation) << ".\n";
        zsys.applyJunk(player.currentLocation);
    }
    else {
        cout << "[Item] You don't have any Junk.\n";
    }
}

// =====================================================
//                 PLAYER ACTIONS
// =====================================================

// Move: costs 1 hour (handled by movePlayer)
void playerMove(MapGraph& map, Player& player, MoveLog& log,
    ZombieSystem& zsys, int& zombieMinuteBuffer) {
    cout << "\nYou are at: " << locationToString(player.currentLocation) << "\n";
    cout << "You can move to:\n";

    Node* n = map.getNeighbors(player.currentLocation);
    vector<Location> options;
    int idx = 1;
    while (n != NULL) {
        Location loc = (Location)n->vertex;
        cout << idx << ". " << locationToString(loc) << "\n";
        options.push_back(loc);
        idx++;
        n = n->next;
    }

    if (options.empty()) {
        cout << "No neighboring locations to move to.\n";
        return;
    }

    int choice;
    cout << "Enter choice number: ";
    cin >> choice;

    if (choice < 1 || choice >(int)options.size()) {
        cout << "Invalid move choice.\n";
        return;
    }

    // movePlayer already adds +60 minutes
    bool ok = map.movePlayer(player, options[choice - 1], log);
    if (ok) {
        advanceZombies(zsys, zombieMinuteBuffer, 60); // 1 hour for zombies
    }
}

// Scavenge: costs 30 minutes, random item / nothing,
// inventory rules: if full → Swap or Leave
void playerScavenge(MapGraph& map, Player& player, Inventory& inv,
                    ZombieSystem& zsys, int& zombieMinuteBuffer) {
    cout << "\n[Scavenge] You search the area at "
        << locationToString(player.currentLocation) << "...\n";

    player.timeMinutes += 30; // 30 minutes cost
    cout << "Time +30 minutes. Total time: " << player.timeMinutes
        << " minutes | Stamina: " << player.stamina << "\n";


    advanceZombies(zsys, zombieMinuteBuffer, 30);
    ItemProb* found = map.scavenge(player.currentLocation);
    if (!found) {
        cout << "You found nothing.\n\n"; // "If 'nothing', just skip"
        return;
    }

    cout << "You found: " << found->name << "\n";

    // Backpack effect
    if (found->name == "Backpack") {
        char c;
        cout << "Pick Backpack? (P to pick, L to leave): ";
        cin >> c;
        c = tolower(c);

        if (c == 'p') {
            inv.applyBackpack();
            map.removeItemChance(player.currentLocation, found);
        }
        else {
            cout << "You left the Backpack.\n";
        }
        cout << "\n";
        return;
    }

    char choice;
    cout << "Pick (P) or Leave (L)? ";
    cin >> choice;
    choice = tolower(choice);

    if (choice == 'p') {
        if (!inv.isFull()) {
            inv.addItem(found->name, "An item you scavenged.", 1);
            map.removeItemChance(player.currentLocation, found);
        }
        else {
            cout << "[Inventory] Inventory is FULL ("
                << inv.getUsedSlots() << "/" << inv.getCapacity() << " slots).\n";
            cout << "Do you want to Swap (S) an existing item or Leave (L) this one?\n";
            char c2;
            cout << "Enter S or L: ";
            cin >> c2;
            c2 = tolower(c2);

            if (c2 == 's') {
                // Swap: show inventory, choose index to delete, then add new item
                inv.listItemsWithIndex();
                int idx;
                cout << "Enter index of item to discard: ";
                cin >> idx;
                inv.deleteByIndex(idx);

                if (!inv.isFull()) {
                    inv.addItem(found->name, "An item you scavenged (swapped).", 1);
                    map.removeItemChance(player.currentLocation, found);
                }
                else {
                    cout << "[Inventory] Still full, could not add item.\n";
                }
            }
            else {
                cout << "You left the item.\n";
            }
        }
    }
    else {
        cout << "You left the item.\n";
    }

    cout << "\n";
}

// Rest: restores stamina, costs 1 hour
void playerRest(Player& player, ZombieSystem &zsys, int &zombieMinuteBuffer) {
    cout << "\n[Rest] You take some time to rest...\n";
    player.timeMinutes += 60;
    advanceZombies(zsys, zombieMinuteBuffer, 60);
    // simple rule: +30 stamina up to 100
    player.stamina += 30;
    if (player.stamina > 100) player.stamina = 100;

    cout << "Time +60 minutes. Total time: " << player.timeMinutes << " minutes.\n";
    cout << "Stamina restored. Current stamina: " << player.stamina << "\n\n";
}

void useAxeOnBridge(MapGraph& map, Player& player, Inventory& inv) {
    // Must be at Bridge to use Axe
    if (player.currentLocation != BRIDGE) {
        cout << "[Axe] You need to be at the Bridge to chop the barricade.\n";
        return;
    }

    // Check if already unlocked
    if (map.isBridgeOpen()) {
        cout << "[Axe] The path to the Safe Zone is already open.\n";
        return;
    }

    // Try to consume one Axe from inventory
    if (!inv.consumeOne("Axe")) {
        cout << "[Axe] You don't have an Axe in your inventory.\n";
        return;
    }

    // Unlock the edge in the graph
    map.unlockBridgeToSafeZone();
}

// Gun + Ammo combat vs zombie hordes
// Rule:
//  - Must have Gun and Ammo
//  - Let H = number of hordes at current node
//  - If Ammo >= H: kill all hordes, consume H ammo, survive
//  - If 0 < Ammo < H: kill one horde, consume 1 ammo, player dies
void useGunOnZombies(Player &player, Inventory &inv, ZombieSystem &zsys, bool &playerAlive) {
    // Must have gun
    if (!inv.contains("Gun")) {
        cout << "[Gun] You don't have a gun.\n";
        return;
    }

    int hordesHere = zsys.countHordesAt(player.currentLocation);
    if (hordesHere == 0) {
        cout << "[Gun] There are no zombie hordes here to shoot.\n";
        return;
    }

    int ammo = inv.countItem("Ammo");
    if (ammo == 0) {
        cout << "[Gun] You have no ammo.\n";
        return;
    }

    cout << "[Gun] There are " << hordesHere << " horde(s) here. You have " << ammo << " ammo.\n";

    if (ammo >= hordesHere) {
        // Enough ammo to clear all hordes here
        bool ok = inv.consumeMany("Ammo", hordesHere);
        if (!ok) {
            // should not happen, but safety
            cout << "[Gun] Error consuming ammo.\n";
        }
        zsys.removeAllHordesAt(player.currentLocation);
        cout << "[Gun] You unload your gun and wipe out all hordes at this location. You survive.\n";
    } else {
        // Not enough ammo: kill one, die
        inv.consumeOne("Ammo"); // consume 1 bullet
        zsys.removeOneHordeAt(player.currentLocation);
        cout << "[Gun] You managed to kill one horde, but you ran out of ammo.\n";
        cout << "      The remaining zombies overwhelm you...\n";
        cout << "      You died.\n";
        playerAlive = false;
    }
}

// Undo last move using MoveLog DLL
void undoLastMove(Player& player, MoveLog& log) {
    Location prevLoc;
    int prevTime;
    if (!log.pop(prevLoc, prevTime)) {
        cout << "[Undo] No moves to undo.\n";
        return;
    }

    player.currentLocation = prevLoc;
    player.timeMinutes = prevTime;

    cout << "[Undo] Reverted to previous location: "
        << locationToString(prevLoc)
        << " | Time: " << prevTime << " minutes.\n\n";
}

// =====================================================
//                  MAIN GAME LOOP DEMO
// =====================================================
int main() {
    srand(time(0));

    MapGraph map;
    Inventory inventory(8); // default 8 slots
    MoveLog moveLog;

    ZombieSystem zsys(&map);
    int zombieMinuteBuffer = 0;

    // Example: create 2 initial hordes
    zsys.addInitialHorde(PARK);
    zsys.addInitialHorde(OFFICE);

    Player player;
    player.currentLocation = TOWN_HALL;
    player.timeMinutes = 0;
    player.stamina = 100;

    bool playerAlive = true;

    cout << "=== SURVIVAL GAME DEMO: INVENTORY + UNDO + ZOMBIES + ITEMS ===\n";
    map.printMap();

    char choice;
    do {
        cout << "\n====================================\n";
        cout << "Location: " << locationToString(player.currentLocation) << "\n";
        cout << "Time: " << player.timeMinutes << " minutes\n";
        cout << "Stamina: " << player.stamina << "\n";
        cout << "Inventory: " << inventory.getUsedSlots()
            << "/" << inventory.getCapacity() << " slots used\n";
        cout << "====================================\n";
        cout << "Choose action:\n";
        cout << "1. Move       (costs 1 hour)\n";
        cout << "2. Scavenge   (costs 30 minutes)\n";
        cout << "3. Rest       (costs 1 hour, restores stamina)\n";
        cout << "a. Use an axe on Bridge barricade (no time cost)\n";
        cout << "j. Use junk on this node (no time cost)\n";
        cout << "g. Use gun on nearby zombies (no time cost)\n";
        cout << "i. Inventory  (no time cost)\n";
        cout << "u. Undo last move (no time cost)\n";
        cout << "q. Quit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case '1':
            playerMove(map, player, moveLog, zsys, zombieMinuteBuffer);
            break;
        case '2':
            playerScavenge(map, player, inventory, zsys, zombieMinuteBuffer);
            break;
        case '3':
            playerRest(player, zsys, zombieMinuteBuffer);
            break;
        case 'a':
        case 'A':
            useAxeOnBridge(map, player, inventory);
            break;
        case 'j':
        case 'J':
            useJunkAtCurrentNode(player, inventory, zsys);
            break;
        case 'g':
        case 'G':
            useGunOnZombies(player, inventory, zsys, playerAlive);
            break;
        case 'i':
        case 'I':
            inventory.openMenu(); // no time cost
            break;
        case 'u':
        case 'U':
            undoLastMove(player, moveLog);
            break;
        case 'q':
        case 'Q':
            cout << "Exiting game loop.\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }

        if (!playerAlive) {
            cout << "\n=== GAME OVER: You died. ===\n";
            break;
        }

    } while (choice != 'q' && choice != 'Q');

    return 0;
}
