#include <raylib.h>
#include "carta.h"
#include "game.h"



int main() 
{
    Color green = {20,160,133,255};
    InitWindow(800, 800, "Truco Online");
    SetTargetFPS(60);

    LoadAllCardTextures();
    game truco = game();
    truco.StartGame();
    

    // Game loop
    while (!WindowShouldClose())
    {
        //1. Event Handling

        //2. Update
        truco.Update();

        //3. Draw
        BeginDrawing();
            ClearBackground(green);
            truco.Draw();
        EndDrawing();
    }

    UnloadAllCardTextures();
    CloseWindow();
    return 0;
}