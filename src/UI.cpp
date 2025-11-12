#include "UI.h"
#include "game.h"


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


void UI::DrawTable(const vector<carta>& table){
    for (const auto& card : table) {
        card.Draw();
    }
}

Rectangle UI::getRec(){
    return this->tableRect;
}

void UI::Draw(const game& g){
    DrawRectangleLinesEx(tableRect,2.0f, WHITE);
    DrawTable(g.getTable());
    DrawHand(g.players[0]);
}