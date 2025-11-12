#include "game.h"

Texture2D cardTextures[40];
Texture2D cardBackTexture;

game::game(){
    players[0] = player("Jogador 1");
    players[1] = player("Jogador 2");
}

void game::StartGame(){
    net.startThread(*this);
}


const vector<carta> &game::getTable() const{
    return mesa;
}

void game::Draw(){
    interface.Draw(*this);
}

// Função de inicialização das texturas globais
void LoadAllCardTextures() {
    // Carrega a textura da parte de trás da carta
    cardBackTexture = LoadTexture("resources/spanish_deck/back.PNG");

    // Carrega todas as texturas de frente das cartas
    for (int i = 0; i < 40; i++) {
        std::string path = "resources/spanish_deck/" + std::to_string(i+1) + ".PNG";
        Image image = LoadImage(path.c_str());
        cardTextures[i] = LoadTextureFromImage(image);
        UnloadImage(image);
    }
}

// No final do jogo, liberar as texturas globais
void UnloadAllCardTextures() {
    UnloadTexture(cardBackTexture);
    for (int i = 0; i < 40; i++) {
        UnloadTexture(cardTextures[i]);
    }
}

game::~game(){}

void game::Update(){

    switch ((gamestate)PacoteAtual.state){
        case PLAY:
            SelectHandCard();
            break;
        case WAIT:
            
            break;
        case ROUND_START:
            
            break;
        default:
            break;
    }
    
}

// Função para selecionar e arrastar cartas da mão do jogador para a mesa
void game::SelectHandCard(){
    static bool arrastando = false;
    static int cartaSelecionada = -1;
    const Rectangle table_rect = interface.getRec();

    if(!arrastando && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        for(int i=0;i<3;i++){
            if(CheckCollisionPointRec(GetMousePosition(), players[0].mao[i].getRect())){
                arrastando = true;
                cartaSelecionada = i;
                break;
            }
        }
    }
    
    if(arrastando && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        Rectangle tem_rec = players[0].mao[cartaSelecionada].getRect();
        players[0].mao[cartaSelecionada].setPos(GetMouseX() - tem_rec.width/2, 
                                                GetMouseY()-tem_rec.height/2);
    }

    if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
        if(arrastando){
            if(CheckCollisionPointRec(GetMousePosition(),table_rect)){
                players[0].mao[cartaSelecionada].setPos(65 + table_rect.x +100*mesa.size(),
                                                             table_rect.y + 18.3f);
                mesa.push_back(players[0].mao[cartaSelecionada]);
                players[0].mao[cartaSelecionada].setPos(900.0f,900.0f);
            }else{
                // Retorna a carta para a posição original se não for colocada na mesa
                players[0].mao[cartaSelecionada].setPos(cartaSelecionada*100.0f + 260.0f, 680.0f);
            }
        }
        arrastando = false;
        cartaSelecionada = -1;
    }
}
