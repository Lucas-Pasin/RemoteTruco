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

void UI::DrawGamestate(gamestate state, float centerX, float centerY){
    switch(state){
        case PLAY:
            DrawText("Your turn to play!", centerX, centerY, 20, GREEN);
            break;
        case WAIT:
            DrawText("Waiting for opponent...", centerX, centerY, 20, YELLOW);
            break;
        case TRUCO:
            DrawText("TRUCO called!", centerX, centerY, 20, ORANGE);
            break;
        case RETRUCO:
            DrawText("RETRUCO called!", centerX, centerY, 20, PURPLE);
            break;
        case ROUND_START:
            DrawText("New Round Starting!", centerX, centerY, 20, BLUE);
            break;
        case WIN:
            DrawText("You Win!", centerX, centerY, 20, GOLD);
            break;
        case LOSE:
            DrawText("You Lose!", centerX, centerY, 20, RED);
            break;
        default:
            break;
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

Rectangle UI::getTrucoButtonRect() const {
    return this->trucoButtonRect;
}

Rectangle UI::getAcceptButtonRect() const {
    return this->acceptButtonRect;
}

Rectangle UI::getRejectButtonRect() const {
    return this->rejectButtonRect;
}

void UI::DrawTrucoButton(gamestate state, Rectangle buttonRect){
    // Desenha o botão com bordas
    DrawRectangleRec(buttonRect, DARKGRAY);
    DrawRectangleLinesEx(buttonRect, 2.0f, WHITE);
    
    // Determina o texto baseado no gamestate
    const char* buttonText = "";
    Color textColor = WHITE;
    
    switch(state){
        case TRUCO:
            buttonText = "TRUCO!";
            textColor = RED;
            break;
        case RETRUCO:
            buttonText = "RETRUCO!";
            textColor = ORANGE;
            break;
        case PLAY:
            buttonText = "TRUCO";
            textColor = WHITE;
            break;
        default:
            buttonText = "";
            break;
    }
    
    // Calcula posição do texto centralizado no botão
    int textWidth = MeasureText(buttonText, 16);
    float textX = buttonRect.x + (buttonRect.width - textWidth) / 2.0f;
    float textY = buttonRect.y + (buttonRect.height - 16) / 2.0f;
    
    // Desenha o texto
    if (buttonText[0] != '\0') {
        DrawText(buttonText, textX, textY, 16, textColor);
    }
}

void UI::DrawTrucoResponseButtons(gamestate state){
    // Só mostra os botões se estado é TRUCO ou RETRUCO
    if (state != TRUCO && state != RETRUCO) {
        return;
    }
    
    // Desenha botão "Aceito"
    DrawRectangleRec(acceptButtonRect, DARKGREEN);
    DrawRectangleLinesEx(acceptButtonRect, 2.0f, WHITE);
    
    int acceptWidth = MeasureText("Aceito", 14);
    float acceptX = acceptButtonRect.x + (acceptButtonRect.width - acceptWidth) / 2.0f;
    float acceptY = acceptButtonRect.y + (acceptButtonRect.height - 14) / 2.0f;
    DrawText("Aceito", acceptX, acceptY, 14, WHITE);
    
    // Desenha botão "Não Aceito"
    DrawRectangleRec(rejectButtonRect, MAROON);
    DrawRectangleLinesEx(rejectButtonRect, 2.0f, WHITE);
    
    int rejectWidth = MeasureText("Não Aceito", 14);
    float rejectX = rejectButtonRect.x + (rejectButtonRect.width - rejectWidth) / 2.0f;
    float rejectY = rejectButtonRect.y + (rejectButtonRect.height - 14) / 2.0f;
    DrawText("Não Aceito", rejectX, rejectY, 14, WHITE);
}

void UI::Draw(const game& g){
    // Player table
    DrawRectangleLinesEx(tableRect,2.0f, WHITE);
    // Enemy table
    DrawRectangleLinesEx(RecOffsetPos(tableRect,0,-400),2.0f, WHITE);

    DrawTable(g.getTable());
    DrawTable(g.getTableEnemy());

    DrawHand(g.players[0]);

    DrawGamestate(g.estado);
    DrawTrucoButton(g.estado, trucoButtonRect);
    DrawTrucoResponseButtons(g.estado);

    // debug overlay: show counts
    int active = 0;
    for (size_t i = 0; i < g.players[0].mao.size(); ++i) if (g.players[0].mao[i].isActive()) ++active;
    std::string dbg = "Hand active: " + std::to_string(active) + "  Mesa: " + std::to_string(g.getTable().size());
    DrawText(dbg.c_str(), 10, 10, 12, WHITE);
}