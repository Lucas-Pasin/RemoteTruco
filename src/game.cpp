#include "game.h"

Texture2D cardTextures[40];
Texture2D cardBackTexture;

void game::buildDeck(){
    naipes naipe;
    int score;

    int var=1;
    int d=0;
    // Inicializador baralho
    for(int i=0;i<4;i++){
        naipe = (naipes)i;
        for(int j=0;j<10;j++){
            if(j == 7){
                var = 3;
            }
            score = j+var;
            if(j < 3) score = 12+j+var;
            switch(naipe){
                case DIAMOND:
                    if(j == 6) score = 16;
                break;
                case SPADE:
                    if(j == 6) score = 17;     
                    if(j == 0) score = 19; 
                break;
                case CLUB:
                    if(j == 0) score = 18;        
                break;
                case HEART:
                break;
            }
            deck[j+d] = carta(j+var,naipe,score,&cardTextures[j+d]);
            //printf("Carta %d do naipe %s criada!\n",j+var,naipe == DIAMOND ? "Ouro" : naipe == HEART ? "Copa" : naipe == SPADE ? "Espada" : "Paus");
        }
        d += 10;
        var=1;
    }
}

void game::shuffleDeck() {
    carta temp;
    int r;
    for (int i = 39; i > 0; i--) {
        r = GetRandomValue(0, i);
        temp = deck[i];
        deck[i] = deck[r];
        deck[r] = temp;
    }
}

game::game(){
    players[0] = player("Jogador 1");
    players[1] = player("Jogador 2");
}

void game::StartGame(){
    buildDeck();
    shuffleDeck();
    dealCards();
}

void game::dealCards(){
    for(int i=0;i<2;i++){
        for(int j=0;j<3;j++){
            players[i].mao[j] = deck[i*3 + j];
            players[i].mao[j].setPos(j*100.0f + 260.0f, 680.0f - i*660.0f);
        }
    }
}

void game::Draw(){
    interface.Draw(players[0]);

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
    SelectHandCard();
    
}

// Função para verificar colisão do mouse com as cartas na mão do jogador
void game::SelectHandCard(){
    static bool arrastando = false;
    static int cartaSelecionada = -1;

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
        const Rectangle tem_rec = players[0].mao[cartaSelecionada].getRect();
        players[0].mao[cartaSelecionada].setPos(GetMouseX()- tem_rec.width/2, GetMouseY()-tem_rec.height/2);
    }

    if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
        if(arrastando){
            if(CheckCollisionPointRec(GetMousePosition(), players[0].mao[cartaSelecionada].getRect())){
                mesa[mesacount] = players[0].mao[cartaSelecionada];
                mesacount++;
                players[0].mao[cartaSelecionada].setPos(900.0f,900.0f);
            }
        }
        arrastando = false;
        cartaSelecionada = -1;
    }
}
