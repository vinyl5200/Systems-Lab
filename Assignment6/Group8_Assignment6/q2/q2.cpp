#include "wireless_node.h"
#include "function.h"
#include <vector>
#include <ctime>
using namespace std;

int main()
{
  srand(time(nullptr));

  int num_nodes, simulation_duration;
  cout << "Enter the number of nodes: ";
  cin >> num_nodes;
  cout << "Enter the simulation duration: ";
  cin >> simulation_duration;
  vector<WirelessNode> nodes;

  for (int i = 0; i < num_nodes; i++)
  {
    nodes.push_back(WirelessNode());
    cout << "Node: " << nodes[i].node_id << ", transmission duration: " << nodes[i].transmission_duration << endl;
  }

  for (int time = 0; time < simulation_duration; time++)
  {
    cout << endl
         << "Time t = " << time + 1 << ":" << endl;
    vector<WirelessNode *> ready;
     
     int channel_status=0;
      for (const auto &node : nodes)
  	{
    		if (node.is_transmitting)
    		{
      		channel_status= node.node_id;
    		}
  	}
   
     
  //  int channel_status = is_channel_idle(nodes);

    for (WirelessNode &node : nodes)
    {
      if (is_ready_to_transmit(node))
      {
        if (channel_status > 0)
        {
          if (node.node_id != channel_status)
          {
            cout << "BUSY CHANNEL! ";
            cout << "Node " << node.node_id << " detects the channel being used by " << channel_status << endl;
            node.backoff_time = (rand() % MAX_BACKOFF_TIME) + 1;
          }
          else
          {
            cout << "Node " << channel_status << " is transmitting, ";
            cout << "Time remaining: " << node.time_remaining << endl;
          }
        }
        else
        {
          ready.push_back(&node);
        }
      }
      else
      {
        cout << "Node " << node.node_id << " is waiting (backoff=" << node.backoff_time << ")." << endl;
      }
    }

    if (ready.size() == 1)
    {
      cout << "Node " << ready[0]->node_id << " begins transmitting" << endl;
      ready[0]->is_transmitting = true;
      ready[0]->time_remaining = ready[0]->transmission_duration;
      ready[0]->backoff_time = 0;
    }
    else
    {
      handle_collision(ready);
    }

    for (WirelessNode &node : nodes)
    {
      node.update();
    }

    cout << "                                           " << endl;
  }

  cout << endl;
  cout << "************************************************\n";
  cout << "Total Successful Transmissions: " << successful_transmissions << endl;
  cout << "Total Collisions: " << total_collisions << endl;
  cout << "************************************************\n";

  return 0;
}
