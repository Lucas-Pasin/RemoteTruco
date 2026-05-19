#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

enum gamestate{
    PLAY,
    WAIT,
    ROUND_START,
    WIN,
    LOSE,
    TRUCO,
    TRUCO_ACCEPT,
    TRUCO_REJECT,
    RETRUCO,
    RETRUCO_ACCEPT,
    RETRUCO_REJECT,
    VALE4,
    VALE4_ACCEPT,
    VALE4_REJECT,
    ROUND_WIN,
    ROUND_LOSE,
    PLAYED_CARD
};

enum Place {hand,table,enemy_table};

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

class game; // forward declare to avoid circular include

class network{
    public:
        network();
        struct PacoteTurno rec_jogada();
        bool isConnected();
        void connectToServer(string ip = SERVER_IP, int port = LISTEN_PORT);
        void checkConnectionStatus(); // Verifica se conexão assíncrona completou
        void getPlay(game &g);
        void startThread(game &g);
        bool sendPlay(game &g);
        void ThreadSend(void *arg);
    private:
        int sock;
        struct sockaddr_in serv_addr;
        bool connected = false; // connection status

};

void recv_all(int sock, void* buffer, size_t size);
void send_all(int sock, void* buffer, size_t size);


