#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <limits.h>

class Router
{
public:
    Router();
    void add_neighbor(Router *neighbour, int cost = 1);
    void update_routing_table();
    void print_routing_table(int dst = INT_MIN);
    int get_id();
    int get_next_id(int destination_router_id);
    Router *get_next_router(int destination_router_id);

private:
    int router_id;
    std::vector<std::pair<Router *, int>> neighbors;
    std::map<int, Router *> routing_table;
};

void simulate(Router *source_router, Router *destination_router);

#endif // ROUTER_H

