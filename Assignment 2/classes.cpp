

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
    std::vector<std::vector<uint32_t>> board;
    std::vector<std::pair<uint32_t,std::pair<uint32_t,uint32_t>>> moves;

    public:
        Game(uint32_t player1ID,uint32_t player2ID)
        {
            board = std::vector<std::vector<uint32_t>>(3,std::vector<uint32_t>(3,0));
            this->winner = 0;
            this->player1ID = player1ID;
            this->player2ID = player2ID;
        }

        uint32_t* serialise()
        {
           
            uint32_t* data =  new uint32_t(13);
           
            *data = htonl(player1ID);
            data++;
            *data = htonl(player2ID);
            data++;
            *data = htonl((uint32_t) winner);
            data++;
            for(int row=0;row<3;row++)
            {
                for(int col=0;col<3;col++)
                {
                    *data = htonl(board[row][col]);
                    data++;
                }
            }
           
            if(isPlayer1Turn)
            {
                *data = htonl((uint32_t)1);
            }
            else
            {
                *data = (uint32_t)0;
            }
            
            return data;

        }


        void deserialise(uint32_t* data)
        {
            player1ID = ntohl(data[0]);
            cout << player1ID << endl;
            player2ID = ntohl(data[1]);
             cout << player2ID << endl;
            winner = (int32_t)ntohl(data[2]);
            cout << winner << endl;
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





        }

        int32_t move(int row,int col,int player)
        {
            if(winner != 0)
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

        void abandonGame()
        {
            winner = 4;
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