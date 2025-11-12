#pragma once
#include "carta.h"
#include "player.h"
#include "UI.h"
#include "network.h"

class game{
    public: 
        player players[2];
        game();
        ~game();
        void StartGame();
        const vector<carta> &getTable() const;
        void SelectHandCard();

        void Draw();
        void Update();
        vector <carta> mesa;
        gamestate estado;
        PacoteTurno PacoteAtual;
        network net;
    private:
        UI interface;
        
         
        carta deck[40]; 
};

void LoadAllCardTextures();
void UnloadAllCardTextures();