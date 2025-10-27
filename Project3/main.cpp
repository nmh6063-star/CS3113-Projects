/**
* Author: Natt Hong
* Assignment: Lunar Lander
* Date due: 2025-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#5D517A";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr int   NUMBER_OF_TILES         = 20,
                NUMBER_OF_BLOCKS        = 3;
constexpr float TILE_DIMENSION          = 50.0f,
                FUEL_VALUE = 10.0f,
                TURN_SPEED = 10.0f,
                AIR_RESISTANCE = 5.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 5.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD_Y      = 275.0f,
                GAME_THRESHOLD_X = 500.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Entity *ship = nullptr;
Entity *bar = nullptr;
Entity *barFill = nullptr;
Entity *platforms   = nullptr;
Entity *moonGround = nullptr;
Entity *moonGround2 = nullptr;
Entity *success = nullptr;
Entity *fail = nullptr;

float barFillPercent = 100.0f;

bool turnLeft = false;
bool turnRight = false;
bool canMove = true;
bool moving = false;

int gameState = 0;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LUNAR LANDER");

    std::map<Direction, std::vector<int>> animationAtlas = {
        {DOWN,  {  0}},
        {LEFT,  {  0}},
        {UP,    {  0, 1, 2, 3, 4}},
        {RIGHT, {  0}},
    };
    std::vector<int> platformAtlas = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
    };

    float sizeRatioShip  = 366.0f / 594.0f,
            sizeRatioBar = 474.0f/98.0f;

    ship = new Entity(
        {ORIGIN.x, ORIGIN.y - 200.0f}, // position
        {100.0f * sizeRatioShip, 100.0f},  // scale
        "assets/game/SpaceShipSpriteSheet.png",        // texture file address
        ATLAS,                         // single image or atlas?
        { 1, 5 },                      // atlas dimensions
        animationAtlas,                // actual atlas
        PLAYER                         // entity type
    );

    bar = new Entity(
        {ORIGIN.x - 375.0f, ORIGIN.y - 250.0f},
        {50.0f * sizeRatioBar, 50.0f},
        "assets/game/Bar.png", 
        UI
    );
    barFill = new Entity(
        {ORIGIN.x - 375.0f, ORIGIN.y - 251.0f},
        {210.0f, 200.0f},
        "assets/game/BarFill.png", 
        UI
    );
   platforms = new Entity[2];
    for(int i = 0; i < 2; i++){
    platforms[i].setTexture("assets/game/PlatformSpriteSheet.png");
    platforms[i].setEntityType(PLATFORM);
    platforms[i].setScale({200.0f, 200.0f});
    platforms[i].setColliderDimensions({
        platforms[i].getScale().x/3.0f,
        platforms[i].getScale().y/5.0f
    });
    platforms[i].setPosition({
        ORIGIN.x - 50.0f + (175.0f * i),
        ORIGIN.y+250.0f
    });
    platforms[i].setFrameSpeed(4);
    platforms[i].setMovement({i == 0 ? 1.0f : -1.0f, 0.0f});
    platforms[i].setTextureType(ATLAS);
    platforms[i].setSpriteSheetDimensions({1, 12});
    platforms[i].setAnimationIndicies(platformAtlas);
    platforms[i].setSpeed(50);
   }
    moonGround = new Entity(
        {ORIGIN.x, ORIGIN.y+150.f},
        {1000.0f, 400.0f},
        "assets/game/MoonGround.png",
        UI
    );
    moonGround2 = new Entity(
        {ORIGIN.x, ORIGIN.y-175.f},
        {1000.0f, 400.0f},
        "assets/game/MoonGround2.png",
        UI
    );
    success = new Entity(
        {ORIGIN.x, ORIGIN.y-150.f},
        {900.0f, 150.0f},
        "assets/game/MissionSuccess.png",
        UI
    );
    fail = new Entity(
        {ORIGIN.x, ORIGIN.y-150.f},
        {900.0f, 150.0f},
        "assets/game/MissionFail.png",
        UI
    );

    ship->setColliderDimensions({
        ship->getScale().x / 2.0f,
        ship->getScale().y / 3.0f
    });
    ship->setAcceleration({0, ACCELERATION_OF_GRAVITY});
    ship->setSpeed(10);
    SetTargetFPS(FPS);
}

void processInput() 
{
    ship->resetMovement();

    if      (IsKeyDown(KEY_A)){turnLeft = true; turnRight = false;}
    else if (IsKeyDown(KEY_D)){turnRight = true; turnLeft = false;}
    else{turnLeft = false; turnRight = false;}
    if (IsKeyDown(KEY_W) && canMove){ship->moveUp(); moving = true;
    ship->setAcceleration({static_cast<float>(cos(((2 * acos(0.0))/180)*(ship->getAngle()-90)) * ship->getSpeed()),
    static_cast<float>(sin(((2 * acos(0.0))/180)*(ship->getAngle()-90)) * ship->getSpeed())});}
    else {ship->moveDown(); moving = false; ship->setAcceleration({
        ship->getVelocity().x < 1.0f && ship->getVelocity().x > -1.0f ? 0 :
        ship->getVelocity().x > 0.0f ? -AIR_RESISTANCE : AIR_RESISTANCE,
        ACCELERATION_OF_GRAVITY});}

    // to avoid faster diagonal speed
    if (GetLength(ship->getMovement()) > 1.0f) 
        ship->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    if(gameState == 0){
        // Delta time
        float ticks = (float) GetTime();
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks  = ticks;

        // Fixed timestep
        deltaTime += gTimeAccumulator;

        if (deltaTime < FIXED_TIMESTEP)
        {
            gTimeAccumulator = deltaTime;
            return;
        }

        while (deltaTime >= FIXED_TIMESTEP)
        {
            ship->update(FIXED_TIMESTEP, platforms, 2);
            for(int i = 0; i < 2; i++){
                platforms[i].update(FIXED_TIMESTEP, nullptr, 0);
                if(platforms[i].getPosition().x > ORIGIN.x + 300.0f){
                    platforms[i].setMovement({-1.0, 0.0f});
                }
                else if(platforms[i].getPosition().x < ORIGIN.x - 300.0f){
                    platforms[i].setMovement({1.0, 0.0f});
                }
            }
            if(barFillPercent > -110.0f && moving) barFillPercent-= FIXED_TIMESTEP * FUEL_VALUE;
            if (barFillPercent <= -110.0f && canMove) canMove = false;
            deltaTime -= FIXED_TIMESTEP;
            if(turnLeft){
                ship->setAngle(ship->getAngle() - (FIXED_TIMESTEP * TURN_SPEED));
            }
            else if(turnRight){
                ship->setAngle(ship->getAngle() + (FIXED_TIMESTEP * TURN_SPEED));
            }
        }
        barFill->setScale({110.0f+barFillPercent, 200.0f});
        barFill->setPosition({ORIGIN.x - 375.0f - ((abs(barFillPercent - 100.0f))/2.0f), ORIGIN.y - 251.0f});
        if(ship->getPosition().x < ORIGIN.x - GAME_THRESHOLD_X){
            ship->setPosition({ORIGIN.x + GAME_THRESHOLD_X - 5.0f, ship->getPosition().y});
        }
        else if(ship->getPosition().x > ORIGIN.x + GAME_THRESHOLD_X){
            ship->setPosition({ORIGIN.x - GAME_THRESHOLD_X + 5.0f, ship->getPosition().y});
        }
        if (ship->getPosition().y > ORIGIN.y + END_GAME_THRESHOLD_Y || ship->getPosition().y < ORIGIN.y - END_GAME_THRESHOLD_Y) 
            gameState = 2;
        else if(!ship->isActive()){
            gameState = 1;
        }
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    moonGround->render();
    moonGround2->render();
    ship->render();
    bar->render();
    barFill->render();
    for(int i = 0; i < 2; i++){
        platforms[i].render();
    }
    if(gameState == 1){
        success->render();
    }
    else if(gameState == 2){
        fail->render();
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
}

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