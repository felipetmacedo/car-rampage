#include "raylib.h"

#include <math.h>        // Used for sinf()

#define MAX_ENEMIES 10

typedef enum { TITLE, GAMEPLAY, ENDING } GameScreen;

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Init window
    InitWindow(screenWidth, screenHeight, "Car Rampage: ROAD RAGE!");

    // Initialize audio device
    InitAudioDevice();

    // Load game resources: textures
    Texture2D cenario = LoadTexture("resources/cenario.png");
    Texture2D title = LoadTexture("resources/titulo.png");
    Texture2D carro = LoadTexture("resources/carro.png");
    Texture2D mad = LoadTexture("resources/mad2.png");
    Texture2D azul = LoadTexture("resources/carroazul.png");
    Texture2D laranja = LoadTexture("resources/carrolaranja.png");
    Texture2D vermelho = LoadTexture("resources/carrovermelho.png");
    Texture2D engrenagem = LoadTexture("resources/engrenagem.png");
    Texture2D gframe = LoadTexture("resources/gframe.png");
    Texture2D explosao = LoadTexture("resources/explosao.png");

    // CARREGA A FONTE
    Font font = LoadFont("resources/komika.png");

    // CARREGA OS SONS
    Sound transformacao = LoadSound("resources/transformacao.wav");
    Sound ferramenta = LoadSound("resources/ferramenta.wav");
    Sound boom = LoadSound("resources/explosao.wav");

    // CARREGA MUSICA
    Music music = LoadMusicStream("resources/tokyo2.ogg");
    PlayMusicStream(music);

    // Define scrolling variables
    int backScrolling = 0;
    int cenarioScrolling = 0;

    // Define current screen
    GameScreen currentScreen = TITLE;

    // Define player variables
    int playerRail = 1;
    Rectangle playerBounds = { 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
    bool madMode = false;

    // Define enemies variables
    Rectangle enemyBounds[MAX_ENEMIES];
    int enemyRail[MAX_ENEMIES];
    int enemyType[MAX_ENEMIES];
    bool enemyActive[MAX_ENEMIES];
    float enemySpeed = 10;

    // Init enemies variables
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        // Define enemy type (all same probability)
        //enemyType[i] = GetRandomValue(0, 3);

        // Probability system for enemies type
        int enemyProb = GetRandomValue(0, 100);

        if (enemyProb < 30) enemyType[i] = 0;
        else if (enemyProb < 60) enemyType[i] = 1;
        else if (enemyProb < 90) enemyType[i] = 2;
        else enemyType[i] = 3;

        // define enemy rail
        enemyRail[i] = GetRandomValue(0, 4);

        // Make sure not two consecutive enemies in the same row
        if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

        enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
        enemyActive[i] = false;
    }

    // Define additional game variables
    int score = 0;
    float distance = 0.0f;
    int hiscore = 0;
    float hidistance = 0.0f;
    int foodBar = 0;
    int framesCounter = 0;
    float timeCounter = 0;

    SetTargetFPS(60);       // Setup game frames per second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateMusicStream(music);   // Refill music stream buffers (if required)

        timeCounter += 0.01;
        framesCounter++;

        // Game screens management
        switch (currentScreen)
        {
            case TITLE:
            {
                // Sea scrolling
                cenarioScrolling -= 2;
                if (cenarioScrolling <= -screenWidth) cenarioScrolling = 0;

                // Press enter to change to gameplay screen
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                    framesCounter = 0;
                }

            } break;
            case GAMEPLAY:
            {
                // Background scrolling logic
                backScrolling--;
                if (backScrolling <= -screenWidth) backScrolling = 0;

                // Sea scrolling logic
                cenarioScrolling -= (enemySpeed - 2);
                if (cenarioScrolling <= -screenWidth) cenarioScrolling = 0;

                // Player movement logic
                if (IsKeyPressed(KEY_DOWN)) playerRail++;
                else if (IsKeyPressed(KEY_UP)) playerRail--;

                // Check player not out of rails
                if (playerRail > 4) playerRail = 4;
                else if (playerRail < 0) playerRail = 0;

                // Update player bounds
                playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };

                // Enemies activation logic (every 40 frames)
                if (framesCounter > 40)
                {
                    for (int i = 0; i < MAX_ENEMIES; i++)
                    {
                        if (enemyActive[i] == false)
                        {
                            enemyActive[i] = true;
                            i = MAX_ENEMIES;
                        }
                    }

                    framesCounter = 0;
                }

                // Enemies logic
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (enemyActive[i])
                    {
                        enemyBounds[i].x -= enemySpeed;
                    }

                    // Check enemies out of screen
                    if (enemyBounds[i].x <= 0 - 128)
                    {
                        enemyActive[i] = false;
                        enemyType[i] = GetRandomValue(0, 3);
                        enemyRail[i] = GetRandomValue(0, 4);

                        // Make sure not two consecutive enemies in the same row
                        if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

                        enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                    }
                }

                if (!madMode) enemySpeed += 0.005;

                // Check collision player vs enemies
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (enemyActive[i])
                    {
                        if (CheckCollisionRecs(playerBounds, enemyBounds[i]))
                        {
                            if (enemyType[i] < 3)   // Bad enemies
                            {
                                if (madMode)
                                {
                                    if (enemyType[i] == 0) score += 50;
                                    else if (enemyType[i] == 1) score += 150;
                                    else if (enemyType[i] == 2) score += 300;

                                    foodBar += 15;

                                    enemyActive[i] = false;

                                    // After enemy deactivation, reset enemy parameters to be reused
                                    enemyType[i] = GetRandomValue(0, 3);
                                    enemyRail[i] = GetRandomValue(0, 4);

                                    // Make sure not two consecutive enemies in the same row
                                    if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

                                    enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };

                                    PlaySound(boom);
                                }
                                else
                                {
                                    // Player die logic
                                    PlaySound(boom);

                                    currentScreen = ENDING;
                                    framesCounter = 0;

                                    // Save hiscore and hidistance for next game
                                    if (score > hiscore) hiscore = score;
                                    if (distance > hidistance) hidistance = distance;
                                }
                            }
                            else    // Sweet fish
                            {
                                enemyActive[i] = false;
                                enemyType[i] = GetRandomValue(0, 3);
                                enemyRail[i] = GetRandomValue(0, 4);

                                // Make sure not two consecutive enemies in the same row
                                if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

                                enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };

                                if (!madMode) foodBar += 80;
                                else foodBar += 25;

                                score += 10;

                                if (foodBar == 400)
                                {
                                    madMode = true;

                                    PlaySound(transformacao);
                                }

                                PlaySound(ferramenta);
                            }
                        }
                    }
                }

                // Gamera mode logic
                if (madMode)
                {
                    foodBar--;
                    if (foodBar <= 0)
                    {
                        madMode = false;
                        enemySpeed -= 2;
                        if (enemySpeed < 10) enemySpeed = 10;
                    }
                }

                // Update distance counter                
                distance += 0.5f;

            } break;
            case ENDING:
            {
                // Press enter to play again
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;

                    // Reset player
                    playerRail = 1;
                    playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
                    madMode = false;

                    // Reset enemies data
                    for (int i = 0; i < MAX_ENEMIES; i++)
                    {
                        int enemyProb = GetRandomValue(0, 100);

                        if (enemyProb < 30) enemyType[i] = 0;
                        else if (enemyProb < 60) enemyType[i] = 1;
                        else if (enemyProb < 90) enemyType[i] = 2;
                        else enemyType[i] = 3;

                        //enemyType[i] = GetRandomValue(0, 3);
                        enemyRail[i] = GetRandomValue(0, 4);

                        // Make sure not two consecutive enemies in the same row
                        if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

                        enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                        enemyActive[i] = false;
                    }

                    enemySpeed = 10;

                    // Reset game variables
                    score = 0;
                    distance = 0.0;
                    foodBar = 0;
                    framesCounter = 0;
                }

            } break;
            default: break;
        }

        // DESENHO
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTexture(cenario, cenarioScrolling, 0, WHITE);
            DrawTexture(cenario, screenWidth + cenarioScrolling, 0, WHITE);

            switch (currentScreen)
            {
                case TITLE:
                {
                    // Draw title
                    DrawTexture(title, screenWidth/2 - title.width/2, screenHeight/2 - title.height/2 - 80, WHITE);

                    // Draw blinking text
                    if ((framesCounter/30) % 2) DrawTextEx(font, "APERTE ENTER", (Vector2){ screenWidth/2 - 150, 600 }, font.baseSize, 0, WHITE);

                } break;
                case GAMEPLAY:
                {

                    // Draw player
                    if (!madMode) DrawTexture(carro, playerBounds.x - 50, playerBounds.y - 50, WHITE);
                    else DrawTexture(mad, playerBounds.x - 100, playerBounds.y - 40, WHITE);


                    // Draw enemies
                    for (int i = 0; i < MAX_ENEMIES; i++)
                    {
                        if (enemyActive[i])
                        {
                            // Draw enemies
                            switch(enemyType[i])
                            {
                                case 0: DrawTexture(azul, enemyBounds[i].x, enemyBounds[i].y, WHITE); break;
                                case 1: DrawTexture(laranja, enemyBounds[i].x, enemyBounds[i].y, WHITE); break;
                                case 2: DrawTexture(vermelho, enemyBounds[i].x, enemyBounds[i].y, WHITE); break;
                                case 3: DrawTexture(engrenagem, enemyBounds[i].x + 40, enemyBounds[i].y + 40, WHITE); break;
                                default: break;
                            }
                        }
                    }

                    // Draw gameplay interface
                    DrawRectangle(20, 20, 400, 40, Fade(GRAY, 0.4f));
                    DrawRectangle(20, 20, foodBar, 40, ORANGE);
                    DrawRectangleLines(20, 20, 400, 40, BLACK);

                    DrawTextEx(font, TextFormat("POINTS: %04i", score), (Vector2){ screenWidth - 300, 20 }, font.baseSize, -2, YELLOW);
                    DrawTextEx(font, TextFormat("DISTANCE: %04i", (int)distance), (Vector2){ 550, 20 }, font.baseSize, -2, YELLOW);

                    if (madMode)
                    {
                        DrawText("MODO MAD MAX", 60, 22, 40, WHITE);
                        DrawTexture(gframe, 0, 0, Fade(WHITE, 0.5f));
                    }

                } break;
                case ENDING:
                {
                    // Draw a transparent black rectangle that covers all screen
                    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.4f));

                    DrawTextEx(font, "GAME OVER", (Vector2){ 300, 160 }, font.baseSize*3, -2, MAROON);

                    DrawTextEx(font, TextFormat("POINTS: %04i", score), (Vector2){ 680, 350 }, font.baseSize, -2, YELLOW);
                    DrawTextEx(font, TextFormat("DISTANCE: %04i", (int)distance), (Vector2){ 290, 350 }, font.baseSize, -2, YELLOW);
                    DrawTextEx(font, TextFormat("HIGHSCORE: %04i", hiscore), (Vector2){ 665, 400 }, font.baseSize, -2, YELLOW);
                    DrawTextEx(font, TextFormat("HIGHDISTANCE: %04i", (int)hidistance), (Vector2){ 270, 400 }, font.baseSize, -2, YELLOW);

                    // Draw blinking text
                    if ((framesCounter/30) % 2) DrawTextEx(font, "PRESS ENTER TO REPLAY", (Vector2){ screenWidth/2 - 250, 520 }, font.baseSize, -2, LIGHTGRAY);

                } break;
                default: break;
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    // Unload textures
    UnloadTexture(cenario);
    UnloadTexture(gframe);
    UnloadTexture(title);
    UnloadTexture(carro);
    UnloadTexture(azul);
    UnloadTexture(laranja);
    UnloadTexture(vermelho);
    UnloadTexture(engrenagem);
    UnloadTexture(mad);

    // Unload font texture
    UnloadFont(font);

    // Unload sounds
    UnloadSound(ferramenta);
    UnloadSound(boom);
    UnloadSound(transformacao);

    UnloadMusicStream(music);   // Unload music
    CloseAudioDevice();         // Close audio device

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
