// #include "myDBs.h"

// // Global variable to control the server loop
// bool scontinuation;
// bool ccontinuation ; 

// void handleError(const char* msg) {
//     printf("%s: %d\n", msg, WSAGetLastError());
//     WSACleanup(); // Clean up Winsock
//     exit(EXIT_FAILURE);
// }


// void zeroTheStructure(int8 * buf , int16 size){
//     int8 *p ;
//     int16 i ; 

//     for(i = 0 , p = buf; i < size ; ++i , ++p){
//         *p = 0 ;
//     }
//     return;
// }



// void childloop(Client *cli){
//     sleep(1);
//     return ; 
// }

// void mainloop(SOCKET s) {
//     struct sockaddr_in cli;
//     SOCKET s2;
//     int len = sizeof(cli);
//     char *ip;
//     int16 port;
//     Client *client ;
//     pid_t pid ; 

//     s2 = accept(s, (struct sockaddr *)&cli, &len);
//     if (s2 == INVALID_SOCKET) {
//         handleError("Accept failed");
//         return;
//     }

//     port = (int16)ntohs(cli.sin_port); // Use 'ntohs' instead of 'htons'
//     ip = inet_ntoa(cli.sin_addr);
//     printf("Connection from %s:%d\n", ip, port);

//     client = (Client *)malloc(sizeof(struct s_client));
//     assert(client); 
//     zeroTheStruture((int8 *)client , sizeof(struct s_client));
//     client->s = s;
//     client->port = port;
//     strncpy(client->ip,ip,15); 

//     pid = fork() ; 
//     if(pid){
//         free(client);
//         return;
//     }
//     else{
//         dprintf(s2, "100 Connected to Nimbus2000 server\n");
//         ccontinuation = true ; 
//         while(ccontinuation)
//             childloop(client);
        
//         close(s2) ;
//         free(client);
//         return;
//     }
//     // Here you would handle the communication with the client.
//     //closesocket(s2);
// }

// SOCKET initServer(int16 port) {
//     struct sockaddr_in sock;
//     SOCKET s;

//     // Step 1: Create the socket
//     s = socket(AF_INET, SOCK_STREAM, 0);
//     if (s == INVALID_SOCKET) {
//         handleError("Socket creation failed");
//     }

//     // Step 2: Set up the sockaddr_in structure
//     sock.sin_family = AF_INET;
//     sock.sin_port = htons(port);
//     sock.sin_addr.s_addr = inet_addr(HOST);

//     // Step 3: Bind the socket
//     if (bind(s, (struct sockaddr*)&sock, sizeof(sock)) == SOCKET_ERROR) {
//         handleError("Bind failed");
//     }

//     // Step 4: Listen for incoming connections
//     if (listen(s, SOMAXCONN) == SOCKET_ERROR) {
//         handleError("Listen failed");
//     }

//     printf("Server is listening on %s:%d...\n", HOST, port);

//     return s;
// }

// int main(int argc, char *argv[]) {
//     int16 port;

//     // Initialize Winsock
//     WSADATA wsaData;
//     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//         printf("WSAStartup failed with error: %d\n", WSAGetLastError());
//         return 1;
//     }

//     // Determine port
//     if (argc < 2) {
//         port = PORT; // Use default port from the header
//     } else {
//         port = (int16)atoi(argv[1]);
//     }

//     scontinuation = true;
//     SOCKET s = initServer(port);
    
//     while (scontinuation) {
//         mainloop(s); 
//         // You might want to set scontinuation to false at some point to exit the loop.
//         // Example: Check for a condition to stop the server loop.
//     }

//     closesocket(s);
//     // Cleanup Winsock
//     WSACleanup();
//     return 0;
// }



#include "myDBs.h"

// Global variable to control the server loop
bool scontinuation;
bool ccontinuation; 

void handleError(const char* msg) {
    printf("%s: %d\n", msg, WSAGetLastError());
    WSACleanup(); // Clean up Winsock
    exit(EXIT_FAILURE);
}


void zeroTheStructure(int8 * buf , int16 size){
    int8 *p ;
    int16 i ; 

    for(i = 0 , p = buf; i < size ; ++i , ++p){
        *p = 0 ;
    }
    return;
}

// Function that runs in a separate thread for handling client communication
DWORD WINAPI clientHandler(LPVOID lpParam) {
    Client *client = (Client *)lpParam;
    SOCKET s2 = client->s;
    char buffer[1024];

    // Sending a welcome message to the client
    const char *message = "100 Connected to Nimbus2000 server\n";
    send(s2, message, strlen(message), 0);

    ccontinuation = true;
    while (ccontinuation) {
        // Example: Receiving data from the client (can modify for actual use)
        int recvSize = recv(s2, buffer, sizeof(buffer) - 1, 0);
        if (recvSize > 0) {
            buffer[recvSize] = '\0';
            printf("Received: %s\n", buffer);
        } else {
            printf("Connection closed by client.\n");
            ccontinuation = false;
        }
    }

    // Close the connection and clean up
    closesocket(s2);
    free(client);
    return 0;
}

void mainloop(SOCKET s) {
    struct sockaddr_in cli;
    SOCKET s2;
    int len = sizeof(cli);
    char *ip;
    int16 port;
    Client *client;
    DWORD threadId;

    s2 = accept(s, (struct sockaddr *)&cli, &len);
    if (s2 == INVALID_SOCKET) {
        handleError("Accept failed");
        return;
    }

    port = (int16)ntohs(cli.sin_port); // Use 'ntohs' instead of 'htons'
    ip = inet_ntoa(cli.sin_addr);
    printf("Connection from %s:%d\n", ip, port);

    // Allocate and initialize client structure
    client = (Client *)malloc(sizeof(struct s_client));
    assert(client); 
    zeroTheStructure((int8 *)client, sizeof(struct s_client));
    client->s = s2;
    client->port = port;
    strncpy(client->ip, ip, 15);

    // Create a new thread to handle the client connection
    HANDLE hThread = CreateThread(NULL, 0, clientHandler, (LPVOID)client, 0, &threadId);
    if (hThread == NULL) {
        printf("Failed to create thread for client\n");
        closesocket(s2);
        free(client);
        return;
    }

    // Close the thread handle (the thread will keep running independently)
    CloseHandle(hThread);
}

SOCKET initServer(int16 port) {
    struct sockaddr_in sock;
    SOCKET s;

    // Step 1: Create the socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        handleError("Socket creation failed");
    }

    // Step 2: Set up the sockaddr_in structure
    sock.sin_family = AF_INET;
    sock.sin_port = htons(port);
    sock.sin_addr.s_addr = inet_addr(HOST);

    // Step 3: Bind the socket
    if (bind(s, (struct sockaddr*)&sock, sizeof(sock)) == SOCKET_ERROR) {
        handleError("Bind failed");
    }

    // Step 4: Listen for incoming connections
    if (listen(s, SOMAXCONN) == SOCKET_ERROR) {
        handleError("Listen failed");
    }

    printf("Server is listening on %s:%d...\n", HOST, port);

    return s;
}

int main(int argc, char *argv[]) {
    int16 port;

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Determine port
    if (argc < 2) {
        port = PORT; // Use default port from the header
    } else {
        port = (int16)atoi(argv[1]);
    }

    scontinuation = true;
    SOCKET s = initServer(port);
    
    while (scontinuation) {
        mainloop(s); 
        // You might want to set scontinuation to false at some point to exit the loop.
    }

    closesocket(s);
    // Cleanup Winsock
    WSACleanup();
    return 0;
}
