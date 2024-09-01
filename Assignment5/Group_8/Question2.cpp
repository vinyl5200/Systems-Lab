#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

using namespace std;

mutex mtx; // mutex for general output
mutex bridge_mtx; // mutex for access to on_bridge variable
int on_bridge = 0; // number of people currently on the bridge
int max_limit; // maximum number of people allowed on the bridge at once

void crossBridge1(int id, string direction) { //this function is to implement case 1
    // Approach bridge
    mtx.lock();
    cout << "Person " << id << " is travelling " << direction << " towards the bridge." << endl;
    mtx.unlock();
    
    // Cross bridge
    mtx.lock();
    cout << "Person " << id << " is crossing the bridge.\n" << endl;
    mtx.unlock();
    this_thread::sleep_for(chrono::seconds(rand() % 5 + 1));
    
    // Exit bridge
    mtx.lock();
    cout << "Person " << id << " has crossed the bridge and is now on the other side." << endl;
    mtx.unlock();
}


void crossBridge2(int id, string direction) { //this function is to implement case 2
    // Approach bridge
    mtx.lock();
    cout << "Person " << id << " is trying to accesss the " << direction << " towards the bridge." << endl;
    mtx.unlock();
    
    // Cross bridge
    bridge_mtx.lock(); // lock access to on_bridge variable
    mtx.lock(); // lock general output
    //below if condition is to prevent deadlock
    if (on_bridge >= max_limit) { // if maximum number of people on the bridge has been reached
        mtx.unlock(); // unlock general output
        bridge_mtx.unlock(); // unlock access to on_bridge variable
        cout<<"\nBridge max_limit reached.\n\n";
        this_thread::sleep_for(chrono::seconds(1)); // wait for a short period of time
        crossBridge2(id, direction); // recursively call function to try crossing the bridge again
        return;
    }
    on_bridge++; // increment number of people on the bridge
    cout << "Person " << id << " is crossing the bridge. \nCurrent number of people on the bridge: " << on_bridge << endl<<endl;
    mtx.unlock(); // unlock general output
    bridge_mtx.unlock(); // unlock access to on_bridge variable
    
    this_thread::sleep_for(chrono::seconds(rand() % 5 + 1)); // simulate crossing the bridge
    
    // Exit bridge
    bridge_mtx.lock(); // lock access to on_bridge variable
    mtx.lock(); // lock general output
    on_bridge--; // decrement number of people on the bridge
    cout << "Person " << id << " has crossed the bridge and is now on the other side. \nCurrent number of people on the bridge: " << on_bridge << endl<<endl;
    mtx.unlock(); // unlock general output
    bridge_mtx.unlock(); // unlock access to on_bridge variable
}

int main() {
    srand(time(NULL)); // seed random number generator
    int choice; //to implement case1 or case2
    int count_north,count_south;//to count no of northbound and southbound people respectively
    while(1)
    {   
        cout<<"\nEnter 1 for Case 1 :: 2 for Case 2 :: 0 for exit \n";
        cin>>choice;
        cout<<endl;
        if(choice==1)
        {
            cout<<"Enter no of north_bound people: ";
            cin>>count_north;
            cout<<"Enter no of south_bound people: ";
            cin>>count_south;
            cout<<endl;
            thread persons[count_north+count_south]; // array of threads for people crossing the bridge
            int temp=rand()%2; //temp is random number 0 or 1 to decide between north and south people who crosses the bridge first
            if(temp==0)
            {
                for (int i = 0; i < count_north; i++) { // create count_north number of threads for people travelling north
                    persons[i] = thread(crossBridge1, i + 1, " [[ NORTH ]] ");
                }
                for (int i = 0; i < count_north; i++) { // wait for all north_bound threads to finish
                    persons[i].join();
                }
                cout<<"\nAll northbound people successfully crossed the bridge and now southbound people will cross the bridge.......\n\n";
                for (int i = count_north; i < count_north+count_south; i++) { // create count_south number of threads for people travelling south
                    persons[i] = thread(crossBridge1, i + 1, " << SOUTH >> ");
                }
                for (int i = count_north; i < count_north+count_south; i++) { // wait for all south_bound threads to finish
                    persons[i].join();
                }
                cout<<"\nAll southbound people successfully crossed the bridge!\n\n";
            }
            else
            {
                for (int i = count_north; i < count_north+count_south; i++) { // create count_south number of threads for people travelling south
                    persons[i] = thread(crossBridge1, i + 1, " << SOUTH >> ");
                }
                for (int i = count_north; i < count_north+count_south; i++) { // wait for all south_bound threads to finish
                    persons[i].join();
                }
                cout<<"\nAll southbound people successfully crossed the bridge and now northbound people will cross the bridge.......\n\n";
                for (int i = 0; i < count_north; i++) { // create count_north number of threads for people travelling north
                    persons[i] = thread(crossBridge1, i + 1, " [[ NORTH ]] ");
                }
                for (int i = 0; i < count_north; i++) { // wait for all north_bound threads to finish
                    persons[i].join();
                }
                cout<<"\nAll northbound people successfully crossed the bridge!\n\n";
            }
        }
        else if(choice==2)
        {
            cout<<"Enter no of north_bound people: ";
            cin>>count_north;
            cout<<"Enter no of south_bound people: ";
            cin>>count_south;
            cout<<"Enter max limit of bridge: ";
            cin>>max_limit;
            cout<<endl;
            thread persons[count_north+count_south]; // array of threads for people crossing the bridge
            for (int i = 0; i < count_north; i++) { // create count_north number of threads for people travelling north
                persons[i] = thread(crossBridge2, i + 1, " [[ NORTH ]] ");
            }
               for (int i = 0; i < count_north; i++) { // wait for all north_bound threads to finish
                    persons[i].join();
                } 
           
            for (int i = count_north; i < count_north +count_south; i++) { // create count_south number of threads for people travelling south
                persons[i] = thread(crossBridge2, i + 1, " << SOUTH >> ");
            }
             for (int i = count_north; i < count_north+count_south; i++) { // wait for all south_bound threads to finish
                    persons[i].join();
                }
            
              
            cout<<"\nEveryone successfully crossed the bridge!\n\n";
        }
        else
        {
            break;
        }
        cout<<"---------------------------------------------------------------------------------------------------------------------------------\n";
    }
    return 0;
}

