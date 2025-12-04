#include "carta.h"
#include <string>


carta::carta()
    : numero(0), naipe(static_cast<naipes>(0)), score(0), cartaRect{0, 0,0 ,0}, textura()
{
    //carregar textura
    this->textura = &cardBackTexture;
    // don't rely on texture being loaded yet; use a sensible default size
    cartaRect.width = 70.0f;
    cartaRect.height = 90.0f;
    this->active = false; // default empty card
}

carta::carta(int num, naipes naip, int scor, Texture2D* texture){
    this->numero = num;
    this->naipe = naip;
    this->score = scor;


    this->textura = texture;
    if (this->textura && this->textura->height > 0) {
        cartaRect.height = this->textura->height;
    }
    if (this->textura && this->textura->width > 0) {
        cartaRect.width = this->textura->width;
    }
    this->active = true;
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

void carta::Draw(bool figShow) const{
    if(!this->active) return; // don't draw empty slots

    if(figShow){
        DrawTexture(*this->textura, cartaRect.x, cartaRect.y, WHITE);
        return;
    }
    DrawTexture(cardBackTexture, cartaRect.x, cartaRect.y, WHITE);
}

void carta::cartaToCard(card cartinha){
    this->numero = cartinha.numero;
    this->naipe = cartinha.naipe;
    this->score = cartinha.score;
    int num = this->numero;
    if(num > 7) num -= 2; //ajustar numero para pegar a imagem correta
    int index = num - 1 + (int)naipe * 10;
    if (index >= 0 && index < 40) {
        //printf("Assigning texture index [%d] for card [numero: %d naipe: %d\n]", index, this->numero, (int)this->naipe);
        
        this->textura = &cardTextures[index];
        
        this->cartaRect.width = this->textura->width;
        this->cartaRect.height = this->textura->height;
    }
    this->active = true;
}

void carta::setActive(bool a){
    this->active = a;
}

bool carta::isActive() const {
    return this->active;
}

card carta::cardGet(){
    card cartinha;
    cartinha.numero = numero;
    cartinha.naipe  = naipe;
    cartinha.score  = score;
    
    return cartinha;
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
