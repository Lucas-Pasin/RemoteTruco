#pragma once
#include "raylib.h"
#include "player.h"

class UI {
public:
    void Draw(const player& jogador);
    void DrawHand(const player& jogador);
    Rectangle RecOffsetPos(Rectangle rec, float offsetX,float offsetY);


    private: 
        Rectangle handRect = { 200.0f, 500.0f , 400.0f, 150.0f };
        //Rectangle cardRec = { 250.0f, 518.5f , CARD_WIDTH, CARD_HEIGHT};
};