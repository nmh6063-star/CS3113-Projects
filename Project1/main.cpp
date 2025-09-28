/**
* Author: Natt Hong
* Assignment: Simple 2D Scene
* Date due: 2025-09-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"
#include <math.h>

enum Direction { UPRIGHT, DOWNRIGHT, DOWNLEFT, UPLEFT };
Direction direction = UPRIGHT;

// Global Constants
constexpr int   SCREEN_WIDTH  = 1600 / 2,
                SCREEN_HEIGHT = 900 / 2,
                FPS           = 60,
                SIZE          = 150,
                FRAME_LIMIT   = 100;
constexpr float MAX_AMP       = 10.0f,
                RADIUS = 75.0f,
                DIST_DIFF = 25.0f;

char    BG_COLOUR[] = "#000000ff";
constexpr char SELECT_HEX[] = "0123456789abcdef";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE   = { (float) SIZE, (float) SIZE };
constexpr Vector2 BASE_SIZE2   = { ((float) SIZE) / 2.0f, ((float) SIZE) / 2.0f };

constexpr char CLAUDE_FP[] = "./assets/ClaudeMyBeloved.png";
constexpr char NOODLE_FP[] = "./assets/Noodle.png";
constexpr char SANS_FP[] = "./assets/snas.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gScaleFactor   = SIZE,
          gAngle         = 0.0f,
          gAngle2         = 0.0f,
          gPreviousTicks = 0.0f,
          gConstantIncrease = 0.0f,
          changeClock = 0.0f,
          lastDiagonalLimit = 0.0f;
Vector2   gPosition      = ORIGIN;
Vector2   gPosition2      = ORIGIN;
Vector2   gPosition3      = { ORIGIN.x/2.0f, ORIGIN.y/2.0f };
Vector2   gScale         = BASE_SIZE;
Vector2   gScale2         = BASE_SIZE2;
Vector2   gScale3         = BASE_SIZE;
Vector2 diagonalLimit = {0.0f, 0.0f};
int frame = 0;
int test = 0;

Texture2D gTexture;
Texture2D gTexture2;
Texture2D gTexture3;
Rectangle textureArea;
Rectangle textureArea2;
Rectangle textureArea3;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures");

    gTexture = LoadTexture(CLAUDE_FP);
    gTexture2 = LoadTexture(NOODLE_FP);
    gTexture3 = LoadTexture(SANS_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    frame += 1;

    gConstantIncrease += 1.0f * deltaTime;

    gScale3 = {
        BASE_SIZE.x + MAX_AMP * cos((gConstantIncrease) * 30.0f),
        BASE_SIZE.y + MAX_AMP * cos((gConstantIncrease) * 30.0f)
    };

    diagonalLimit.x += (1.0f * deltaTime) * (direction == UPRIGHT ? 1 : direction == DOWNRIGHT ? 1 : -1);
    diagonalLimit.y += (1.0f * deltaTime) * (direction == UPRIGHT ? -1 : direction == UPLEFT ? -1 : 1);


    if(abs(diagonalLimit.x - lastDiagonalLimit) > 1.5f){
        switch(direction){
            case(UPRIGHT):
                direction = DOWNRIGHT;
                break;
            case(DOWNRIGHT):
                direction = DOWNLEFT;
                break;
            case(DOWNLEFT):
                direction = UPLEFT;
                break;
            case(UPLEFT):
                direction = UPRIGHT;
                break;
        }
        lastDiagonalLimit = diagonalLimit.x;
    }

    gPosition2.x = (gPosition.x-DIST_DIFF) + ((diagonalLimit.x * 25.0f));
    gPosition2.y = (gPosition.y+DIST_DIFF) + ((diagonalLimit.y * 25.0f));
    gAngle2 += cos(gConstantIncrease * 5.0f);

    gPosition3.x = gPosition3.x + ((sin(gConstantIncrease*3.0f)));
    gPosition3.y = gPosition3.y + (sin(gConstantIncrease)*2.0f);

    gPosition.x = ORIGIN.x + (RADIUS * cos(gConstantIncrease));
    gPosition.y = ORIGIN.y + (RADIUS * sin(gConstantIncrease));
}

void render()
{
    BeginDrawing();
    for(int i = 6; i > 0; i--){
        bool next = false;
        for(int x = 0; x < 16; x++){
            if(SELECT_HEX[x] == BG_COLOUR[i]){
                if(x == 15){
                    next = true;
                    BG_COLOUR[i] = SELECT_HEX[0];
                    break;
                }
                BG_COLOUR[i] = SELECT_HEX[x + 1];
                break;
            }
        }
        if(!next){
            break;
        }
    }
    ClearBackground(ColorFromHex(BG_COLOUR));
    Rectangle destinationArea = {
        gPosition.x,
        gPosition.y,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    Rectangle destinationArea2 = {
        gPosition2.x,
        gPosition2.y,
        static_cast<float>(gScale2.x),
        static_cast<float>(gScale2.y)
    };


    Rectangle destinationArea3 = {
        gPosition3.x,
        gPosition3.y,
        static_cast<float>(gScale3.x),
        static_cast<float>(gScale3.y)
    };
    Vector2 objectOrigin = {
        static_cast<float>(gScale.x) / 2.0f,
        static_cast<float>(gScale.y) / 2.0f
    };

    Vector2 objectOrigin2 = {
        static_cast<float>(gScale2.x) / 2.0f,
        static_cast<float>(gScale2.y) / 2.0f
    };

    Vector2 objectOrigin3 = {
        static_cast<float>(gScale3.x) / 2.0f,
        static_cast<float>(gScale3.y) / 2.0f
    };

    textureArea = {
        0.0f, 0.0f,

        static_cast<float>(gTexture.width), static_cast<float>(gTexture.height)
    };

    textureArea2 = {
        0.0f, 0.0f,

        static_cast<float>(gTexture2.width), static_cast<float>(gTexture2.height)
    };

    textureArea3 = {
        0.0f, 0.0f,

        static_cast<float>(gTexture3.width), static_cast<float>(gTexture3.height)
    };

    DrawTexturePro(
        gTexture, 
        textureArea, 
        destinationArea, 
        objectOrigin, 
        gAngle, 
        WHITE
    );

    DrawTexturePro(
        gTexture2, 
        textureArea2, 
        destinationArea2, 
        objectOrigin2, 
        gAngle2, 
        WHITE
    );

    DrawTexturePro(
        gTexture3, 
        textureArea3, 
        destinationArea3, 
        objectOrigin3, 
        gAngle, 
        WHITE
    );

    EndDrawing();
}

void shutdown() { CloseWindow(); }

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}