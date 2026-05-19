#include <raylib.h>
#include "game.h"

enum Scene{
    MENU,
    GAME
};

typedef struct inputBox {
    char text[100];
    int letterCount=0;
    const char *placeholder; 
    Rectangle box;
    bool mouseOnBox = false;

    void checkbox(){
        if (CheckCollisionPointRec(GetMousePosition(), box)) mouseOnBox = true;
        else mouseOnBox = false;
    };

    void update(){
        checkbox();
        if (mouseOnBox)
        {
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (letterCount < 99))
                {
                    text[letterCount] = (char)key;
                    text[letterCount+1] = '\0'; // Add null terminator at the end of the string
                    letterCount++;
                }

                key = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                text[letterCount] = '\0';
            }
        }else SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    };

    void draw(){
        DrawRectangleRec(box, mouseOnBox ? LIGHTGRAY : GRAY);
        DrawText(text, box.x + 5, box.y + 15, 20, BLACK);
        if (text[0] == '\0') DrawText(placeholder, box.x + 5, box.y + 15, 20, DARKGRAY);
    };

}inputBox;

game truco;
Scene currentScene = MENU;

struct Menu{

    network *net = &truco.net;
    inputBox ipBox = { "", 0, "Digite o IP do servidor", { 800/2.0f - 150, 180, 280, 50 }, false };
    inputBox portBox = { "", 0, "Digite o número da porta", { 800/2.0f - 150, 250, 280, 50 }, false };
    Rectangle playButton = { 800/2.0f - 75, 320, 150, 50 };

    //métodos do menu
    void update(){
        ipBox.update();
        portBox.update();

        // Verifica se a conexão assíncrona completou
        net->checkConnectionStatus();

        if(net->isConnected()){
            currentScene = GAME; // mudar para a cena de jogo
            
            return;
        }

        if(CheckCollisionPointRec(GetMousePosition(), playButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            // Tentar conectar ao servidor usando os valores dos input boxes
            string ip = ipBox.text;
            int port = atoi(portBox.text);
            if (port == 0) port = LISTEN_PORT; // fallback para porta padrão
            net->connectToServer(ip, port);
        }
    };
    
    void draw(){
        DrawText("Truco Online", 800/2 - MeasureText("Truco Online", 40)/2 - 10, 100, 40, BLACK);
        ipBox.draw();
        portBox.draw();
        DrawRectangleRec(playButton, GREEN);
        if(net->isConnected()){
            DrawRectangleRec(playButton, GRAY);
        }else DrawText("Jogar", playButton.x + 25, playButton.y + 15, 20, BLACK);
    };
};

Menu menu;
void UpdateScene(Scene scene);
void DrawScene(Scene scene);

int main() 
{

    Color green = {20,160,133,255};
    InitWindow(800, 800, "Truco Online");
    SetTargetFPS(60);
    
    LoadAllCardTextures();
    truco.StartGame();

    

    // Game loop
    while (!WindowShouldClose())
    {

        //1. Update
        UpdateScene(currentScene);


        //2. Draw
        BeginDrawing();
            ClearBackground(green);
            DrawScene(currentScene);
        EndDrawing();
    }

    UnloadAllCardTextures();
    CloseWindow();
    return 0;
}

void UpdateScene(Scene scene) {
    switch (scene) {
        case MENU:
            menu.update();
            break;
        case GAME:
            truco.Update();
            break;
    }
}

void DrawScene(Scene scene) {
    switch (scene) {
        case MENU:
            // Desenhar tela de menu
            menu.draw();
            break;
        case GAME:
            // Desenhar tela de jogo
            truco.Draw();
            break;
    }

}



