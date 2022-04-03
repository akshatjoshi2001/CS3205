#include "classes.cpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include<iostream>
#include<queue>
using namespace std;



int main()
{
    int fd = socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in address;
    if(fd == -1)
    {
        cout << "Error Occured" << endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    unsigned int addrlen = sizeof(address);

    
    if(connect(fd,(sockaddr*)&address,addrlen) == -1)
    {
        cout << "Unable to connect to server" << endl;
        return -1;
    }
    uint32_t id;
    cout << "Connected to server. Fetching id...";
    read(fd,(char*)&id,sizeof(uint32_t));
    cout << id << endl;
    Player p(id,fd);
    cout << "You are id " << id << endl;
    Game* g = new Game(0,0);
    while(true)
    {
        uint32_t* data = new uint32_t[13];
        read(fd,data,sizeof(uint32_t)*13);
           cout << "g read" << endl;
        g->deserialise(data);
        cout << "g deserialised" << endl;
        cout << g->getWinner() << endl;

    }




}
