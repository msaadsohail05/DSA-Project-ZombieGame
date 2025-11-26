#include <iostream>
#include <string>
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

// ---------- MAP (GRAPH) CLASS ----------
class MapGraph {
private:
    Node* adj[COUNT]; // adjacency list

public:
    MapGraph() {
        // initialize adjacency list
        for (int i = 0; i < COUNT; i++)
            adj[i] = NULL;

        buildDefaultMap();
    }

    // Add undirected edge
    void addEdge(Location a, Location b) {
        int u = a;
        int v = b;

        // Add edge u -> v
        Node* newNode = new Node(v);
        newNode->next = adj[u];
        adj[u] = newNode;

        // Add edge v -> u
        Node* newNode2 = new Node(u);
        newNode2->next = adj[v];
        adj[v] = newNode2;
    }

    // Print full map
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

    // Get adjacency list head (you can use it for movement)
    Node* getNeighbors(Location loc) {
        return adj[loc];
    }

private:
    // Build the map layout (same as previous answer)
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
};

// ---------- DEMO MAIN ----------
int main() {
    MapGraph map;

    map.printMap();

    // Example: neighbors of Town Hall
    Node* n = map.getNeighbors(TOWN_HALL);
    cout << "You are at Town Hall\nNeighbors:\n";
    while (n != NULL) {
        cout << "- " << locationToString((Location)n->vertex) << "\n";
        n = n->next;
    }

    return 0;
}
