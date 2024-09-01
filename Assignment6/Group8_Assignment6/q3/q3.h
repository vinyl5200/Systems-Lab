#define MAX_BUFFER_SIZE 4096

using namespace std;

// Node class for storing URL and page content
class Node {
public:
    string url;
    string page_content;
    Node* next;
    Node(string url, string page_content) {
        this->url = url;
        this->page_content = page_content;
        next = NULL;
    }
};

// LRU Cache class for caching web pages
class LRU_CACHE {
private:
    int curr_size;
    int max_size;
    Node* head;

public:
    // Constructors
    LRU_CACHE() {
        head = NULL;
        curr_size = 0;
        max_size = 5;
    }

    LRU_CACHE(int n) {
        head = NULL;
        curr_size = 0;
        max_size = n;
    }

    // Remove a URL from cache
    void remove(string url) {
        Node* temp = head;
        Node* prev = NULL;
        while (temp != NULL) {
            if (temp->url == url) {
                if (prev == NULL) {
                    head = head->next;
                    delete temp;
                    curr_size--;
                    return;
                }
                prev->next = temp->next;
                delete temp;
                curr_size--;
                return;
            }
            prev = temp;
            temp = temp->next;
        }
    }

    // Insert a URL and its content into cache
    void insert(string url, string content) {
        if (curr_size == max_size) {
            remove(head->url);
        }
        if (curr_size == 0) {
            head = new Node(url, content);
            curr_size++;
            return;
        }
        Node* temp = head;
        Node* prev = NULL;
        while (temp != NULL) {
            prev = temp;
            temp = temp->next;
        }
        prev->next = new Node(url, content);
        curr_size++;
    }

    // Find URL in cache
    pair<int, string> find_url(string url) {
        Node* temp = head;
        while (temp != NULL) {
            if (temp->url == url) {
                string content = temp->page_content;
                remove(url);
                insert(url, content);
                return make_pair(1, content);
            }
            temp = temp->next;
        }
        return make_pair(0, "");
    }

    // Print cache contents
    void printcache() {
        vector<pair<string, string>> temp;
        Node* temp1 = head;
        int count = 0;
        while (temp1 != NULL) {
            temp.push_back(make_pair(temp1->url, temp1->page_content));
            count++;
            temp1 = temp1->next;
        }
        for (int i = temp.size() - 1; i >= 0; i--) {
            cout << temp.size() - i << ". " << temp[i].first << endl;
        }
    }
};

// Function to fetch content from a web URL
pair<int, string> getcontentfromweb(string url) {
    // Extracting host address and path from the URL
    int pos = url.find("://");
    if (pos == string::npos) {
        cout << "Failure! Please add http:// or https:// in front of the URL." << endl;
        return make_pair(0, "");
    }

    string address = url.substr(pos + 3);
    string path;
    int counter = -1;
    for (int i = 0; i < address.length(); i++) {
        if (address[i] == '/') {
            counter = i;
            break;
        }
    }
    if (counter != -1) {
        path = address.substr(counter);
        address = address.substr(0, address.find("/"));
    } else {
        path = "/";
    }

    // Setting up socket connection
    int target_port = 80;
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error while creating socket");
        return make_pair(0, "");
    }
    struct hostent* server_info = gethostbyname(address.c_str());

    if (server_info == nullptr) {
        perror("Error while resolving host");
        return make_pair(0, "");
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(target_port);
    memcpy(&server_addr.sin_addr.s_addr, server_info->h_addr, server_info->h_length);
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error while connecting to server");
        return make_pair(0, "");
    }
    // Sending HTTP GET request
    string request = "GET " + path + " HTTP/1.1\r\nHost: " + address + "\r\n\r\n";
    if (send(client_socket, request.c_str(), request.length(), 0) == -1) {
        perror("Error while sending request");
        return make_pair(0, "");
    }
    // Receiving response
    string content = "";
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        content += buffer;
        if (buffer[bytes_received - 1] == '\n' && buffer[bytes_received - 2] == '\r' && buffer[bytes_received - 3] == '\n' && buffer[bytes_received - 4] == '\r') {
            break;
        }
        if (content.find("</html>") != string::npos || content.find("</HTML>") != string::npos) {
            break;
        }
    }

    if (bytes_received == -1) {
        perror("Error while receiving response");
        return make_pair(0, "");
    }

    close(client_socket);

    return make_pair(1, content);
}

