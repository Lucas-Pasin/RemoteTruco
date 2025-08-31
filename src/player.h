#pragma once
#include <string>
using namespace std;

class player{
    public:
        carta mao[3];

        player();
        player(string nome);
        void Draw();

    private:
        string nome;
        int score;
};    