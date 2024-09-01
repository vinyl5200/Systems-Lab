#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include "q3.h"

int main() {
    // Create a new LRU cache
    LRU_CACHE* cache = new LRU_CACHE();
    // Main loop
    while (true) {
        // Prompt for URL input
        cout << "\nEnter URL (or 'exit' to quit): ";
        string url;
        cin >> url;
        if (url == "exit") {
            break;
        }
        // Check if URL is in cache
        pair<int, string> search = cache->find_url(url);
        if (search.first == 1) {
            cout << "The requested URL is found in the cache." << endl;
            cout << "Content in the requested url:" << endl;
            cout << search.second << endl;
        } else {
            cout << "\nThe requested URL was not found in the cache." << endl;
            cout << "Trying to fetch content..." << endl;
            // Fetch content from web
            pair<int, string> temp = getcontentfromweb(url);
            if (temp.first == 0) {
                cout << "Error! Failure while trying to fetch content." << endl;
                continue;
            }
            cout << "\nPage content: " << endl;
            cout << temp.second << endl;
            // Insert content into cache
            cache->insert(url, temp.second);
        }
        // Print current cache contents
        cout << "\nCache Contents (Most to Least Recently Used): " << endl;
        cache->printcache();
    }
    cout << "You have exited." << endl;
    // Cleanup
    delete cache;
}

