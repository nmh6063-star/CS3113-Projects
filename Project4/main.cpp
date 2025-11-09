#include "CS3113/LevelC.h"
#include "CS3113/Start.h"
#include "CS3113/Win.h"
#include "CS3113/Fail.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 5;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

Start *gStart = nullptr;
Win *gWin = nullptr;
Fail *gFail = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();
    
    gStart = new Start(ORIGIN, "#000000");
    gWin = new Win(ORIGIN, "#000000");
    gFail = new Fail(ORIGIN, "#000000");
    gLevelA = new LevelA(ORIGIN, "#C0897E");
    gLevelB = new LevelB(ORIGIN, "#011627");
    gLevelC = new LevelC(ORIGIN, "#C0897E");
    
    gLevels.push_back(gStart);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);
    gLevels.push_back(gFail);
    gLevels.push_back(gWin);

    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if(gCurrentScene != gStart && gCurrentScene != gFail && gCurrentScene != gWin){
        gCurrentScene->getState().mimi->resetMovement();

        if      (IsKeyDown(KEY_A)) {gCurrentScene->getState().mimi->moveLeft();
        gCurrentScene->getState().mimi->setFrameSpeed(4);}
        else if (IsKeyDown(KEY_D)) {gCurrentScene->getState().mimi->moveRight();
        gCurrentScene->getState().mimi->setFrameSpeed(4);}
        else if(gCurrentScene->getState().mimi->getDirection() == LEFT) {gCurrentScene->getState().mimi->moveToIdleLeft();
        gCurrentScene->getState().mimi->setFrameSpeed(3);}
        else if(gCurrentScene->getState().mimi->getDirection() == RIGHT) {gCurrentScene->getState().mimi->moveToIdleRight();
        gCurrentScene->getState().mimi->setFrameSpeed(3);}

        if (IsKeyPressed(KEY_W)){
            if(gCurrentScene->getState().mimi->getContinue() == CONTINUE){
                PlaySound(gCurrentScene->getState().doorSound);
                gCurrentScene->getState().mimi->setContinue(SWAP);
            }
            else if( gCurrentScene->getState().mimi->isCollidingBottom()){
                gCurrentScene->getState().mimi->jump();
                PlaySound(gCurrentScene->getState().jumpSound);
            }
        }

        if (GetLength(gCurrentScene->getState().mimi->getMovement()) > 1.0f) 
            gCurrentScene->getState().mimi->normaliseMovement();
    }
    else{
        if(IsKeyPressed(KEY_ENTER)){
            if(gCurrentScene == gFail || gCurrentScene == gWin){
                switchToScene(gLevels[0]);
            }
            else{
                gLevelA->setLives(3);
                switchToScene(gLevels[1]);
                gCurrentScene->setScreenHeight(SCREEN_HEIGHT);
                gCurrentScene->setScreenWidth(SCREEN_WIDTH);
            }
        }
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCurrentScene->getState().camera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    delete gStart;
    gStart = nullptr;
    delete gWin;
    gWin = nullptr;
    delete gFail;
    gFail = nullptr;
    delete gLevelA;
    gLevelA = nullptr;
    delete gLevelB;
    gLevelB = nullptr;
    delete gLevelC;
    gLevelC = nullptr;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID > 0)
        {
            int currLives = gCurrentScene->getState().lives;
            int id = gCurrentScene->getState().nextSceneID;
            gLevels[id]->setLives(currLives);
            printf("we switched to %d\n", id);
            switchToScene(gLevels[id]);
            gCurrentScene->resetNextScene();
            gCurrentScene->setScreenHeight(SCREEN_HEIGHT);
            gCurrentScene->setScreenWidth(SCREEN_WIDTH);
        }

        render();
    }

    shutdown();

    return 0;
}