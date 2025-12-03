#include "player.h"

player::player() {
    this->nome = "John Doe";
    this->score = 0;
    // start with an empty hand; ApplyPacoteToHands will populate
    this->mao.clear();
    
}

player::player(string nome){
    this->nome = nome;
    this->score = 0;
    this->mao.clear();
}

// void player::Draw(){
//     for(int i=0;i<3;i++){
//         mao[i].Draw();
//     }
//     DrawRectangleLines(100,300,600,200,WHITE);
// }
