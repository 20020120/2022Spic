#pragma once

#include"skinned_mesh.h"
#include"graphics_pipeline.h"
#include<memory>
//****************************************************************
// 
// íeÇÃäÓíÍÉNÉâÉX 
// 
//****************************************************************
class BaseBullet
{
public:
    //****************************************************************
    // 
    // ä÷êî
    // 
    //****************************************************************
    BaseBullet(GraphicsPipeline& Graphics_, const char* FileName_);
    virtual ~BaseBullet() = default;

    virtual void fUpdate(float elapsedTime_);
    virtual void fRender(GraphicsPipeline& Graphics_);
private:
    //****************************************************************
    // 
    // ïœêî
    // 
    //****************************************************************
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };
    DirectX::XMFLOAT3 mPosition{};
    DirectX::XMFLOAT3 mScale{};
    DirectX::XMFLOAT4 mOrientation{};
    bool mIsAlive{}; // íeÇÃê∂ë∂îªíË
};
