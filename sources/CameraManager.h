#pragma once
#include "camera.h"

class CameraManager
{
public:
    CameraManager(GraphicsPipeline& graphics, Player* player, std::vector<int> cameraType);
    ~CameraManager();

    void Initialize();
    void Update(float elapsedTime);

    camera* GetCamera(int i) { return cameras[i].get(); }
private:

    std::unique_ptr<camera> cameras[2]{};

public:
    enum CameraTypes
    {
        Title = 0,
        Game,
    };
};