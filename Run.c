#include "raylib.h"
#include "stdlib.h"

struct Tela{bool menu;bool gameplay;bool gameover;bool pause;};

void trocarEnemy (int *inimigo, float *y, float *width, float *height, int *velEnemy){
    int tipo = GetRandomValue(0, 2);
    *inimigo = tipo;
    // Cerca
    if (tipo == 0){
        *y = GetScreenHeight()-240;
        *width = 50;
        *height = 455;
        *velEnemy = GetRandomValue(30, 40);
    }    
    // Arvore
    else if (tipo == 1){
        *y = GetScreenHeight()-300;
        *width = 150;
        *height = 400;
        *velEnemy = GetRandomValue(30, 50);
    }    
    // Passaro
    else if (tipo == 2){
        *y = GetScreenHeight()-330;
        *width = 200;
        *height = 100;
        *velEnemy = GetRandomValue(30, 40);      
    }
    
}

// Função do Menu 
void menu(bool *menuT, bool *gameplayT){
    
    Music music = LoadMusicStream("musicas/menu_do_jogo_oh_my_goose.wav");
    PlayMusicStream(music);
    Image image = LoadImage("Desenhos/MENU_INICIAL.png");  // Load image data 
    Texture2D texture = LoadTextureFromImage(image);       // Image converted to texture  
    SetTargetFPS(60);
    
    while(!WindowShouldClose()){
        
        UpdateMusicStream(music);
        
        // Começa o jogo
        if (IsKeyPressed(KEY_ENTER)){
            *menuT = false;
            *gameplayT = true;
            return;
        }
        
        // --------Draw ---------
            BeginDrawing();
                
                ClearBackground(WHITE);
                // Coloca a tela de fundo
                DrawTexture(texture, 0, 0, WHITE);
                DrawText("Aperte ENTER para\n\ncomeçar o jogo",GetScreenWidth()/2-530,GetScreenHeight()/2+300,30,BLUE);

            EndDrawing();
        }
        UnloadImage(image);
        CloseAudioDevice();
        CloseWindow();
}

