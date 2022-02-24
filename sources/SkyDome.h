#pragma once
#include<DirectXMath.h>
#include"skinned_mesh.h"
#include"practical_entities.h"
#include"graphics_pipeline.h"
#include"collision.h"
class SkyDome : PracticalEntities
{
public:
    SkyDome(GraphicsPipeline& grahics);
    ~SkyDome();
public:
    void Render(GraphicsPipeline& grahics, float elapsed_time);
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
private:
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 scale{ 0.058f , 0.058f , 0.058f };
    DirectX::XMFLOAT3 angle{};
public:
    void SetPosition(DirectX::XMFLOAT3 p) { position = p; }
    void SetScale(DirectX::XMFLOAT3 p) { scale = p; }
    void SetAngle(DirectX::XMFLOAT3 p) { angle = p; }
private:
    std::shared_ptr<SkinnedMesh> model;

};