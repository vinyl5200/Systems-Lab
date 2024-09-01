#include "router.h" // Include the router header file

using namespace std;

int rnumber; // Global variable to keep track of the number of routers

// Constructor to initialize the router ID
Router::Router() : router_id(rnumber + 1)
{
    rnumber = rnumber + 1; // Increment the router number
}

// Function to add a neighbor router with a link cost
void Router::add_neighbor(Router *neighbour, int cost)
{
    neighbors.push_back(make_pair(neighbour, cost)); // Add neighbor and cost to the neighbors list
}

// Function to update the routing table using Dijkstra's algorithm
void Router::update_routing_table()
{
    // Clear the existing routing table
    routing_table.clear();

    // Initialize distance, parent, and next hop maps
    map<int, int> distance;
    map<int, Router *> parent;
    map<int, Router *> next_hop;

    // Initialize distances to infinity and set the source distance to 0
    for (int neighbor_id = 1; neighbor_id <= rnumber; neighbor_id++)
    {
        distance[neighbor_id] = INT_MAX;
    }
    distance[this->router_id] = 0;
    parent[this->router_id] = this;

    // Initialize priority queue for Dijkstra's algorithm
    priority_queue<pair<int, Router *>, vector<pair<int, Router *>>, greater<pair<int, Router *>>> pq;
    pq.push(make_pair(0, this));

    // Dijkstra's algorithm
    while (!pq.empty())
    {
        Router *current_router = pq.top().second;
        pq.pop();

        for (auto neighbor : current_router->neighbors)
        {
            int neighbor_id = neighbor.first->router_id;
            int link_cost = neighbor.second;

            // Update distance if a shorter path is found
            if (distance[current_router->router_id] + link_cost < distance[neighbor_id])
            {
                distance[neighbor_id] = distance[current_router->router_id] + link_cost;
                parent[neighbor_id] = current_router;
                pq.push(make_pair(distance[neighbor_id], neighbor.first));
            }
        }
    }

    // Calculate next hop for each destination
    next_hop[this->router_id] = this;
    for (auto i : neighbors)
    {
        next_hop[i.first->router_id] = i.first;
    }
    for (int i = 1; i <= rnumber; i++)
    {
        Router *j = parent[i];
        if (j == this)
            continue;
        while (parent[j->router_id] != this)
        {
            j = parent[j->router_id];
        }
        next_hop[i] = j;
    }

    // Build routing table
    for (auto const &entry : next_hop)
    {
        int destination = entry.first;
        Router *next = entry.second;
        routing_table[destination] = next;
    }
}

// Function to print the routing table
void Router::print_routing_table(int dst)
{
    // Print routing table header
    if (dst == INT_MIN)
    {
        cout << endl
             << "\e[1m* Routing Table for Router " << router_id << ":\e[0m" << endl
             << endl;
    }
    else
    {
        cout << endl
             << "Routing Table for Router " << router_id << ":\n" << endl;
    }

    // Print routing table entries
    cout << " Destination   Next Hop " << endl;
    cout << "                       " << endl;

    for (auto const &entry : routing_table)
    {
        int destination = entry.first;
        Router *next_hop = entry.second;

        if (destination != dst)
        {
            cout << "  Router " <<destination << "     Router " << next_hop->router_id << endl;
        }
        else
        {
            cout << "  Router " << destination<< "     Router " <<next_hop->router_id <<"***" <<  endl;
        }
    }

    cout << endl;
}

// Function to get the router ID
int Router::get_id()
{
    return router_id;
}

// Function to get the next hop router ID for a given destination
int Router::get_next_id(int destination_router_id)
{
    return routing_table[destination_router_id]->get_id();
}

// Function to get the next hop router pointer for a given destination
Router *Router::get_next_router(int destination_router_id)
{
    return routing_table[destination_router_id];
}



