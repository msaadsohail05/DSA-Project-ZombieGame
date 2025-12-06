#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

// ---------- LOCATIONS ----------
enum Location {
    TOWN_HALL,
    HOME,
    OFFICE,
    LAB,
    PETROL_STATION,
    PARK,
    BUS_STOP,
    STORE,
    SCHOOL,
    CAFE,
    POLICE_STATION,
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
        : name(n), description(d), quantity(q), prev(NULL), next(NULL) {}
};

struct ItemProb {
    string name;
    double probability; // percentage (e.g. 19 = 19%)
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
        } else {
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
        } else if (current->next) {
            current = current->next;
        } else {
            cout << "[Inventory] Already at last item.\n";
        }
    }

    void moveBack() {
        if (!current) {
            current = head;
        } else if (current->prev) {
            current = current->prev;
        } else {
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
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        // cout << "[MoveLog] Pushed state: " << locationToString(locBefore)
        //      << ", time " << timeBefore << "\n";
    }

    // Pop last state, returns true if successful and writes into parameters
    bool pop(Location &locOut, int &timeOut) {
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

public:
    MapGraph() {
        for (int i = 0; i < COUNT; i++)
            adj[i] = NULL;

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
    bool movePlayer(Player &player, Location dest, MoveLog &log) {
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
        for (auto &ip : itemTable[loc]) sum += ip.probability;

        int roll = rand() % 100; // 0–99
        double cumulative = 0;
        for (auto &ip : itemTable[loc]) {
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
        for (auto &ip : itemTable[loc]) {
            cout << " - " << ip.name << " : " << ip.probability << "%\n";
            sum += ip.probability;
        }
        cout << "Total = " << sum << "% | Nothing = " << (100.0 - sum) << "%\n\n";
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

        addEdge(BRIDGE, SAFE_ZONE);
    }

    void addItem(Location loc, const string& name, double prob) {
        itemTable[loc].push_back({name, prob});
    }

    void initItemProbabilities() {
        // HOME
        addItem(HOME, "Bread", 19);
        addItem(HOME, "Pills", 10);
        addItem(HOME, "Apple", 10);
        addItem(HOME, "User ID", 10);
        addItem(HOME, "Car Keys", 1);

        // SCHOOL (for Backpack)
        addItem(SCHOOL, "Backpack", 30);

        // TOWN HALL
        addItem(TOWN_HALL, "Coin", 20);
        addItem(TOWN_HALL, "Bread", 10);

        // LAB
        addItem(LAB, "PIN", 60);

        // You can fill rest from your document...
    }
};

// =====================================================
//                 PLAYER ACTIONS
// =====================================================

// Move: costs 1 hour (handled by movePlayer)
void playerMove(MapGraph &map, Player &player, MoveLog &log) {
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

    if (choice < 1 || choice > (int)options.size()) {
        cout << "Invalid move choice.\n";
        return;
    }

    map.movePlayer(player, options[choice - 1], log);
}

// Scavenge: costs 30 minutes, random item / nothing,
// inventory rules: if full → Swap or Leave
void playerScavenge(MapGraph &map, Player &player, Inventory &inv) {
    cout << "\n[Scavenge] You search the area at "
         << locationToString(player.currentLocation) << "...\n";

    player.timeMinutes += 30; // 30 minutes cost
    cout << "Time +30 minutes. Total time: " << player.timeMinutes
         << " minutes | Stamina: " << player.stamina << "\n";

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
        } else {
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
        } else {
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
                } else {
                    cout << "[Inventory] Still full, could not add item.\n";
                }
            } else {
                cout << "You left the item.\n";
            }
        }
    } else {
        cout << "You left the item.\n";
    }

    cout << "\n";
}

// Rest: restores stamina, costs 1 hour
void playerRest(Player &player) {
    cout << "\n[Rest] You take some time to rest...\n";
    player.timeMinutes += 60;
    // simple rule: +30 stamina up to 100
    player.stamina += 30;
    if (player.stamina > 100) player.stamina = 100;

    cout << "Time +60 minutes. Total time: " << player.timeMinutes << " minutes.\n";
    cout << "Stamina restored. Current stamina: " << player.stamina << "\n\n";
}

// Undo last move using MoveLog DLL
void undoLastMove(Player &player, MoveLog &log) {
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

    Player player;
    player.currentLocation = TOWN_HALL;
    player.timeMinutes = 0;
    player.stamina = 100;

    cout << "=== SURVIVAL GAME DEMO: INVENTORY + UNDO MOVE LOG ===\n";
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
        cout << "i. Inventory  (no time cost)\n";
        cout << "u. Undo last move (no time cost)\n";
        cout << "q. Quit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case '1':
                playerMove(map, player, moveLog);
                break;
            case '2':
                playerScavenge(map, player, inventory);
                break;
            case '3':
                playerRest(player);
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

    } while (choice != 'q' && choice != 'Q');

    return 0;
}
