#ifndef WIRELESS_NODE_H
#define WIRELESS_NODE_H

#include <iostream>
#include <vector>

extern const int MAX_BACKOFF_TIME;
extern int successful_transmissions;
extern int total_collisions;

class WirelessNode
{
public:
  int node_id;
  bool is_transmitting;
  int backoff_time;
  int transmission_duration;
  int time_remaining;
  int backoff_attempts;

  WirelessNode();

  void update();
};


bool is_ready_to_transmit(WirelessNode &node);
void handle_collision(std::vector<WirelessNode *> &nodes);

#endif // WIRELESS_NODE_H

