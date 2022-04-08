#include "classes.cpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <string.h>
#include <stdio.h>
#include<iostream>
#include<queue>
using namespace std;

bool askGameRestart(int fd)
{
    cout << "Would you like to restart the game? (yes/no)"<<endl;
    string ans;
    cin >> ans;
    uint32_t decision;
    if(ans == "yes" || ans == "YES" || ans == "y" || ans == "Y")
    {
        decision = 1;
    }
    else
    {
        decision = 0;
       
       
    }
   
    write(fd,&decision,sizeof(uint32_t));
    
    int32_t finaldec;
    read(fd,&finaldec,sizeof(int32_t));

    if(finaldec == 0)
    {
        cout <<"Your opponent denied playing again"<<endl;
        return false;
    }
    if(finaldec == -1)
    {
        cout << "Both players denied." << endl;
        return false;
    }
    return true;
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
    if(g->getWinner() <= 0)
    {
       if(g->getWinner() < 0)
       {
           cout << "Your previous move was illegal!" << endl;
       }

        if((g->isP1Turn() && g->getPlayer1ID() == id) || (!(g->isP1Turn()) && g->getPlayer2ID() == id))
        {
             
            cout << "Your turn to move: <row col> (move within 15 seconds or else timeout will happen) ";
           
            int row=-1;
            int col=-1;
            auto t_start = std::chrono::high_resolution_clock::now();
            cin >> row >> col;
            auto t_end = std::chrono::high_resolution_clock::now();
            double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();




           

            cout << endl;
            if(!(row ==-1 || col==-1) && !(elapsed_time_ms >= 14000))
            {

                uint32_t* move = new uint32_t[2];
                move[0] = row-1;
                move[1] = col-1;
                write(fd,move,sizeof(uint32_t)*2);
            }
          

        }
        else
        {
            cout << "Waiting for opponent to move..." << endl;
        }

    } 
    else if(g->getWinner() == 1)
    {
        cout << "Player X (id: " << g->getPlayer1ID() << ") won! " << endl;
        if(!askGameRestart(fd))
            break;

        
    }
    else if(g->getWinner() == 2)
    {
        cout << "Player O (id: " << g->getPlayer2ID() << ") won! " << endl;
        if(!askGameRestart(fd))
            break;
    }
    else if(g->getWinner() == 3)
    {
         cout << "Game is a tie." << endl;
         if(!askGameRestart(fd))
            break;
    }   
    else if(g->getWinner() == 4)
    {
        cout << "Sorry player X timed out. " << endl;

         if(!askGameRestart(fd))
            break;
    }
    else if(g->getWinner() == 5)
    {
        cout << "Sorry player O timed out. " << endl;
         if(!askGameRestart(fd))
            break;
    }
    else if(g->getWinner() == 6)
    {
        cout << "Sorry your partner disconnected. " << endl;
        break;
    }
    
    
}



}
