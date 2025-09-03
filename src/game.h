#pragma once
#include "carta.h"
#include "player.h"
#include "UI.h"

class game{
    public: 
        player players[2];
        int mesacount=0;
        game();
        ~game();
        void StartGame();
        void shuffleDeck();
        void buildDeck();
        void dealCards();
        void SelectHandCard();

        void Draw();
        void Update();
        
    private:
        UI interface;
        carta mesa[6]; 
        carta deck[40]; 
};

void LoadAllCardTextures();
void UnloadAllCardTextures();