#pragma once
#include "raylib.h"
#include "player.h"
#include "network.h"


class game;

class UI {
public:
    void Draw(const game& g);
    void DrawHand(const player& jogador);
    void DrawTable(const vector<carta>& table);
    void DrawGamestate(gamestate state, float centerX = 300.0f, float centerY = 400.0f);
    void DrawTrucoButton(gamestate state, Rectangle buttonRect);
    void DrawTrucoResponseButtons(gamestate state);

    Rectangle RecOffsetPos(Rectangle rec, float offsetX,float offsetY);
    Rectangle getRec();
    Rectangle getTrucoButtonRect() const;
    Rectangle getAcceptButtonRect() const;
    Rectangle getRejectButtonRect() const;


    private: 
        Rectangle tableRect = { 200.0f, 500.0f , 400.0f, 150.0f };
        Rectangle trucoButtonRect = { 650.0f, 300.0f, 120.0f, 50.0f };
        Rectangle acceptButtonRect = { 620.0f, 240.0f, 80.0f, 40.0f };
        Rectangle rejectButtonRect = { 720.0f, 240.0f, 80.0f, 40.0f };
        //Rectangle cardRec = { 250.0f, 518.5f , CARD_WIDTH, CARD_HEIGHT};
};