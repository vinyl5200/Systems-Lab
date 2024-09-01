#include "wireless_node.h"
#include <cstdlib>
#include <ctime>
using namespace std;
const int MAX_BACKOFF_TIME = 10;
int successful_transmissions = 0;
int total_collisions = 0;
int node_counter = 1;

WirelessNode::WirelessNode()
{
  node_id = node_counter;
  node_counter++;
  is_transmitting = false;
  backoff_time = 0;
  transmission_duration = (rand() % 6) + 1;
  time_remaining = 0;
  backoff_attempts = 0;
}

void WirelessNode::update()
{
  if (backoff_time > 0)
  {
    backoff_time--;
  }
  if (is_transmitting)
  {
    time_remaining--;
  }
  if (time_remaining == 0 && is_transmitting)
  {
  
    cout<<"==============================================================\n";
    std::cout << "Node " << node_id << " completed transmission after " << backoff_attempts << " backoff attempts" << std::endl;
   cout<<"==============================================================\n";
    is_transmitting = false;
    successful_transmissions++;
    backoff_attempts = 0;
  }
}



bool is_ready_to_transmit(WirelessNode &node)
{
  return node.backoff_time == 0;
}

void handle_collision(std::vector<WirelessNode *> &nodes)
{
  total_collisions += (nodes.size() * (nodes.size() - 1)) / 2;
  for (auto node : nodes)
  {
    node->backoff_time = (rand() % MAX_BACKOFF_TIME) + 1;
    node->is_transmitting = false;
    node->time_remaining = 0;
    node->backoff_attempts++;
    std::cout << "Collision detected for Node " << node->node_id << ". Initiating backoff with duration=" << node->backoff_time << std::endl;
  }
}

