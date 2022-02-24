#pragma once
#include "camera.h"

class CameraManager
{
public:
    CameraManager(GraphicsPipeline& graphics, Player* player, std::vector<int> cameraType);
    ~CameraManager(){};

    void Initialize(GraphicsPipeline& graphics, Player* player);
    void Update(float elapsedTime, Player* player);

    void CalcViewProjection(GraphicsPipeline& graphics);

    camera* GetCamera(int i) { return cameras[i].get(); }
private:

    std::unique_ptr<camera> cameras[2]{};

    std::vector<int>activeCamera{};
public:
    enum CameraTypes
    {
        Title = 0,
        Game,
    };
};