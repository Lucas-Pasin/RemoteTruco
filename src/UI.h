#pragma once
#include "raylib.h"
#include "player.h"

class game;

class UI {
public:
    void Draw(const game& g);
    void DrawHand(const player& jogador);
    void DrawTable(const vector<carta>& table);

    Rectangle RecOffsetPos(Rectangle rec, float offsetX,float offsetY);
    Rectangle getRec();


    private: 
        Rectangle tableRect = { 200.0f, 500.0f , 400.0f, 150.0f };
        //Rectangle cardRec = { 250.0f, 518.5f , CARD_WIDTH, CARD_HEIGHT};
};