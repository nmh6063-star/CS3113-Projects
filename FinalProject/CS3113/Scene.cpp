#include "Scene.h"

Scene::Scene() : mOrigin{{}} {}

Scene::Scene(Vector2 origin, const char *bgHexCode, Vector2 boundaries) : mOrigin{origin}, mBGColourHexCode {bgHexCode}, boundaries {boundaries}
{
    ClearBackground(ColorFromHex(bgHexCode));
}