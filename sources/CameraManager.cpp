#include "CameraManager.h"

CameraManager::CameraManager(GraphicsPipeline& graphics, Player* player, std::vector<int> cameraType)
{
    for(auto it : cameraType)
    {
        cameras[it] = std::make_unique<camera>(it, graphics, player);
    }
}

void CameraManager::Initialize()
{
}

void CameraManager::Update(float elapsedTime)
{
    for (auto& it : cameras)
    {
        it->Update(elapsedTime);
    }
}