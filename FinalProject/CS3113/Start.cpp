#include "Start.h"

Start::Start()                                      : Scene { {0.0f}, nullptr, {0.0f}   } {}
Start::Start(Vector2 origin, const char *bgHexCode, Vector2 boundaries) : Scene { origin, bgHexCode, boundaries } {}

Start::~Start() { shutdown(); }

void Start::initialise()
{
   mGameState.hit = LoadSound("assets/game/SelectBlip.wav");
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin; // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void Start::update(float deltaTime)
{
   if(lastState != mGameState.pulseStep){
      PlaySound(mGameState.hit);
      lastState = mGameState.pulseStep;
   }
}

void Start::renderVideo(){

}

void Start::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   DrawText(TextFormat("ARROW BEAT"), mOrigin.x-((float) MeasureText("ARROW BEAT", 50) / 2.0f), mOrigin.y-200.0f, 50, ColorFromHex("#f0f6f0"));
   if(mGameState.pulseStep == 0.0f){
      DrawText(TextFormat("- start"), mOrigin.x-((float) MeasureText("- start", 25) / 2.0f), mOrigin.y, 25, ColorFromHex("#ffff00"));
      DrawText(TextFormat("how to play"), mOrigin.x-((float) MeasureText("how to play", 25) / 2.0f), mOrigin.y+ 100.0f, 25, ColorFromHex("#f0f6f0"));
   }
   else if(mGameState.pulseStep == 1.0f){
      DrawText(TextFormat("start"), mOrigin.x-((float) MeasureText("start", 25) / 2.0f), mOrigin.y, 25, ColorFromHex("#f0f6f0"));
      DrawText(TextFormat("- how to play"), mOrigin.x-((float) MeasureText("- how to play", 25) / 2.0f), mOrigin.y+ 100.0f, 25, ColorFromHex("#ffff00"));
   }
   else{
      DrawText(TextFormat("Press WASD to face up, left, down, and up."), mOrigin.x-((float) MeasureText("Press WASD to face up, left, down, and up.", 25) / 2.0f), mOrigin.y, 25, ColorFromHex("#f0f6f0"));
      DrawText(TextFormat("Turn in the direction of an arrow to block it."), mOrigin.x-((float) MeasureText("Turn in the direction of an arrow to block it.", 25) / 2.0f), mOrigin.y+50.0f, 25, ColorFromHex("#f0f6f0"));
      DrawText(TextFormat("The more on beat you block a note, the better your score."), mOrigin.x-((float) MeasureText("The more on beat you block a note, the better your score.", 25) / 2.0f), mOrigin.y+100.0f, 25, ColorFromHex("#f0f6f0"));
      DrawText(TextFormat("Combo increases your score multiplier. Block notes in succession to build it."), mOrigin.x-((float) MeasureText("Combo increases your score multiplier. Block notes in succession to build it.", 25) / 2.0f), mOrigin.y+150.0f, 25, ColorFromHex("#f0f6f0"));
      DrawText(TextFormat("Press enter to return to the menu"), mOrigin.x-((float) MeasureText("Press enter to return to the menu", 25) / 2.0f), mOrigin.y+200.0f, 25, ColorFromHex("#f0f6f0"));
   }

   //mGameState.player->render();
   //mGameState.map->render();
}

void Start::shutdown()
{
   delete mGameState.player;
   delete mGameState.map;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.hit);
   //UnloadSound(mGameState.jumpSound);
}