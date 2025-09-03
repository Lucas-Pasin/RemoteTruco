#pragma once
#include "raylib.h"

extern Texture2D cardTextures[40];
extern Texture2D cardBackTexture;

typedef enum{
    DIAMOND = 0,
    HEART,
    SPADE,
    CLUB
} naipes;


class carta{
    public:
        //constructors
        carta();
        carta(int num,naipes naipe,int score,Texture2D* textura = &cardBackTexture);

        //destructor
        ~carta();

        //methods
        int getNumber();
        naipes getNaipe();
        int getScore();
        Rectangle getRect();
        void setPos(float x,float y);
        
        // void setScore(int score);

        //rendering
        void updateDraw();
        void Draw() const;
    
    private:
        int numero;
        naipes naipe;
        int score;
        Rectangle cartaRect = { 400.0f, 400.0f , 70.0f, 90.0f };
        Texture2D* textura;
};