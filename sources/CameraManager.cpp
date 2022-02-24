#include "CameraManager.h"

CameraManager::CameraManager(GraphicsPipeline& graphics, Player* player, std::vector<int> cameraType)
{
    for(auto it : cameraType)
    {
        cameras[it] = std::make_unique<camera>(it, graphics, player);
    }
    activeCamera = cameraType;
}

void CameraManager::Initialize(GraphicsPipeline& graphics, Player* player)
{
    for (auto& it : activeCamera)
    {
        cameras[it]->Initialize(graphics, player);
    }
}

void CameraManager::Update(float elapsedTime, Player* player)
{
    for (auto& it : activeCamera)
    {
        cameras[it]->Update(elapsedTime, player);
    }
}