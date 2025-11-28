#include "Scene.h"

class Start : public Scene {
private:

public:
    float lastState = 0.0f;

    Start();
    Start(Vector2 origin, const char *bgHexCode, Vector2 boundaries);
    ~Start();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void renderVideo() override;
    void shutdown() override;
};