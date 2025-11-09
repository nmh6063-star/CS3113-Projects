#include "LevelB.h"

LevelB::LevelB()                                      : Scene { {0.0f}, nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{
   mGameState.bgm = LoadMusicStream("assets/game/3D Dot Game Heroes - Breakout.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/game/Jump.wav");
   mGameState.deathSound = LoadSound("assets/game/Death.wav");
   mGameState.doorSound = LoadSound("assets/game/DoorOpen.wav");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/game/Pixel-Tileset.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      4, 1,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> mimiAnimationAtlas = {
      {IDLERIGHT,  {  1,  2,  3,  4}},
      {IDLELEFT, {17, 16, 15, 14}},
      {LEFT,  {  13, 12, 11, 10}},
      {RIGHT, { 5, 6, 7, 8}},
   };

   float sizeRatio  = 48.0f / 64.0f;

   mGameState.mimi = new Entity(
      {mOrigin.x - 500.0f, mOrigin.y-300.0f}, // position
      {100.0f * sizeRatio, 100.0f},             // scale
      "assets/game/Player-Sheet.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 2,  9},                                 // atlas dimensions
      mimiAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.misc1 = new Entity(
      {(mOrigin.x - (LEVEL_WIDTH * TILE_DIMENSION) / 2.0f) + (16.5f * TILE_DIMENSION), 
         (mOrigin.y - (LEVEL_HEIGHT * TILE_DIMENSION) / 2.0f) + (3.5f * TILE_DIMENSION)}, // position
      {90.0f, 90.0f},             // scale
      "assets/game/PixelDoor.png",                   // texture file address
      DOOR
   );
   mGameState.misc1->setColliderDimensions({mGameState.misc1->getScale().x / 2.0f, mGameState.misc1->getScale().y});

   std::map<Direction, std::vector<int>> birdAnimationAtlas = {
        {LEFT,  { 0, 1, 2, 3, 4, 5, 6, 7 }},
        {RIGHT, { 15, 14, 13, 12, 11, 10, 9, 8 }},
    };

   mGameState.misc2 = new Entity({
      {mOrigin.x + 250.0f, mOrigin.y}, // position
      {100.0f * sizeRatio, 100.0f},             // scale
      "assets/game/BirdSprite.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 1,  16},                                 // atlas dimensions
      birdAnimationAtlas,                    // actual atlas
      NPC
   });
   mGameState.misc2->setAIType(FLYER);
   mGameState.misc2->setAIState(IDLE);
   mGameState.misc2->setSpeed(Entity::DEFAULT_SPEED * 0.50f);
   mGameState.misc2->setColliderDimensions({
        mGameState.misc2->getScale().x / 2.0f,
        mGameState.misc2->getScale().y
    });
    mGameState.misc2->setFlyerPos(
      {mOrigin.x-100.0f, mOrigin.y-50.0f},
      {mOrigin.x+200.0f, mOrigin.y+100.0f}
   );
   mGameState.misc2->setDirection(RIGHT);

    mGameState.misc2->setAcceleration({0.0f, 0.0f});
    mGameState.misc2->setDirection(RIGHT);
    mGameState.misc2->setFrameSpeed(5);


   if(mGameState.lives > 2){
      mGameState.heart3 = new Entity();
      mGameState.heart3->setTexture("assets/game/Heart.png");
      mGameState.heart3->setEntityType(NONE);
      mGameState.heart3->setScale({50.0f, 50.0f});
   }
   if(mGameState.lives > 1){
      mGameState.heart2 = new Entity();
      mGameState.heart2->setTexture("assets/game/Heart.png");
      mGameState.heart2->setEntityType(NONE);
      mGameState.heart2->setScale({50.0f, 50.0f});
   }
   mGameState.heart1 = new Entity();
   mGameState.heart1->setTexture("assets/game/Heart.png");
   mGameState.heart1->setEntityType(NONE);
   mGameState.heart1->setScale({50.0f, 50.0f});

   mGameState.mimi->setJumpingPower(550.0f);
   mGameState.mimi->setColliderDimensions({
      mGameState.mimi->getScale().x / 1.5f,
      mGameState.mimi->getScale().y / 1.0f
   });
   

   mGameState.mimi->setJumpingPower(550.0f);
   mGameState.mimi->setColliderDimensions({
      mGameState.mimi->getScale().x / 1.5f,
      mGameState.mimi->getScale().y / 1.0f
   });
   mGameState.mimi->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.mimi->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelB::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.mimi->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      mGameState.misc1,        // collidable entities
      1               // col. entity count
   );

   mGameState.misc2->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      mGameState.mimi,        // collidable entities
      1               // col. entity count
   );

   // CAMERA
   Vector2 currentPlayerPosition = { mGameState.mimi->getPosition().x, mOrigin.y };

   if (mGameState.mimi->getPosition().y > 800.0f || mGameState.mimi->checkKill() == true) {mGameState.lives--; mGameState.mimi->setPosition({mOrigin.x - 500.0f, mOrigin.y-300.0f}); mGameState.mimi->resetKill(); PlaySound(mGameState.deathSound);};
   if (mGameState.mimi->getContinue() == SWAP) mGameState.nextSceneID = 3;

   panCamera(&mGameState.camera, &currentPlayerPosition);
   if(mGameState.lives > 2){
      mGameState.heart3->setPosition({((mGameState.camera.target.x - mGameState.screenWidth / 2.0f) + 75.0f * (2)) + 100.0f,
      (mGameState.camera.target.y - mGameState.screenHeight / 2.0f) + 50.0f});
   }
   if(mGameState.lives > 1){
      mGameState.heart2->setPosition({((mGameState.camera.target.x - mGameState.screenWidth / 2.0f) + 75.0f) + 100.0f,
      (mGameState.camera.target.y - mGameState.screenHeight / 2.0f) + 50.0f});
   }
   mGameState.heart1->setPosition({((mGameState.camera.target.x - mGameState.screenWidth / 2.0f)) + 100.0f,
      (mGameState.camera.target.y - mGameState.screenHeight / 2.0f) + 50.0f});
   
   if(mGameState.lives <= 0){
      mGameState.nextSceneID = 4;
   }
}

void LevelB::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.misc1->render();
   mGameState.misc2->render();
   mGameState.mimi->render();
   mGameState.map->render();
   if(mGameState.lives > 2){
      mGameState.heart3->render();
   }
   if(mGameState.lives > 1){
      mGameState.heart2->render();
   }
   mGameState.heart1->render();
}

void LevelB::shutdown()
{
   delete mGameState.mimi;
   delete mGameState.misc1;
   delete mGameState.misc2;
   delete mGameState.map;
   delete mGameState.heart3;
   delete mGameState.heart2;
   delete mGameState.heart1;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.deathSound);
   UnloadSound(mGameState.doorSound);
}