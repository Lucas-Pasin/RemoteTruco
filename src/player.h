#pragma once
#include <string>
#include <vector>
#include "carta.h"
using namespace std;

class player{
    public:
        std::vector<carta> mao;

        player();
        player(string nome);
        void Draw() const;

    private:
        string nome;
        int score;
};    