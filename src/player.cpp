#include "player.h"

player::player() {
    this->nome = "John Doe";
    this->score = 0;
    for(int i=0;i<3;i++){
        this->mao[i].setPos(i*100.0f + 120.0f, 600.0f);
        this->mao[i].setActive(false);
    }
    
}

player::player(string nome){
    this->nome = nome;
    this->score = 0;
    for (int i = 0; i < 3; i++) {
        this->mao[i] = carta();
        this->mao[i].setPos(i*100.0f + 120.0f, 600.0f);
        this->mao[i].setActive(false);
    }
}

// void player::Draw(){
//     for(int i=0;i<3;i++){
//         mao[i].Draw();
//     }
//     DrawRectangleLines(100,300,600,200,WHITE);
// }
