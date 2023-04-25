#include "raylib.h"

#include <math.h>        // Used for sinf()

#define MAX_ENEMIES 10

typedef enum { TITLE, GAMEPLAY, ENDING } GameScreen;

int main()
{
    // INICIALIZAÇAO
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // ABRE JANELA
    InitWindow(screenWidth, screenHeight, "Car Rampage: ROAD RAGE!");

    // ABRE SOM
    InitAudioDevice();

    // CARREGA AS TEXTURAS DO JOGO
    Texture2D cenario = LoadTexture("resources/cenario.png");
    Texture2D title = LoadTexture("resources/titulo.png");
    Texture2D carro = LoadTexture("resources/carro.png");
    Texture2D mad = LoadTexture("resources/mad2.png");
    Texture2D azul = LoadTexture("resources/carroazul.png");
    Texture2D laranja = LoadTexture("resources/carrolaranja.png");
    Texture2D vermelho = LoadTexture("resources/carrovermelho.png");
    Texture2D engrenagem = LoadTexture("resources/engrenagem.png");
    Texture2D gframe = LoadTexture("resources/gframe.png");
    Texture2D botao = LoadTexture("resources/botao.png");
    Texture2D vv = LoadTexture("resources/vv.png");
    Texture2D va = LoadTexture("resources/va.png");
    Texture2D vl = LoadTexture("resources/vl.png");

    // CARREGA A FONTE
    Font font = LoadFont("resources/komika.png");

    // CARREGA OS SONS
    Sound transformacao = LoadSound("resources/transformacao.wav");
    Sound ferramenta = LoadSound("resources/ferramenta.wav");
    Sound boom = LoadSound("resources/explosao.wav");

    // CARREGA MUSICA
    Music music = LoadMusicStream("resources/tokyo2.ogg");
    Music madmodemusic = LoadMusicStream("resources/madmodemusic.ogg");
    
    PlayMusicStream(music);

    // DEFINE AS VARIAVEIS DE ROLAGEM
    int backScrolling = 0;
    int cenarioScrolling = 0;

    // DEFINE A TELA ATUAL
    GameScreen currentScreen = TITLE;

    // DEFINE VARIAVEIS DO JOCADOR
    int playerRail = 1;
    Rectangle playerBounds = { 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
    bool madMode = false;

    // DEFINE VARIAVEIS DOS CARROS INIMIGOS
    Rectangle enemyBounds[MAX_ENEMIES];
    int enemyRail[MAX_ENEMIES];
    int enemyType[MAX_ENEMIES];
    bool enemyActive[MAX_ENEMIES];
    float enemySpeed = 10;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        // PROBABILIDADE DO TIPO DOS INIMIGOS
        int enemyProb = GetRandomValue(0, 100);

        if (enemyProb < 30) enemyType[i] = 0;
        else if (enemyProb < 60) enemyType[i] = 1;
        else if (enemyProb < 90) enemyType[i] = 2;
        else enemyType[i] = 3;

        // DEFINE VETOR DE INIMIGOS
        enemyRail[i] = GetRandomValue(0, 4);

        // CONFERE SE NAO EXITS DOIS INIMIGOS NA MESMA RAIL
        if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

        enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
        enemyActive[i] = false;
    }

    // DEFINE AS OUTRAS VARIAVEIS DO JOGO
    int score = 0;
    float distance = 0.0f;
    int hiscore = 0;
    float hidistance = 0.0f;
    int toolBar = 0;
    int framesCounter = 0;
    float timeCounter = 0;
    int contmusic;

    SetTargetFPS(60);     //DEFINE O FPS DO JOGO

    // LOOP PRINCIPAL DO JOGO
    while (!WindowShouldClose())   
    {
        UpdateMusicStream(music);  
        timeCounter += 0.01;
        framesCounter++;

        // CONTROLADOR DE TELAS DO JOGO
        switch (currentScreen)
        {
            case TITLE:
            {
                // CENARIO COMEÇA A RODAR
                cenarioScrolling -= 2;
                if (cenarioScrolling <= -screenWidth) cenarioScrolling = 0;

                // SE APERTAR ENTER COMEÇA O JOGO 
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                    framesCounter = 0;
                }

            } break;
            case GAMEPLAY:
            {
                // ROLAGEM DO CENÁRIO
                backScrolling--;
                if (backScrolling <= -screenWidth) backScrolling = 0;


                cenarioScrolling -= (enemySpeed - 2);
                if (cenarioScrolling <= -screenWidth) cenarioScrolling = 0;

                // MOVIMENTAÇÃO DO PLAYER, SE APERTAR O SETA ELE MUDA A RUA
                if (IsKeyPressed(KEY_DOWN)) playerRail++;
                else if (IsKeyPressed(KEY_UP)) playerRail--;

                // DELIMITA SE ESTÁ DENTRO DAS 4 RUAS
                if (playerRail > 4) playerRail = 4;
                else if (playerRail < 0) playerRail = 0;

                // ATUALIZA AS FRONTEIRAS DO JOGADOR
                playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };

                // ATIVA MAIS UM INIMIGO A CADA 40 FRAMES
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

                // LOGICA DOS INIMIGOS
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (enemyActive[i])
                    {
                        enemyBounds[i].x -= enemySpeed;
                    }

                    // VERIFICA SE TEM INIMIGOS TAO FORA DA TELA
                    if (enemyBounds[i].x <= 0 - 128)
                    {
                        enemyActive[i] = false;
                        enemyType[i] = GetRandomValue(0, 3);
                        enemyRail[i] = GetRandomValue(0, 4);

                        // VERIFICA SE EXISTE DOIS INIMIGOS CONSECUTIVOS NA MESMA RUA
                        if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

                        enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                    }
                }

                if (!madMode) enemySpeed += 0.005;

                // VERIFICA A COLISÃO DOS CARROS INIMIGOS COM O PRINCIPAL
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (enemyActive[i])
                    {
                        // SE EXITSER COLISAO
                        if (CheckCollisionRecs(playerBounds, enemyBounds[i]))
                        {
                            if (enemyType[i] < 3)
                            {
                                if (madMode)
                                {
                                    if (enemyType[i] == 0) score += 50; // CARRO AZUL
                                    else if (enemyType[i] == 1) score += 150; // CARRO LARANJA
                                    else if (enemyType[i] == 2) score += 300; //CARRO VERMELHO

                                    toolBar += 15;
                                    enemyActive[i] = false;
                                    enemyType[i] = GetRandomValue(0, 3);
                                    enemyRail[i] = GetRandomValue(0, 4);

                                    // VERIFICA SE EXISTE DOIS INIMIGOS CONSECUTIVOS NA MESMA RUA
                                    if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

                                    enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };

                                    PlaySound(boom);
                                }
                                else
                                {
                                    // LOGICA DE MORTE CASO BATA EM OUTRO E CARRO E NAO ESTIVER NO MADMODE
                                    PlaySound(boom);

                                    currentScreen = ENDING;
                                    framesCounter = 0;

                                    // SALVA RECORDE MAXIMO E PONTOS MAXIMO
                                    if (score > hiscore) hiscore = score;
                                    if (distance > hidistance) hidistance = distance + 1;
                                }
                            }
                            else    
                            // COLISAO COM A ENGRANAGEM
                            {
                                enemyActive[i] = false;
                                enemyType[i] = GetRandomValue(0, 3);
                                enemyRail[i] = GetRandomValue(0, 4);

                                // VERIFICA SE EXISTE DOIS INIMIGOS CONSECUTIVOS NA MESMA RUA
                                if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

                                enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                                
                                //AUMENTA A BARRA DE ENGRANAGEM
                                if (!madMode) toolBar += 80;
                                else toolBar += 25;

                                score += 10;
                                //SE ELE PEGAR 5 ENGRANAGENS 80x5 = 400, MADMODE VAI SER VERDADE
                                if (toolBar == 400)
                                {
                                    madMode = true;

                                    PlaySound(transformacao);
                                }

                                PlaySound(ferramenta);
                            }
                        }
                    }
                }

                // LOGICA DO MAD MODE
                if (madMode)
                {
                    //PAUSA MUSICA TOKYO DRIFT
                    PauseMusicStream(music);

                    if(contmusic == 1){
                        ResumeMusicStream(madmodemusic);
                    }else      
                        PlayMusicStream(madmodemusic);

                    UpdateMusicStream(madmodemusic); 
                    // DIMINUI A BARRA DE FERRAMENTAS ENQUANTO ESTIVER NO MADMODE
                    toolBar--;
                    if (toolBar <= 0)
                    {
                        madMode = false;
                        enemySpeed -= 2;
                        if (enemySpeed < 10) enemySpeed = 10;
                    }
                }else{
                    PauseMusicStream(madmodemusic);
                    //VOLTA A TOCAR TOKYO DRIFT
                    ResumeMusicStream(music);
                    int contmusic = 1;
                }

                // ATUALIZA O CONTADOR DE DISTANCIA               
                distance += 0.5f;

            } break;
            case ENDING:
            {
                //SE APERTAR ENTER, VOLTA PARA A TELA DO JOGO E RESETA AS VARIAVEIS ANTIGAS
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;

                    //RESETA O JOGADOR
                    playerRail = 1;
                    playerBounds = (Rectangle){ 30 + 14, playerRail*120 + 90 + 14, 100, 100 };
                    madMode = false;

                    // RESETA OS INIMIGOS
                    for (int i = 0; i < MAX_ENEMIES; i++)
                    {
                        int enemyProb = GetRandomValue(0, 100);

                        if (enemyProb < 30) enemyType[i] = 0;
                        else if (enemyProb < 60) enemyType[i] = 1;
                        else if (enemyProb < 90) enemyType[i] = 2;
                        else enemyType[i] = 3;

                        enemyRail[i] = GetRandomValue(0, 4);

                        // VERIFICA SE EXISTE DOIS INIMIGOS CONSECUTIVOS NA MESMA RUA
                        if (i > 0) while (enemyRail[i] == enemyRail[i - 1]) enemyRail[i] = GetRandomValue(0, 4);

                        enemyBounds[i] = (Rectangle){ screenWidth + 14, 120*enemyRail[i] + 90 + 14, 100, 100 };
                        enemyActive[i] = false;
                    }

                    enemySpeed = 10;

                    // RESETA AS VARIAVEIS DO JOGO
                    score = 0;
                    distance = 0.0;
                    toolBar = 0;
                    framesCounter = 0;
                }

            } break;
            default: break;
        }

        // PARTE DO DESENHOS
        BeginDrawing();

            ClearBackground(RAYWHITE);
            // COLOCA O CENARIO E FAZ ROLAR
            DrawTexture(cenario, cenarioScrolling, 0, WHITE);
            DrawTexture(cenario, screenWidth + cenarioScrolling, 0, WHITE);

            switch (currentScreen)
            {
                case TITLE:
                {
                    //PÓSIÇÃO DO TITULO DO JOGO
                    DrawTexture(title, screenWidth/2 - title.width/2, screenHeight/2 - title.height/2 - 80, WHITE);

                    DrawTexture(botao, screenWidth/2 - 600, screenHeight/2 - 330, WHITE);
                    DrawTextEx(font, "CIMA", (Vector2){ screenWidth/2 - 450, screenHeight/2 - 300}, font.baseSize, 0, YELLOW);
                    DrawTextEx(font, "BAIXO", (Vector2){ screenWidth/2 - 450, screenHeight/2 - 200}, font.baseSize, 0, YELLOW);
                    

                    //MOSTRA A PONTUAÇÃO DE CADA CARRO COM SUA RESPECTIVA MINIATURA
                    DrawTextEx(font, "SCORES:", (Vector2){ screenWidth/2 - 600, screenHeight/2 - 100, }, font.baseSize, 0, WHITE);

                    DrawTexture(vv, screenWidth/2 - 600, screenHeight/2 - 50, WHITE); 
                    DrawTextEx(font, "-300", (Vector2){ screenWidth/2 - 450, screenHeight/2 - 40, }, font.baseSize, 0, RED);

                    DrawTexture(vl, screenWidth/2 - 600, screenHeight/2 + 30, WHITE);
                    DrawTextEx(font, "-150", (Vector2){ screenWidth/2 - 450, screenHeight/2 + 40}, font.baseSize, 0, ORANGE);

                    DrawTexture(va, screenWidth/2 - 600, screenHeight/2 + 110, WHITE);
                    DrawTextEx(font, "-50", (Vector2){ screenWidth/2 - 450, screenHeight/2 + 120}, font.baseSize, 0, BLUE);

                    DrawTexture(engrenagem, screenWidth/2 - 560, screenHeight/2 + 200, WHITE);
                    DrawTextEx(font, "-10", (Vector2){ screenWidth/2 - 450, screenHeight/2 + 210}, font.baseSize, 0, LIGHTGRAY);

                    // APARECE A TECLA ENTER E FICA PISCANDO
                    if ((framesCounter/30) % 2) DrawTextEx(font, "APERTE ENTER", (Vector2){ screenWidth/2 - 150, 600 }, font.baseSize, 0, WHITE);

                } break;
                case GAMEPLAY:
                {

                    // DESENHA O CARRO PRINIPAL
                    if (!madMode) DrawTexture(carro, playerBounds.x - 50, playerBounds.y - 50, WHITE);
                    else DrawTexture(mad, playerBounds.x - 100, playerBounds.y - 40, WHITE);


                    // DESENHA INIMIGOS
                    for (int i = 0; i < MAX_ENEMIES; i++)
                    {
                        if (enemyActive[i])
                        {
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

                    // DESENHA OUTROS ELEMENTOS DO JOGO
                    DrawRectangle(20, 20, 400, 40, Fade(GRAY, 0.4f));
                    DrawRectangle(20, 20, toolBar, 40, ORANGE);
                    DrawRectangleLines(20, 20, 400, 40, BLACK);

                    DrawTextEx(font, TextFormat("PONTOS: %04i", score), (Vector2){ screenWidth - 300, 20 }, font.baseSize, -2, YELLOW);
                    DrawTextEx(font, TextFormat("DISTANCIA: %04i", (int)distance), (Vector2){ 550, 20 }, font.baseSize, -2, YELLOW);

                    if (madMode)
                    {
                        DrawText("MODO MAD MAX", 60, 22, 40, WHITE);
                        DrawTexture(gframe, 0, 0, Fade(WHITE, 0.5f));
                    }

                } break;
                case ENDING:
                {
                    // ESCURE A TELA UM POUCO CASO ESTEJA NA TELA DE GAME OVER
                    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.4f));

                    DrawTextEx(font, "GAME OVER", (Vector2){ 300, 160 }, font.baseSize*3, -2, MAROON);

                    DrawTextEx(font, TextFormat("PONTOS: %04i", score), (Vector2){ 665, 350 }, font.baseSize, -2, YELLOW);
                    DrawTextEx(font, TextFormat("DISTANCIA: %04i", (int)distance), (Vector2){ 290, 350 }, font.baseSize, -2, YELLOW);
                    DrawTextEx(font, TextFormat("RECORDE PONTOS: %04i", hiscore), (Vector2){ 665, 400 }, font.baseSize, -2, YELLOW);
                    DrawTextEx(font, TextFormat("RECORDE DISTANCIA: %04i", (int)hidistance), (Vector2){ 120, 400 }, font.baseSize, -2, YELLOW);

                    // FICA PISCANDO PARA JOGAR DENOVO
                    if ((framesCounter/30) % 2) DrawTextEx(font, "APERTER ENTER PARA JOGAR DENOVO", (Vector2){ screenWidth/2 - 350, 520 }, font.baseSize, -2, LIGHTGRAY);

                } break;
                default: break;
            }

        EndDrawing();
    }

     // DESCARREGA TODOS ELEMENTOS DO JOGO!!
    UnloadTexture(cenario);
    UnloadTexture(gframe);
    UnloadTexture(title);
    UnloadTexture(carro);
    UnloadTexture(azul);
    UnloadTexture(laranja);
    UnloadTexture(vermelho);
    UnloadTexture(engrenagem);
    UnloadTexture(mad);
    UnloadFont(font);
    UnloadSound(ferramenta);
    UnloadSound(boom);
    UnloadSound(transformacao);
    UnloadMusicStream(music);
    UnloadMusicStream(madmodemusic);

    CloseAudioDevice();         

    CloseWindow();             
    return 0;
}
