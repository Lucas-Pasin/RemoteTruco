#pragma once
#include "carta.h"
#include "player.h"
#include "UI.h"

class game{
    public: 
        player players[2];
        game();
        ~game();
        void StartGame();
        void shuffleDeck();
        void buildDeck();
        void dealCards();
        const vector<carta> &getTable() const;
        void SelectHandCard();

        void Draw();
        void Update();
        
    private:
        UI interface;
        vector <carta> mesa; 
        carta deck[40]; 
};

void LoadAllCardTextures();
void UnloadAllCardTextures();