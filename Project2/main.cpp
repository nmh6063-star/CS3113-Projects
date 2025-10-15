/**
* Author: Natt Hong
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

enum Player2Status { PLAYER, CPU };
enum Direction { UP, DOWN };
enum Winner {P1, P2};

// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60,
              SPEED         = 200,
              SHRINK_RATE   = 100;

constexpr Vector2 ORIGIN             = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  PADDLE_BASE_SIZE = { 100.0f, 100.0f },
                  PADDLE_COLLIDER_DIMENSIONS = { 5.0f, 75.0f },
                  BORDER1_BASE_SIZE = {1500.0f, 500.0f},
                  BORDER2_BASE_SIZE = {500.0f, 1000.0f},
                  BORDER1_COLLIDER_DIMENSIONS = {1000.0f, 25.0f},
                  BORDER2_COLLIDER_DIMENSIONS = {25.0f, 1000.0f},
                  BALL_COLLIDER_DIMENSIONS = {25.0f, 25.0f},
                  BALL_BASE_SIZE   = { 100.0f, 100.0f },
                  P1_INIT_POS  = { ORIGIN.x + 300.0f, ORIGIN.y },
                  P2_INIT_POS = {ORIGIN.x - 300.0f, ORIGIN.y},
                  BORDER_LEFT_POS = {ORIGIN.x - 400.0f, ORIGIN.y},
                  BORDER_RIGHT_POS = {ORIGIN.x + 400.0f, ORIGIN.y},
                  BORDER_TOP_POS = {ORIGIN.x, ORIGIN.y - 225.0f},
                  BORDER_BOTTOM_POS = {ORIGIN.x, ORIGIN.y + 225.0f},
                  TEXT_BASE_SIZE = {500.0f, 250.0f};

// @see https://idol.st/idols/ for source
constexpr char P1_FP[] = "assets/P1Paddle.png";
constexpr char P2_FP[]   = "assets/P2Paddle.png";
constexpr char BALL_FP[]   = "assets/Ball.png";
constexpr char BORDER1_FP[] = "assets/Border1.png";
constexpr char BORDER2_FP[]   = "assets/Border2.png";
constexpr char P1WIN_FP[]   = "assets/P1Win.png";
constexpr char P2WIN_FP[]   = "assets/P2Win.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gPositionP1 = P1_INIT_POS,
        gPositionP2 = P2_INIT_POS,
        gPositionBall[3],
        gMovementP1 = { 0.0f, 0.0f },
        gMovementP2 = { 0.0f, 0.0f },
        gMovementBall[3],
        gScalePaddle    = PADDLE_BASE_SIZE,
        gScaleBall = BALL_BASE_SIZE;

Texture2D gTextureP1;
Texture2D gTextureP2;
Texture2D gTextureBorder1;
Texture2D gTextureBorder2;
Texture2D gTextureBall;
Texture2D gTextureWin1;
Texture2D gTextureWin2;

Player2Status P2Status = PLAYER;
Direction dir = DOWN;
Winner winner = P1;

bool endGame = false;
int balls = 1;

char    BG_COLOUR[] = "#000000ff";

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 * 
 * @see 
 * 
 * @param postionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(postionA->x - positionB->x) - 
                      ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(postionA->y - positionB->y) - 
                      ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void displayCollider(Vector2 mPosition, Vector2 mColliderDimensions) 
{
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,  
        mPosition.y - mColliderDimensions.y / 2.0f,  
        mColliderDimensions.x,                        
        mColliderDimensions.y                        
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}

void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 
        "User Input / Collision Detection");

    gTextureP1 = LoadTexture(P1_FP);
    gTextureP2   = LoadTexture(P2_FP);
    gTextureBall = LoadTexture(BALL_FP);
    gTextureBorder1 = LoadTexture(BORDER1_FP);
    gTextureBorder2 = LoadTexture(BORDER2_FP);
    gTextureWin1 = LoadTexture(P1WIN_FP);
    gTextureWin2 = LoadTexture(P2WIN_FP);
    for(int i = 0; i < 3; i++){
        if(i == 0){
            gMovementBall[i] = {1.0f, 1.0f};
        }
        else if(i == 1){
            gMovementBall[i] = {-1.0f, 1.0f};
        }
        else{
            gMovementBall[i] = {1.0f, -1.0f};
        }
        gPositionBall[i] = ORIGIN;
    }

    SetTargetFPS(FPS);
}

void processInput() 
{
    // TODO - Get mouse position

    // TODO - Check if mouse coordinates fall within teardrop object

    // TODO - If the player presses left mouse button within teardrop, release
    //        teardrop (i.e. use its movement vector)

    // to avoid faster diagonal speed
    for(int i = 0; i < balls; i++){
        if (GetLength(&gMovementBall[i]) > 1.0f) Normalise(&gMovementBall[i]);
    }

    gMovementP1 = { 0.0f, 0.0f };
    gMovementP2 = { 0.0f, 0.0f };
    if(P2Status == PLAYER){
        if      (IsKeyDown(KEY_W)) gMovementP2.y = -1; // up
        else if (IsKeyDown(KEY_S)) gMovementP2.y =  1; // down
    }
    else{
        if(dir == DOWN){
            gMovementP2.y = -1;
        }
        else{
            gMovementP2.y = 1;
        }
    }
    if      (IsKeyDown(KEY_UP)) gMovementP1.y = -1; // up
    else if (IsKeyDown(KEY_DOWN)) gMovementP1.y =  1; // down
    if(IsKeyPressed(KEY_T)){
        if(P2Status == PLAYER){
            P2Status = CPU;
        }
        else{
            P2Status = PLAYER;
        }
    }
    if(IsKeyPressed(KEY_ONE)){
        balls = 1;
        gPositionBall[1] = ORIGIN;
        gPositionBall[2] = ORIGIN;
    }
    else if(IsKeyPressed(KEY_TWO)){
        balls = 2;
        gPositionBall[2] = ORIGIN;
    }
    else if(IsKeyPressed(KEY_THREE)){
        balls = 3;
    }
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;
    if(!endGame){

        float P1ClampY = gPositionP1.y + SPEED * gMovementP1.y * deltaTime;
        float P2ClampY = gPositionP2.y + SPEED * gMovementP2.y * deltaTime;
        if(P1ClampY > 400.0f){
            P1ClampY = 400.0f;
        }
        else if(P1ClampY < 50.0f){
            P1ClampY = 50.0f;
        }
        if(P2ClampY > 400.0f){
            P2ClampY = 400.0f;
            dir = DOWN;
        }
        else if(P2ClampY < 50.0f){
            P2ClampY = 50.0f;
            dir = UP;
        }
        gPositionP1 = {
            gPositionP1.x,
            P1ClampY
        };
        gPositionP2 = {
            gPositionP2.x,
            P2ClampY
        };

        for(int i = 0; i < balls; i++){
            gPositionBall[i] = {
                gPositionBall[i].x + SPEED * gMovementBall[i].x * deltaTime,
                gPositionBall[i].y + SPEED * gMovementBall[i].y * deltaTime
            };
            if(isColliding(&gPositionBall[i], &BALL_COLLIDER_DIMENSIONS, &BORDER_BOTTOM_POS, &BORDER1_COLLIDER_DIMENSIONS) || 
            isColliding(&gPositionBall[i], &BALL_COLLIDER_DIMENSIONS, &BORDER_TOP_POS, &BORDER1_COLLIDER_DIMENSIONS)){
            gMovementBall[i] = {
                gMovementBall[i].x,
                -gMovementBall[i].y
            };
        }
        else if(isColliding(&gPositionBall[i], &BALL_COLLIDER_DIMENSIONS, &gPositionP1, &PADDLE_COLLIDER_DIMENSIONS)){
            gMovementBall[i] = {
                -gMovementBall[i].x,
                gMovementBall[i].y
            };
            while(isColliding(&gPositionBall[i], &BALL_COLLIDER_DIMENSIONS, &gPositionP1, &PADDLE_COLLIDER_DIMENSIONS)){
                gPositionBall[i] = {
                    gPositionBall[i].x + (gMovementBall[i].x),
                    gPositionBall[i].y
                };
            }
        }
        else if(isColliding(&gPositionBall[i], &BALL_COLLIDER_DIMENSIONS, &gPositionP2, &PADDLE_COLLIDER_DIMENSIONS)){
            gMovementBall[i] = {
                -gMovementBall[i].x,
                gMovementBall[i].y
            };
            while(isColliding(&gPositionBall[i], &BALL_COLLIDER_DIMENSIONS, &gPositionP2, &PADDLE_COLLIDER_DIMENSIONS)){
                gPositionBall[i] = {
                    gPositionBall[i].x + (gMovementBall[i].x),
                    gPositionBall[i].y
                };
            }
        }
        if(isColliding(&gPositionBall[i], &BALL_COLLIDER_DIMENSIONS, &BORDER_LEFT_POS, &BORDER2_COLLIDER_DIMENSIONS)){
            winner = P1;
            endGame = true;
        }
        else if(isColliding(&gPositionBall[i], &BALL_COLLIDER_DIMENSIONS, &BORDER_RIGHT_POS, &BORDER2_COLLIDER_DIMENSIONS)){
            winner = P2;
            endGame = true;
        }
        }
    }

    // TODO - Stop horizontal translation once teardrop is released
    //gMousePosition = GetMousePosition();

    //printf("test position %f", gPosition.x);

    // TODO - Add vertical translation for teardrop

    // TODO - If teardrop is colliding with beaker, shrink the size of teardrop
    //        in a way that is FRAME-INDEPENDENT
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    if(!endGame){
    // render teardrop
        renderObject(&gTextureP1, &gPositionP1, &gScalePaddle);
        renderObject(&gTextureP2, &gPositionP2, &gScalePaddle);

        renderObject(&gTextureBorder1, &BORDER_TOP_POS, &BORDER1_BASE_SIZE);
        renderObject(&gTextureBorder1, &BORDER_BOTTOM_POS, &BORDER1_BASE_SIZE);
        renderObject(&gTextureBorder2, &BORDER_LEFT_POS, &BORDER2_BASE_SIZE);
        renderObject(&gTextureBorder2, &BORDER_RIGHT_POS, &BORDER2_BASE_SIZE);

        // render the rupee
        for(int i = 0; i < balls; i++){
            renderObject(&gTextureBall, &gPositionBall[i], &gScaleBall);
        }
    }
    else{
        if(winner == P1){
            renderObject(&gTextureWin1, &ORIGIN, &TEXT_BASE_SIZE);
        }
        else{
            renderObject(&gTextureWin2, &ORIGIN, &TEXT_BASE_SIZE);
        }
    }

    displayCollider(gPositionP1, PADDLE_COLLIDER_DIMENSIONS);
    //displayCollider(gPositionBall, BALL_COLLIDER_DIMENSIONS);

    EndDrawing();
}

void shutdown() {
    CloseWindow();
    UnloadTexture(gTextureP1);
    UnloadTexture(gTextureP2);
    UnloadTexture(gTextureBall);
    UnloadTexture(gTextureBorder1);
    UnloadTexture(gTextureBorder2);
    UnloadTexture(gTextureWin1);
    UnloadTexture(gTextureWin2);
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