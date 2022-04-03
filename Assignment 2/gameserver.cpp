#include "classes.cpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include<iostream>
#include<queue>
using namespace std;

queue<Player*> q;
int id = 1;

void handleClient(int clientfd)
{
    cout << "Client trying to connect..." << endl;
    Player* p = new Player(id,clientfd);
    id++;
    uint32_t* serialised = p->serialise();
    write(clientfd,(char*) serialised,sizeof(*(serialised)));
    cout << "Client ID sent" << endl;
    if(q.empty())
    {
        q.push(p);
        
    }
    else
    {
        Player p1 = *(q.front());
        Player p2 = *(p);
        //q.pop();
        cout << "check" << endl;
        Game* g = new Game(p1.id,p2.id);
        uint32_t* serialisedGame = g->serialise();
        write(p2.fd, serialisedGame,sizeof(uint32_t)*13);
        write(p1.fd, serialisedGame,sizeof(uint32_t)*13);
        cout << "check2" << endl;
    }    
}


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

    if(bind(fd,(sockaddr*)&address,sizeof(address)) != -1)
    {
        if(listen(fd,10) != -1)
        {
            while(true)
            {
                int clientfd = accept(fd,(sockaddr*) &address,(socklen_t*) &addrlen);
                cout << "client detected " <<endl;
                if(clientfd != -1)
                {
                   handleClient(clientfd);
                }
            }
            
        }
    }
    else
    {
        cout << "error. unable to bind. Port maybe already occupied." << endl;
    }
    close(fd);





    return 0;
}


