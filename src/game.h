#pragma once
#include "carta.h"
#include "player.h"
#include "UI.h"
#include "network.h"
#include <mutex>
#include <queue>

class game{
    public: 
        player players[2];
        game();
        ~game();
        void StartGame();
        const vector<carta> &getTable() const;
        const vector<carta> &getTableEnemy() const;

        void SelectHandCard();
        
        // Detecta clique em um retângulo qualquer
        bool IsRectangleClicked(Rectangle rect);
        
        // Envia um estado (gamestate) para o servidor
        void SendGamestate(gamestate newState);
        
            // Apply the latest received network packet to the game state
        void ApplyPacoteToHands();
        // Thread-safe enqueue from network thread
        void PushPacote(const PacoteTurno& p);

        void Draw();
        void Update();
        vector <carta> mesa;
        vector <carta> mesa_enemy;

        gamestate estado;
        PacoteTurno PacoteAtual;
        bool trucoAceito = false,retrucoAceito = false, vale4Aceito = false;
        // Local pending play (applied locally until server confirmation)
        bool localPlayPending;
        PacoteTurno pendingPacket;
        network net;
    private:
    UI interface;
    // queue to receive network packets from the network thread
    std::mutex pacoteMutex;
    std::queue<PacoteTurno> pacoteQueue;
    carta deck[40]; 
};

void LoadAllCardTextures();
void UnloadAllCardTextures();