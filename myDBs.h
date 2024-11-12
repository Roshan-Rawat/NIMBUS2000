#ifndef MYDBS_H
#define MYDBS_H


#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<assert.h>
#include<errno.h>
#include<winsock2.h>
#include <ws2tcpip.h>




#define HOST      "127.0.0.1"
#define PORT      14000

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8; 



struct s_client{
    int s;
    char ip[16];
    int16 port;
};



typedef struct s_client Client;


void childloop(Client*); 
void zeroTheStructure(int8* , int16);
SOCKET initServer(int16);
void mainloop(SOCKET); 
int main(int , char**); 

#endif