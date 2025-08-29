#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "raylib.h"

typedef enum naipe{
    SPADE,
    HEART,
    DIAMOND,
    CLUB
} naipes;

class carta{
    protected:
        int numero;
        naipes naipe;

    public:
        carta() : numero(0), naipe(HEART){}

        carta(int num,naipes naip){
            this->numero = num;
            this->naipe = naip;
        }

        int getNumber(){
            return this->numero;
        }

        naipes getNaipe(){
            return this->naipe;
        }

        virtual int getScore(){
            if(numero < 4){
                return (this->numero + 13);
            }
            return this->numero;
        }
};

class manilha: public carta{
    public:
        using carta :: carta;
        int getScore(){
            switch(numero){
                case 1:
                    if(naipe == SPADE){
                        return 20;
                    }
                    return 19;
                break;
                case 7:
                    if(naipe == SPADE){
                        return 18;
                    }
                    return 17;
                break;
            }
            return 0;
        }
};



int main(){


    carta *baralho[40];
    
    naipes naipes1[4] = {SPADE,HEART,DIAMOND,CLUB};
    int var=1;
    int d=0;

    // Inicializador baralho
    for(int i=0;i<4;i++){
        for(int j=0;j<10;j++){
            if(j == 7){
                var= 3;
            }
            baralho[j+d] = new carta(j+var,naipes1[i]);
        }
        d += 10;
        var=1;
    }

    // ------- ADD MANILHAS --------- 
    // Implementação horrivel kkk
    
    delete baralho[0];
    delete baralho[30];
    delete baralho[6];
    delete baralho[36];
    
    baralho[0] = new manilha(1,naipes1[0]);
    baralho[30] = new manilha(1,naipes1[3]);
    baralho[6] = new manilha(7,naipes1[0]);
    baralho[36] = new manilha(7,naipes1[2]);


    
    for(int j=0;j<40;j++){
        printf("baralho[%d]= %d %d\n",j,baralho[j]->getNumber(),baralho[j]->getScore());
    }

    
    return 0;

}