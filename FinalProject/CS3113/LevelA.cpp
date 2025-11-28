#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr, {0.0f}   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode, Vector2 boundaries) : Scene { origin, bgHexCode, boundaries } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.nextSceneID = 0;

   mGameState.bpm = 175;
   mGameState.steps = 1.0f;
   mGameState.pulseStep = 1.0f;

   status = 0;

   mGameState.bgm = LoadMusicStream("assets/game/Jamie Paige - Machine Love - 01 Machine Love.wav");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   media = LoadMedia("assets/game/MachineLoveVideo.mp4");

   mGameState.hit = LoadSound("assets/game/clap.wav");

   /*
      ----------- MAP -----------
   */

   mGameState.map = nullptr;

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> playerAnimationAtlas = {
      {FACE_FORWARD, {0}},
      {FACE_LEFT, {1}},
      {FACE_RIGHT, {2}},
      {FACE_UP, {3}},
      {FACE_DOWN, {4}},
   };

   //float sizeRatio  = 32.0f / 64.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.player = new Entity(
      {mOrigin.x, mOrigin.y+75.0f}, // position
      {160.0f, 160.0f},             // scale
      "assets/game/LilGuy-Sheet.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 1, 5 },                                 // atlas dimensions
      playerAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.hook = new Entity(
      {mOrigin.x, mOrigin.y}, // position
      {160.0f, 160.0f},             // scale
      "assets/game/Barrier.png",                   // texture file address
      HOOK                                    // entity type
   );
   mGameState.hook->setColliderDimensions(
      {64.0f, 64.0f}
   );

   mGameState.arrows = new Entity*[15];
   for(int i = 0; i < 15; i++){
      mGameState.arrows[i] = new Entity(
      {mOrigin.x-50.0f, mOrigin.y}, // position
      {160.0f, 160.0f},             // scale
      "assets/game/Arrow.png",                   // texture file address
      ARROW                                    // entity type
   );
   mGameState.arrows[i]->setColliderDimensions(
      {64.0f, 64.0f}
   );
   }

   mGameState.player->setJumpingPower(550.0f);
   mGameState.player->setColliderDimensions({
      mGameState.player->getScale().x / 3.5f,
      mGameState.player->getScale().y / 3.0f
   });
   mGameState.player->setStartScale(mGameState.player->getScale());
   //mGameState.player->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.player->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelA::update(float deltaTime)
{
   if(!finished){
      UpdateMusicStream(mGameState.bgm);
      if(!IsMusicStreamPlaying(mGameState.bgm)){
      }

      mGameState.player->update(
         deltaTime,      // delta time / fixed timestep
         nullptr,        // player
         nullptr, // map
         nullptr,        // collidable entities
         0               // col. entity count
      );

      mGameState.hook->update(
         deltaTime,
         mGameState.player,
         nullptr,
         nullptr,
         0
      );

      for(int i = 0; i < 15; i++){
         AIState state = mGameState.arrows[i]->getAIState();
         if(state != IDLE){
            if(state == HIT){
               PlaySound(mGameState.hit);
               combo++;
               mGameState.arrows[i]->setAIState(IDLE);
               float tempScore = mGameState.arrows[i]->getScoreValue();
               score += (int) ((100.0f * tempScore) * (((float) combo) / 100.0f));
               if(tempScore >= 1.0f){
                  status = 1;
               }
               else if(tempScore >= 0.75f){
                  status = 2;
               }
               else{
                  status = 3;
               }
            }
            else if(state == MISS){
               combo = 0;
               mGameState.arrows[i]->setAIState(IDLE);
               mGameState.player->damagePlayer();
            }
            else{
               mGameState.arrows[i]->update(
                  deltaTime,
                  mGameState.player,
                  nullptr,
                  mGameState.hook,
                  1
               );
            }
         }
      }




      //Vector2 currentPlayerPosition = { mGameState.player->getPosition().x, mOrigin.y };

      float interval = GetMusicTimePlayed(mGameState.bgm) /
      ((60.0f / (((float) mGameState.bpm) * mGameState.pulseStep)));
      //printf("%f\n", interval);
      if(floor(interval) != lastInterval){
         mGameState.player->setScale({((float)mGameState.player->getScale().x) *1.15f, ((float)mGameState.player->getScale().y) *1.15f});
         //PlaySound(mGameState.hit);
         //printf("HIT\n");
         lastInterval = floor(interval);
         pointer++;
         if(pointer <= 630){
            for(int i = 0; i < 4; i++){
               if(mTest[pointer * 4 + i] == 1){
                  Entity *point = nullptr;
                  for(int i = 0; i < 15; i++){
                     if(mGameState.arrows[i]->getAIState() == IDLE){
                        point = mGameState.arrows[i];
                        break;
                     }
                  }
                  if(i == 0){
                     point->setPosition({0.0f, mOrigin.y + 80.0f});
                     point->setAIState(LEFT);
                     point->setAngle(90.0f);
                     point->setNextPos({0.0f, mOrigin.y  + 80.0f});
                  }
                  else if(i == 1){
                     point->setPosition({boundaries.x, mOrigin.y  + 80.0f});
                     point->setAIState(RIGHT);
                     point->setAngle(-90.0f);
                     point->setNextPos({boundaries.x, mOrigin.y  + 80.0f});
                  }
                  else if(i == 2){
                     point->setPosition({mOrigin.x, 32.0f});
                     point->setAIState(UP);
                     point->setAngle(180.0f);
                     point->setNextPos({mOrigin.x, 32.0f});
                  }
                  else if(i == 3){
                     point->setPosition({mOrigin.x, boundaries.y + 80.0f});
                     point->setAIState(DOWN);
                     point->setAngle(0.0f);
                     point->setNextPos({mOrigin.x, boundaries.y + 128.0f});
                  }
                  point->setArrowPos(0);
               }
            }
         }
         else{
            finished = true;
         }
            for(int i = 0; i < 15; i++){
               if(mGameState.arrows[i]->getAIState() != IDLE){
                  AIState state = mGameState.arrows[i]->getAIState();
                  Vector2 lastPos = mGameState.arrows[i]->getNextPos();
                  if(state == LEFT){
                     mGameState.arrows[i]->setNextPos({lastPos.x + 96.0f, lastPos.y});
                  }
                  else if(state == RIGHT){
                     mGameState.arrows[i]->setNextPos({lastPos.x - 96.0f, lastPos.y});
                  }
                  else if(state == UP){
                     mGameState.arrows[i]->setNextPos({lastPos.x, lastPos.y + 48.0f});
                  }
                  else if(state == DOWN){
                     mGameState.arrows[i]->setNextPos({lastPos.x, lastPos.y - 48.0f});
                  }
                  mGameState.arrows[i]->setArrowPos(mGameState.arrows[i]->getArrowPos() + 1);
               }
            }
         }

      Vector2 forceCamera = {mGameState.player->getPosition().x, mGameState.player->getPosition().y};
      if(mGameState.player->getShake() <= 25){
         forceCamera = {forceCamera.x + ((float)((rand()%201)-100) / 10.0f), forceCamera.y + ((float)((rand()%201)-100) / 10.0f)};
         mGameState.player->incrementShake();
      }
      else{
         mGameState.player->removeDamage();
      }
      panCamera(&mGameState.camera, &forceCamera);
   }
   else{

   }
}

