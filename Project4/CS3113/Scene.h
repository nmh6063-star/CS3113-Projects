#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *mimi;
    Entity *misc1;
    Entity *misc2;
    Entity *heart1;
    Entity *heart2;
    Entity *heart3;  
    Map *map;

    int lives;
    int screenWidth;
    int screenHeight;

    Music bgm;
    Sound jumpSound;
    Sound deathSound;
    Sound doorSound;

    Camera2D camera;

    int nextSceneID;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    GameState   getState()           const { return mGameState; }
    void resetNextScene() {mGameState.nextSceneID = 0;}
    void setLives(int l){mGameState.lives = l;}
    void setScreenWidth(int w){mGameState.screenWidth = w;}
    void setScreenHeight(int h){mGameState.screenHeight = h;}
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif