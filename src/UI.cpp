#include "UI.h"


Rectangle UI::RecOffsetPos(Rectangle rec, float offsetX,float offsetY){
    rec.x += offsetX;
    rec.y += offsetY; 
    return rec;
}

void UI::DrawHand(const player& jogador){
    for(int i=0;i<3;i++){
        jogador.mao[i].Draw();
    }
}


// void UI::DrawTable(const player& jogador){
//     for(int i=0;i<3;i++){
//         jogador.mao[i].Draw();
//     }
}

void UI::Draw(const player& jogador){
    DrawRectangleLinesEx(handRect,2.0f, WHITE);
    DrawHand(jogador);
}