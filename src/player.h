#pragma once
#include <string>
#include <vector>
#include "carta.h"
using namespace std;

class player{
    public:
        carta mao[3];

        player();
        player(string nome);
        void Draw() const;

    private:
        string nome;
        int score;
};    