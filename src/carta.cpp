#include "carta.h"
#include <string>


carta::carta()
    : numero(0), naipe(static_cast<naipes>(0)), score(0), cartaRect{0, 0,0 ,0}, textura()
{
    //carregar textura
    
    this->textura = &cardBackTexture;
    cartaRect.height = textura->height;
    cartaRect.width = textura->width;
}

carta::carta(int num, naipes naip, int scor, Texture2D* texture){
    this->numero = num;
    this->naipe = naip;
    this->score = scor;

    //carregar textura
    if(num > 7) num -= 2; //ajustar numero para pegar a imagem correta
    this->textura = texture;
    cartaRect.height = textura->height;
    cartaRect.width = textura->width;
}

carta::~carta(){
    
}

int carta::getNumber(){
    return this->numero;
}

naipes carta::getNaipe(){
    return this->naipe;
}

int carta::getScore(){
    return this->score;
}

void carta::Draw() const{
    DrawTexture(*this->textura,cartaRect.x,cartaRect.y,WHITE);
}

void carta::setPos(float x,float y){
    this->cartaRect.x = x;
    this->cartaRect.y = y;
}

Rectangle carta::getRect(){
    return this->cartaRect;
}
// void carta::setScore(int setscore){
//     this->score = setscore;
// }
