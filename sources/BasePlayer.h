#pragma once
#include<memory>
#include<DirectXMath.h>
#include"graphics_pipeline.h"
#include"skinned_mesh.h"
#include "practical_entities.h"
#include"SkyDome.h"

class BasePlayer :public PracticalEntities
{
public:
    BasePlayer() {}
    virtual ~BasePlayer() {}
protected:
    virtual  void Initialize() = 0;
    virtual  void Update(float elapsed_time, GraphicsPipeline& graphics,SkyDome* sky_dome) = 0;
    virtual void Render(GraphicsPipeline& graphics, float elapsed_time) = 0;
protected:
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT4 orientation{ 0,0,0,1.0f };
    DirectX::XMFLOAT3 scale{ 1.0f,1.0f,1.0f };
    float threshold;
    float threshold_mesh;
    float glow_time;
    DirectX::XMFLOAT4 emissive_color{ 1.0f,1.0f,1.0f,3.0f };
protected:
    std::shared_ptr<SkinnedMesh> model{ nullptr };
};



