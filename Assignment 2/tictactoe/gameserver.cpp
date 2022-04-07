#include "classes.cpp"
#include "decls.hpp"


using namespace std;

queue<Player*> q;
unordered_map<int,Player*> playerMap;
vector<Game*> games;
int id = 1;


void createGame(int player1ID,int player2ID)
{
        Game* g = new Game(player1ID,player2ID);
        
        broadcastGameState(g);
        games.push_back(g);

         thread gameThread(handleGame,g);
         gameThread.detach();
    
    
}


void askForGameRestart(Game* g)
{
  Player* p1 = playerMap[g->getPlayer1ID()];
  Player* p2 = playerMap[g->getPlayer2ID()];
  
  uint32_t decision1=0;
  uint32_t decision2=0;
  
  int ret1 = read(p1->fd,&decision1,sizeof(uint32_t));
  int ret2 = read(p2->fd,&decision2,sizeof(uint32_t));
  
  if(ret1 > 0 && ret2 >0)
  {
     
      if(decision1 == 1 && decision2 == 1)
      {
            write(p1->fd,&decision2,sizeof(uint32_t));
        write(p2->fd,&decision1,sizeof(uint32_t));
        
        createGame(p1->id,p2->id);
      
        return;
      }
      else if(decision1 == 1)
      {
         
           write(p1->fd,&decision2,sizeof(uint32_t));
      }
      else if(decision2 == 1)
      {
           write(p2->fd,&decision1,sizeof(uint32_t));
      }
      else
      {
          decision1 = -1;
          decision2 = -1;

         write(p1->fd,&decision2,sizeof(uint32_t));
         write(p2->fd,&decision1,sizeof(uint32_t));
         

      }
  }
  else if(ret1 <= 0 && ret2 > 0)
  {
     
    write(p2->fd,&decision1,sizeof(uint32_t));

  }
  else if(ret2 <= 0 && ret1 > 0)
  {
     
       write(p1->fd,&decision2,sizeof(uint32_t));

  }



}

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
    if(ret < 0)
    {
    
        delete[] move;
        
        return {-1,-1};
    }
    else if(ret == 0)
    {
        delete[] move;
        
        return {-2,-2};
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

               if( (row == -1) && (col == -1) ) 
               {
                   g->abandonGame(6);
                 
                   sendToPlayer(g,2);
                  
                   break;
               }
               else if( (row == -2) && (col == -2) )
               {
                   g->abandonGame(4);
                   broadcastGameState(g);
                    askForGameRestart(g);
                  
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
                  g->abandonGame(6);
                  
                   sendToPlayer(g,1);
                  
                   break;
               }
               else if(row == -2 && col == -2)
               {
                    g->abandonGame(5);
                   broadcastGameState(g);

                   askForGameRestart(g);
                   
                   break;
               }
               
                
                g->move(row,col,2);
                

            }

        broadcastGameState(g);
            

     

        
    }
    if(g->getWinner() == 1 || g->getWinner() ==2 || g->getWinner() == 3)
    {
        
        askForGameRestart(g);
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
       
        createGame(p1.id,p2.id);
    }    
}

static void handleSigInt(int sig)
{
    ofstream out{ "logs.txt" };
    cout << "Saving Logs and exiting..." << endl;

    out << "Games played: " << games.size() << endl;

    for(int i=0;i<games.size();i++)
    {
        out << "----------------------------------------" << endl;
        out << "Game Number " << (i+1) << ": Played between " << games[i]->getPlayer1ID() << "(X) and " << games[i]->getPlayer2ID() << "(O) " << endl;
        int winner = games[i]->getWinner();
        out << "Game result: ";
        if(winner == 0)
        {
            out << "Was in progress";
        }
        if(winner == 1)
        {
            out << "Player X won";
        }
        if(winner == 2)
        {
            out << "Player O won";
        }
        if(winner == 3)
        {
            out << "Game was a tie";
        }
        if(winner == 4)
        {
            out << "Player X timed out";
        }
        if(winner == 5)
        {
            out << "Player O timed out";
        }
        if(winner == 6)
        {
            out << "One of the players disconnected from the server.";
        }

        out<<endl;
        for(int j=0;j<games[i]->moves.size();j++)
        {
            out << ((games[i]->moves[j].first == 1)?"X":"O");
            out << " moved at position: " << games[i]->moves[j].second.first << " " << games[i]->moves[j].second.second << endl;

        }
    }

    exit(EXIT_SUCCESS);
}


int main()
{
    cout << "Press Ctrl+C to save logs and exit" << endl;
    int fd = socket(AF_INET,SOCK_STREAM,0);
    signal(SIGINT, handleSigInt);
    sockaddr_in address;
    if(fd == -1)
    {
        cout << "Error Occured" << endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8000);

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


