#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

enum gamestate{PLAY,WAIT,ROUND_START};

enum Place {hand,table};

struct CardState{
    int numero;
    int score;
    naipes naipe;
    
    Place place;
};

struct PacoteTurno {
    bool isFirst;
    gamestate state;
    CardState cards[9];
};

#define LISTEN_PORT 5394
#define SERVER_IP "127.0.0.1"

class network{
    public:
        network();

        struct PacoteTurno rec_jogada();
        void getPlay(game &g);
        void startThread(game &g);
        bool sendPlay(game &g);
        void ThreadSend(void *arg);
    private:
        int sock;
        struct sockaddr_in serv_addr;

};

void recv_all(int sock, void* buffer, size_t size);
void send_all(int sock, void* buffer, size_t size);
bool sendPlay(game &g);