// Função do Jogo rodando 
void gaming(bool *menuT, bool *gameplayT, bool *pauseT, bool *gameoverT){
    
    
    Music music = LoadMusicStream("musicas/jogo_oh_my_goose.wav");
    PlayMusicStream(music);
    
    // Initialization
    int animFrames = 0;
    int animGanso = 0;
    //carrega gif em imagem 
    Image imRun = LoadImageAnim("Gifs/GifRUN.gif", &animFrames);
    Image imJump = LoadImageAnim("Gifs/GifJUMP.gif", &animFrames);
    Image imSlide = LoadImageAnim("Gifs/GifSlide.gif", &animFrames);
    Image imGanso = LoadImageAnim("Gifs/Ganso.gif", &animGanso);
    
    //texturiza essa imagem
    Texture2D texRun = LoadTextureFromImage(imRun);
    Texture2D texJump = LoadTextureFromImage(imJump);
    Texture2D texSlide = LoadTextureFromImage(imSlide);
    Texture2D texGanso = LoadTextureFromImage(imGanso);
    Texture2D cerca = LoadTexture("Sprites/Cerca renderizada.png");
    Texture2D arvore = LoadTexture("Sprites/Arbusto renderizado.png");
    Texture2D passaro = LoadTexture("Sprites/Passaro renderizado.png");
    Texture2D background = LoadTexture("Desenhos/BACKGROUND.png");
    Texture2D texAtual = texRun;

    unsigned int nextFrameDataOffset = 0;  // Current byte offset to next frame in image.data
    unsigned int nextFrameDataOffsetP = 0; // do Pato

    int currentAnimFrame = 0;       // Frame atual
    int currentAnimFrameP = 0;      // do Pato
    int enemyDelay = 2;              // Delay do inigo
    int velEnemy = GetRandomValue(30, 40); // Velocidade do inimigo
    int enemy = 0;                  // Tipo de inimigos
    int frameDelay = 3;             // Frame delay to switch between animation frames
    int frameCounter = 0;           // General frames counter
    int valueJ = 35;                // Valor do pulo
    bool jump = false;              // Verifica se está pulando
    bool slide = false;             // Verificar se está deslizando
    bool colisao = false;           // Verifciar a colisao
    bool pause = false;
    int posX = 50;                  // Posição X do jogador
    int posY = GetScreenHeight()/2; // Posição Y do jogador
    int posEx = GetScreenWidth();
    int posEy = GetScreenHeight()-240;
    int delay = 0;
    float scrolling = 0.0f;
    Rectangle colisionP = {(float)posX+100,(float)posY,imRun.width-180,imRun.height};
    Rectangle colisionE = {(float)posEx,(float)posEy,50,455};

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    while (!WindowShouldClose())    // Detectar o ESC sai do jogo
    {
        // Quando apertar P pausa
        if (IsKeyPressed(KEY_P)){
            pause = !pause;
            if (pause) PauseMusicStream(music);
            else ResumeMusicStream(music);
        }
        
        //Se não pusar roda o jogo
        if (!pause){
            // Update
            UpdateMusicStream(music);
            frameCounter++;
            scrolling -= 5.0f;
            
            // Paralax de fundo
            if (scrolling <= -background.width) scrolling = 0;
            
            // Animação do ganso
            if (frameCounter >= frameDelay-2)
            {
                // Move to next frame
                currentAnimFrameP++;
                if (currentAnimFrameP >= animGanso) currentAnimFrameP = 0;

                // Get memory offset position for next frame data in image.data
                nextFrameDataOffsetP = imGanso.width*imGanso.height*4*currentAnimFrameP;

                // Update GPU texture data with next frame image data
                UpdateTexture(texGanso, ((unsigned char *)imGanso.data) + nextFrameDataOffsetP);        
            }
            
            //Animação de correr
            if (frameCounter >= frameDelay && !jump && !slide)
            {
                // Move to next frame
                currentAnimFrame++;
                if (currentAnimFrame >= 20) currentAnimFrame = 0;

                // Get memory offset position for next frame data in image.data
                nextFrameDataOffset = imRun.width*imRun.height*4*currentAnimFrame;

                // Update GPU texture data with next frame image data
                UpdateTexture(texRun, ((unsigned char *)imRun.data) + nextFrameDataOffset);

                frameCounter = 0;          
            }
            
            //Animação de deslizar
            if (frameCounter >= frameDelay && slide && !jump)
            {
                // Move to next frame
                currentAnimFrame++;
                if (currentAnimFrame >= animFrames) currentAnimFrame = 0;
                
                // Get memory offset position for next frame data in image.data
                nextFrameDataOffset = imSlide.width*imSlide.height*4*currentAnimFrame;

                // Update GPU texture data with next frame image data
                UpdateTexture(texSlide, ((unsigned char *)imSlide.data) + nextFrameDataOffset);         
                            
                // Quando termianr o gif começa a correr
                if (currentAnimFrame>=24){
                    texAtual = texRun;
                    currentAnimFrame = 0;
                    colisionP.y = GetScreenHeight()/2;
                    slide = false;
                }
                frameCounter = 0;
            }
            
            //Animação do pulo
            if (frameCounter >= frameDelay && jump && !slide){
                // Move to next frame
                currentAnimFrame++;
                if (currentAnimFrame >= animFrames) currentAnimFrame = 0;

                // Get memory offset position for next frame data in image.data
                nextFrameDataOffset = imJump.width*imJump.height*4*currentAnimFrame;

                // Update GPU texture data with next frame image data
                UpdateTexture(texJump, ((unsigned char *)imJump.data) + nextFrameDataOffset);
                
                // Sobe e desce
                if (currentAnimFrame>0 && currentAnimFrame<=10) colisionP.y-=valueJ;
                else if (currentAnimFrame>10) colisionP.y+=valueJ;
                
                //Quando terminar o gif começa a correr
                if (currentAnimFrame>=20){
                    texAtual = texRun;
                    currentAnimFrame = 0;
                    colisionP.height = imRun.height;          
                    jump = false;
                }   
                frameCounter = 0;
            }
            
            // Movimento do obstaculo: cerca
            if (frameCounter >= enemyDelay && enemy == 0) {
                colisionE.x-=velEnemy;
                if (colisionE.x+delay<=0) {
                    delay = GetRandomValue(100, 300);
                    trocarEnemy(&enemy, &colisionE.y, &colisionE.width, &colisionE.height, &velEnemy);
                    colisionE.x = GetScreenWidth();
                }
            }
            
            // Movimento do obstaculo: arvore
            else if (frameCounter >= enemyDelay && enemy == 1) {
                colisionE.x-=velEnemy;
                if (colisionE.x+delay<=0) {
                    delay = GetRandomValue(200, 600);
                    trocarEnemy(&enemy, &colisionE.y, &colisionE.width, &colisionE.height, &velEnemy);
                    colisionE.x = GetScreenWidth();
                }
            }
            
            else if (frameCounter >= enemyDelay && enemy == 2) {
                colisionE.x-=velEnemy;
                if (colisionE.x+delay<=0) {
                    delay = GetRandomValue(800, 900);               
                    trocarEnemy(&enemy, &colisionE.y, &colisionE.width, &colisionE.height, &velEnemy);
                    colisionE.x = GetScreenWidth();
                }
            }
            
            // Controle do jogador
            
            // Pulo
            if (IsKeyPressed(KEY_SPACE) && !jump && !slide){
                currentAnimFrame = 0;
                colisionP.height = imRun.height-120;
                texAtual = texJump;
                jump = true;
            }
            // Desliza 
            else if (IsKeyPressed(KEY_DOWN) && !jump && !slide){
                colisionP.y = GetScreenHeight()/2 + 180;
                currentAnimFrame = 0;
                texAtual = texSlide;
                slide = true;
            }
            // Volta para o Menu
            if (IsKeyPressed(KEY_Q)){
                *menuT = true;
                *gameplayT = false;
                return;
            }
            
            // Verifica a colisao
            if (CheckCollisionRecs(colisionE, colisionP)){
                *gameoverT = true;
                *gameplayT = false;
                return;
            }
        }
        // --------Draw ---------
        BeginDrawing();

            ClearBackground(WHITE);
            // Desenhar o fundo primeira parte
            DrawTextureEx(background, (Vector2){ scrolling, 0}, 0.0f, 1.0f, WHITE);
            // Segunda parte do fundo
            DrawTextureEx(background, (Vector2){ background.width + scrolling, 0 }, 0.0f, 1.0f, WHITE);
            
            DrawText("'Espaço' para pular\n\n\n'Seta p/ baixo' para deslizar\n\n\n'P' para pausar",70,30,30,RED);
            //Desenhar o player
            if (slide == true) DrawTexture(texAtual, posX, colisionP.y - 180, WHITE); //Ajusta o tamanho da imagem quando tiver deslizando
            else DrawTexture(texAtual, posX, colisionP.y, WHITE); // Não alterar quando não tiver deslizando
            
            //Dsenhar os obstaculos
            if (enemy == 0)DrawTextureEx(cerca, (Vector2){ (float)colisionE.x-120, colisionE.height }, 0.0f, 0.15f, WHITE);
            else if (enemy == 1)DrawTextureEx(arvore, (Vector2){ (float)colisionE.x-100, colisionE.height-10 }, 0.0f, 0.2f, WHITE);
            else if (enemy == 2)DrawTextureEx(passaro, (Vector2){ (float)colisionE.x-90, colisionE.height+250 }, 0.0f, 0.2f, WHITE);
            
            //Desenhar o ganso
            DrawTexture(texGanso, -50, GetScreenHeight() - 240, WHITE);
            
            //Desenhar a linha de colisão
            //DrawRectangleLines(colisionP.x,(float)colisionP.y,colisionP.width,colisionP.height, RED);
            
            //Desenho do inimigo (teste)
            //DrawRectangleLines((float)colisionE.x,(float)colisionE.y,colisionE.width,colisionE.height,RED);
            
            //Pause
            if (pause) DrawText("Jogo Pausado",GetScreenWidth()/2-150,GetScreenHeight()/2-100,40,BLUE);
            
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload texture
    UnloadTexture(texAtual);  
    UnloadTexture(background);
    UnloadTexture(cerca); 
    UnloadTexture(arvore); 
    UnloadTexture(passaro);     
    // Unload image (contains all frames)
    UnloadImage(imRun);   
    UnloadImage(imJump);    
    UnloadImage(imSlide);
    UnloadImage(imGanso);
    // Close window and OpenGL context
    CloseWindow();                  
    //--------------------------------------------------------------------------------------

}

// Função de gameOver 
void over(bool *menuT, bool *gameplayT, bool *gameoverT){
    
    Music music = LoadMusicStream("musicas/musica_de_game_over_jogar_de_novo.mp3");
    PlayMusicStream(music);
    Image image = LoadImage("Desenhos/MENU_MORTE.png");  // Load image data 
    Texture2D texture = LoadTextureFromImage(image);       // Image converted to texture 
    
    SetTargetFPS(60);
    
    while(!WindowShouldClose()){
        
        UpdateMusicStream(music);
        
        // Volta para tela de menu
        if (IsKeyPressed(KEY_M)){
            *menuT = true;
            *gameoverT = false;
            return;
        }        
        // Volta para o jogo
        if (IsKeyPressed(KEY_N)){
            *gameplayT = true;
            *gameoverT = false;
            return;
        }
        
        // --------Draw ---------
            BeginDrawing();
                
                //Desenha menu
                ClearBackground(RED);
                // Coloca a tela de fundo
                DrawTexture(texture, 0, 0, WHITE);
                DrawText("Menu: M\n\n\nJogar novamente: N",GetScreenWidth()/4,GetScreenHeight()/2 + 250,40,WHITE);

            EndDrawing();
        }
        UnloadImage(image);
        CloseWindow();    
}

// Codigo principal
void main(){
    InitAudioDevice();
    int screenWidth = 1080;
    int screenHeight = 720;
    struct Tela tela = {true,false,false,false};
    InitWindow(screenWidth, screenHeight, "Oh my Goose!");
    // Detectar o ESC sai do jogo
    while (!WindowShouldClose()){
        // Entra na tela de menu
        if (tela.menu == true){ 
            menu(&tela.menu, &tela.gameplay);
        }
        // Entra na tela do jogo
        else if (tela.gameplay == true){ 
            gaming(&tela.menu, &tela.gameplay, &tela.pause, &tela.gameover);
        }
        // Entra na tela de GameOver
        else if (tela.gameover == true){
            over(&tela.menu, &tela.gameplay, &tela.gameover);
        }
    }
    CloseAudioDevice();
    CloseWindow();
}