#include "Win.h"

Win::Win()                                      : Scene { {0.0f}, nullptr   } {}
Win::Win(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Win::~Win() { shutdown(); }

void Win::initialise()
{
   mGameState.nextSceneID = 0;

   mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
   SetMusicVolume(mGameState.bgm, 0.33f);
   // PlayMusicStream(gState.bgm);

   //mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
   mGameState.misc1 = new Entity(
      {mOrigin.x, mOrigin.y},
      {500.0f, 100.0f},             // scale
      "assets/game/Win.png",                   // texture file address
      NONE
   );
   mGameState.misc2 = new Entity(
      {mOrigin.x, mOrigin.y+100.0f},
      {400.0f, 75.0f},             // scale
      "assets/game/EnterToRestart.png",                   // texture file address
      NONE
   );

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin;
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
   printf("finished init\n");
}

void Win::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   //if (mGameState.mimi->getPosition().y > 800.0f) mGameState.nextSceneID = 1;
   //if (mGameState.mimi->getContinue() == SWAP) mGameState.nextSceneID = 1;

   //panCamera(&mGameState.camera, &currentPlayerPosition);
   //
}

void Win::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.misc1->render();
   mGameState.misc2->render();
}

void Win::shutdown()
{
   delete mGameState.misc1;
   delete mGameState.misc2;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
}