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
    address.sin_addr.s_addr = inet_addr("127.0.0.1");;
    address.sin_port = htons(8000);

    unsigned int addrlen = sizeof(address);

    
    if(connect(fd,(sockaddr*)&address,addrlen) == -1)
    {
        cout << "Unable to connect to server" << endl;
        return -1;
    }
    uint32_t id;
    cout << "Connected to server.";
    read(fd,(char*)&id,sizeof(uint32_t));
  
    Player p(id,fd);
    cout << "You are id " << id << endl;
    cout << "Waiting for opponent to join... "<< endl;
    Game* g = new Game(0,0);
    while(true)
    {
      uint32_t* data = new uint32_t[13];
        int ret = read(fd,data,sizeof(uint32_t)*13);

        if(ret <= 0)
        {
            cout << "Disconnected from server. " << endl;
            break;
        }
        
        g->deserialise(data);
        
        delete data;


         g->displayBoard();
    if(g->getWinner() == 0)
    {
     

        if((g->isP1Turn() && g->getPlayer1ID() == id) || (!(g->isP1Turn()) && g->getPlayer2ID() == id))
        {
             
            cout << "Your turn to move: <row col> (move within 15 seconds or else timeout will happen) ";
           
            int row;
            int col;
            cin >> row >> col;

            cout << endl;

            uint32_t* move = new uint32_t[2];
            move[0] = row;
            move[1] = col;
            write(fd,move,sizeof(uint32_t)*2);
          

        }
        else
        {
            cout << "Waiting for opponent to move..." << endl;
        }

    } 
    else if(g->getWinner() == 1)
    {
        cout << "Player X (id: " << g->getPlayer1ID() << ") won! " << endl;
        break;
        
    }
    else if(g->getWinner() == 2)
    {
        cout << "Player O (id: " << g->getPlayer2ID() << ") won! " << endl;
        break;
    }
    else if(g->getWinner() == 3)
    {
         cout << "Game is a tie." << endl;
         break;
    }   
    else if(g->getWinner() == 4)
    {
        cout << "Sorry you or your partner disconnected/timed out. " << endl;
        break;
    }
    
}



}