void LevelA::renderVideo(){
   ClearBackground(ColorFromHex(mBGColourHexCode));
   if(!finished){
      UpdateMedia(&media);
      Rectangle sourceRec = { 0.0f, 0.0f, static_cast<float>(media.videoTexture.width), static_cast<float>(media.videoTexture.height) };
      Rectangle destRec = { static_cast<float>(media.videoTexture.width), static_cast<float>(media.videoTexture.height) + 75.0f, 1000.0f, 600.0f };
      Color tint = { 255, 255, 255, 64 };
      DrawTexturePro(media.videoTexture, sourceRec, destRec, {static_cast<float>(media.videoTexture.width), static_cast<float>(media.videoTexture.height)}, 0.0f, tint);
   }
}

void LevelA::render()
{
   if(!finished){
      //DrawTexture(media.videoTexture, 1000, 300, BLACK);


      mGameState.player->render();
      mGameState.hook->render();
      for(int i = 0; i < 15; i++){
         if(mGameState.arrows[i]->getAIState() != IDLE){
            mGameState.arrows[i]->render();
         }
      }
      switch(status){
         case 0:
            DrawText(TextFormat("Start!"), mOrigin.x+300.0f, mOrigin.y-200.0f, 25, ColorFromHex("#f0f6f0"));
            break;
         case 1:
            DrawText(TextFormat("Perfect!"), mOrigin.x+300.0f, mOrigin.y-200.0f, 25, ColorFromHex("#f0f6f0"));
            break;
         case 2:
            DrawText(TextFormat("Great!"), mOrigin.x+300.0f, mOrigin.y-200.0f, 25, ColorFromHex("#f0f6f0"));
            break;
         default:
            DrawText(TextFormat("Good!"), mOrigin.x+300.0f, mOrigin.y-200.0f, 25, ColorFromHex("#f0f6f0"));
      }
   }
   else{
      DrawText(TextFormat("That's All Folks!"), mOrigin.x-((float) MeasureText("That's All Folks!", 50) / 2.0f), mOrigin.y, 50, ColorFromHex("#f0f6f0"));
   }
   DrawText(TextFormat("Combo: %03i", combo), mOrigin.x-((float) MeasureText("Combo: 000", 25) / 2.0f), mOrigin.y+300.0f, 25, ColorFromHex("#f0f6f0"));
   DrawText(TextFormat("Score: %08i", score), mOrigin.x-((float) MeasureText("Combo: 00000000", 25) / 2.0f), mOrigin.y+325.0f, 25, ColorFromHex("#f0f6f0"));
}

void LevelA::shutdown()
{
   printf("Starting LEVEL_A shutdown...\n");
   delete mGameState.player;
   printf("- Deleted player\n");
   // delete mGameState.map;
   delete mGameState.hook;
   printf("- Deleted hook\n");
   for(int i = 0; i < 15; i++){
      delete mGameState.arrows[i];
      printf("- Deleted arrow %d\n", i);

   }

   UnloadMusicStream(mGameState.bgm);
   printf("- Unloaded music\n");
   UnloadSound(mGameState.hit);
   UnloadMedia(&media);
   printf("- Unloaded sound\n");
   printf("Ending LEVEL_A shutdown...\n");
}