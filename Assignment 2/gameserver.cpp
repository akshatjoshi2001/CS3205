#include "classes.cpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <unordered_map>
#include<iostream>
#include<queue>
#include <fcntl.h>
#include <sys/select.h>
using namespace std;

queue<Player*> q;
unordered_map<int,Player*> playerMap;
int id = 1;

pair<int,int> getMove(Player* p)
{
    uint32_t* move = new uint32_t[2];
    timeval timeout;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(p->fd, &set);

    timeout.tv_sec = 15;
    timeout.tv_usec = 0;

    int ret =  select(p->fd+1,&set,nullptr,nullptr,&timeout);  //
    if(ret <= 0)
    {
    
        delete[] move;
        
        return {-1,-1};
    }
    else{
        read(p->fd,move,sizeof(uint32_t)*2);
    }
    uint32_t row = move[0];
    uint32_t col = move[1];
    delete[] move;
    return {row,col};
   

}
void sendToPlayer(Game* g,int player)
{
    uint32_t* serialisedGame = g->serialise();
    if(player == 1)
    {
       write(playerMap[g->getPlayer1ID()]->fd, serialisedGame,sizeof(uint32_t)*13);
    }
    else if(player == 2)
    {
        write(playerMap[g->getPlayer2ID()]->fd, serialisedGame,sizeof(uint32_t)*13);
    }
   
     delete[] serialisedGame; 
}

void broadcastGameState(Game* g)
{
        uint32_t* serialisedGame = g->serialise();
       write(playerMap[g->getPlayer1ID()]->fd, serialisedGame,sizeof(uint32_t)*13);
        write(playerMap[g->getPlayer2ID()]->fd, serialisedGame,sizeof(uint32_t)*13);
   
   
     delete[] serialisedGame; // Avoid memory leaks :)
}


void handleGame(Game* g)
{
    
  
    

    while(g->getWinner() == 0)
    {
            if(g->isP1Turn())
            {
                Player* p = playerMap[g->getPlayer1ID()];
              
               pair<int,int> move = getMove(p);
               int row = move.first;
               int col = move.second;

               if(row == -1 && col == -1)
               {
                   g->abandonGame();
                 
                   sendToPlayer(g,2);
                  
                   break;
               }

                 
                g->move(row,col,1);
               
                
            }
            else
            {
               
                Player* p = playerMap[g->getPlayer2ID()];
               
               pair<int,int> move = getMove(p);
               
               int row = move.first;
               int col = move.second;
                if(row == -1 && col == -1)
               {
                  g->abandonGame();
                  
                   sendToPlayer(g,1);
                  
                   break;
               }
               
                
                g->move(row,col,2);
                

            }

        broadcastGameState(g);
            

     

        
    }


}

void handleClient(int clientfd)
{
   
    Player* p = new Player(id,clientfd);
    playerMap[id] = p;
    id++;
    uint32_t* serialised = p->serialise();
    write(clientfd,(char*) serialised,sizeof(*(serialised)));
   
    if(q.empty())
    {
        q.push(p);
        
    }
    else
    {
        Player p1 = *(q.front());
        Player p2 = *(p);
        q.pop();
       
      
        Game* g = new Game(p1.id,p2.id);
        
        broadcastGameState(g);
        
    
       
         thread gameThread(handleGame,g);
         gameThread.detach();
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


