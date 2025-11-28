#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *player;
    Entity *hook;
    Entity **arrows;
    Map *map;

    int bpm;
    float steps;
    float pulseStep;

    Music bgm;
    Sound hit;

    Camera2D camera;

    int nextSceneID;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    Vector2 boundaries;
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode, Vector2 boundaries);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void renderVideo() = 0;
    virtual void shutdown() = 0;
    
    GameState   getState()           const { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
    void setPulseStep(int state){if(state == 0)mGameState.pulseStep = 1.0f; else mGameState.pulseStep = 0.0f;}
    void setPulseStepThree(){mGameState.pulseStep = 3.0f;}
    void setNextScene(int n){mGameState.nextSceneID = n;}
};

#endif