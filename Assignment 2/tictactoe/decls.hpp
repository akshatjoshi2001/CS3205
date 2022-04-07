#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
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
#include <signal.h>

void createGame(int,int);
void askForGameRestart(Game*);
pair<int,int> getMove(Player*);
void sendToPlayer(Game*,int);
void broadcastGameState(Game*);
void handleGame(Game*);
void handleClient(int);
static void handleSigInt(int);

