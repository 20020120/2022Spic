#pragma once
#include<DirectXMath.h>
#include"skinned_mesh.h"
#include"practical_entities.h"
#include"graphics_pipeline.h"
class SkyDome : PracticalEntities
{
public:
    SkyDome(GraphicsPipeline& grahics);
    ~SkyDome();
public:
    void Render(GraphicsPipeline& grahics, float elapsed_time);
private:
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 scale{ 0.1f , 0.1f , 0.1f };
    DirectX::XMFLOAT3 angle{};
public:
    void SetPosition(DirectX::XMFLOAT3 p) { position = p; }
    void SetScale(DirectX::XMFLOAT3 p) { scale = p; }
    void SetAngle(DirectX::XMFLOAT3 p) { angle = p; }
private:
    std::shared_ptr<SkinnedMesh> model;

};