

#include <vector>
#include <cstdint>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include<iostream>
#include <stdio.h>
using std::uint32_t;
using namespace std;


class Game
{
    uint32_t player1ID;  
    uint32_t player2ID; // Player ID > 0. If PlayerID = 0, it means player has not joined.
    

    bool isPlayer1Turn; 
    int32_t winner; // 0->Game continues, 1-> Player 1 Won, 2-> Player 2 Won, 3->Tie, 4-> Disconnected/Abandoned


    public:
        std::vector<std::vector<uint32_t>> board;
    std::vector<std::pair<uint32_t,std::pair<uint32_t,uint32_t>>> moves;
        Game(uint32_t player1ID,uint32_t player2ID)
        {
            board.resize(3,{0,0,0});

          
            this->winner = 0;
            this->player1ID = player1ID;
            this->player2ID = player2ID;
            this->isPlayer1Turn = true;
        }

        uint32_t* serialise()
        {
           
            uint32_t* data =  new uint32_t[13];
           
           
            data[0] = htonl(player1ID);
           
            data[1] = htonl(player2ID);
            
            data[2] = htonl((uint32_t) winner);
            int idx = 3;
            for(int row=0;row<3;row++)
            {
                for(int col=0;col<3;col++)
                {
                    
                    data[idx] = board[row][col];
                    idx++;
                }
            }
           
            if(isPlayer1Turn)
            {
                data[idx] = htonl((uint32_t)1);
            }
            else
            {
                data[idx] = (uint32_t)0;
            }
           
            
            return data;

        }

        bool isP1Turn()
        {
            return isPlayer1Turn;
        }


        void deserialise(uint32_t* data)
        {
            player1ID = ntohl(data[0]);
           
            player2ID = ntohl(data[1]);
           
            winner = (int32_t)ntohl(data[2]);
           
            int idx = 3;
            for(int row=0;row<3;row++)
            {
                for(int col=0;col<3;col++)
                {
                    board[row][col] = data[idx];
                    idx++;
                }
            }
            if(ntohl(data[idx]) == 1)
            {
                isPlayer1Turn = true;
            }
            else
            {
                isPlayer1Turn = false;
            }

        }

        void checkWinOrTie()
        {
            bool isTie = true;

            for(int row=0;row<3;row++)
            {
                if(board[row][0] != 0 && (board[row][0] == board[row][1]) && (board[row][1] == board[row][2]))
                {
                    winner = board[row][0];
                    return;
                }

            }
            for(int col=0;col<3;col++)
            {
                if(board[0][col] != 0 && (board[0][col] == board[1][col]) && (board[2][col] == board[0][col]))
                {
                    winner = board[0][col];
                    return;
                }
                
            }

            if((board[0][0] !=0) && (board[0][0] == board[1][1]) && (board[1][1] == board[2][2]))
            {
                winner = board[0][0];
                return;
            }

             if((board[2][0] !=0) && (board[2][0] == board[1][1]) && (board[1][1] == board[0][2]))
            {
                winner = board[2][0];
                return;
            }

            for(int row=0;row<3;row++)
            {
                for(int col=0;col<3;col++)
                {
                    if(board[row][col] == 0)
                    {
                        isTie = false;
                        break;
                    }
                }
            }
            if(isTie)
            {
                winner = 3;
            }





        }

        int32_t move(int row,int col,int player)
        {
            if(winner != 0)
                return -1;
            if(row>=3 || row<-1 || col>=3 || col<-1)
                return -1;
            if(player1ID == 0 || player2ID == 0)
            {
                return -1;
            }
            if(isPlayer1Turn && player ==1 || (!isPlayer1Turn && player == 2))
            {
                
                if(board[row][col] == 0)
                {
                   
                    board[row][col] = player;
                    isPlayer1Turn = !isPlayer1Turn;
                  
                    checkWinOrTie();
                   
                    moves.push_back({player,{row,col}});
                    return winner; 
                }
                else
                {
                    return -2; // Position already occupied
                }
            }
            else
            {
                return -1;  // Move not allowed by player.
            }
        }

        int32_t getWinner()
        {
            return winner;
        }

        uint32_t getPlayer1ID()
        {
            return player1ID;
        }
        uint32_t getPlayer2ID()
        {
            return player2ID;
        }

        void abandonGame()
        {
            winner = 4;
        }

        void displayBoard()
        {
            cout << "\033[2J\033[1;1H"; // Clear Screen
            cout << "Match between " << player1ID << " (X) and " << player2ID << "(O)" << endl;
            for(int row=0;row<3;row++)
            {
                for(int col=0;col<3;col++)
                {
                    cout << "|";
                  
                    if(board[row][col] == 1)
                    {
                        cout << "X";
                    }
                    else if(board[row][col] == 2)
                    {
                       cout << "O";
                    }
                    else
                    {
                        cout << "_";
                    }
                  
                    cout << "|";


                }
                cout << endl;
            }
        }




};

class Player
{
    public:
        uint32_t id;
        int fd; // File Descriptor of the Player Client
        
        Player(uint32_t id,int fd)
        {
            this->id = id;
            this->fd = fd;
        }
        uint32_t* serialise()
        {
            uint32_t* data = new uint32_t;
            *data = (id);
            return data;
        }


};