#include "router.h" // Include the router header file
#include "function.h"

// Main function
int main()
{
    // Initialize the number of routers
    rnumber = 0;

    // Create routers
    Router router1 = Router();
    Router router2 = Router();
    Router router3 = Router();
    Router router4 = Router();
    Router router5 = Router();

    // Connect routers with link costs
    router1.add_neighbor(&router2);
    router1.add_neighbor(&router3);
    router2.add_neighbor(&router1);
    router2.add_neighbor(&router3);
    router2.add_neighbor(&router4);
    router3.add_neighbor(&router1);
    router3.add_neighbor(&router2);
    router3.add_neighbor(&router4);
    router3.add_neighbor(&router5);
    router4.add_neighbor(&router2);
    router4.add_neighbor(&router3);
    router4.add_neighbor(&router5);
    router5.add_neighbor(&router3);
    router5.add_neighbor(&router4);

    // Update routing tables
    router1.update_routing_table();
    router2.update_routing_table();
    router3.update_routing_table();
    router4.update_routing_table();
    router5.update_routing_table();

    // Print routing tables
    router1.print_routing_table();
    router2.print_routing_table();
    router3.print_routing_table();
    router4.print_routing_table();
    router5.print_routing_table();

    // Simulate packet transmission
    printf("***************************************\n");
    int source_router_id = router1.get_id();
    int destination_router_id = router5.get_id();

    // Print simulation header
    cout << "\e[1mSimulating packet routing from Router " << source_router_id
         << " to Router " << destination_router_id << ":\e[0m" << endl;

    // Initialize current router
    Router *current_router = &router1;

    // Simulate packet forwarding
    while (current_router->get_id() != destination_router_id)
    {
        int next_hop_id = current_router->get_next_id(destination_router_id);
        current_router->print_routing_table(destination_router_id);
        cout << "\e[1m-Packet forwarded from Router " << current_router->get_id()
             << " to Router " << next_hop_id << "\e[0m" << endl;
        current_router = current_router->get_next_router(destination_router_id);
    }

    // Print packet reached destination message
    cout << "\n\e[1mPacket reached destination Router " << destination_router_id << "\e[0m" << endl;

    return 0; // Return 0 to indicate successful execution
}

