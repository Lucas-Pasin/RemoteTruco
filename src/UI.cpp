#include "UI.h"
#include "game.h"


Rectangle UI::RecOffsetPos(Rectangle rec, float offsetX,float offsetY){
    rec.x += offsetX;
    rec.y += offsetY; 
    return rec;
}

void UI::DrawHand(const player& jogador){
    for (size_t i = 0; i < jogador.mao.size(); ++i) {
        if (jogador.mao[i].isActive()) {
            jogador.mao[i].Draw(true);
        }
    }
}


void UI::DrawTable(const vector<carta>& table){
    for (const auto& card : table) {
        card.Draw(true);
    }
}

Rectangle UI::getRec(){
    return this->tableRect;
}

void UI::Draw(const game& g){
    // Player table
    DrawRectangleLinesEx(tableRect,2.0f, WHITE);
    // Enemy table
    DrawRectangleLinesEx(RecOffsetPos(tableRect,0,-400),2.0f, WHITE);

    DrawTable(g.getTable());
    DrawTable(g.getTableEnemy());

    DrawHand(g.players[0]);
    // debug overlay: show counts
    int active = 0;
    for (size_t i = 0; i < g.players[0].mao.size(); ++i) if (g.players[0].mao[i].isActive()) ++active;
    std::string dbg = "Hand active: " + std::to_string(active) + "  Mesa: " + std::to_string(g.getTable().size());
    DrawText(dbg.c_str(), 10, 10, 12, WHITE);
}