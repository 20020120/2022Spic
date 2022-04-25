#pragma once
#include"BaseEnemy.h"
#include "EnemiesEditor.h"
#include<memory>

#include "Common.h"
class Turret
{
public:
    Turret(ID3D11Device* pDevice_);
    void fUpdate(float elapsedTime_);
    void fRender(GraphicsPipeline& Graphics_);
    void fSetParentMat(const DirectX::XMFLOAT4X4& Mat_);
private:
    std::unique_ptr<SkinnedMesh> mpTurret{ nullptr };
    DirectX::XMFLOAT3 mPosition{0.0f,0.0f,0.0f};
    DirectX::XMFLOAT4 mOrientation{0.0f,0.0f,0.0f,1.0f};
    DirectX::XMFLOAT3 mScale{ 1.0f,1.0f,1.0f };
    DirectX::XMFLOAT4X4 mParentMatrix{};
    SkinnedMesh::anim_Parameters mAnimPara;
};

class LastBoss final :public BaseEnemy
{
public:
    LastBoss(GraphicsPipeline& graphics_, int UniqueId_,
        DirectX::XMFLOAT3 EmitterPoint_/*スポーン位置*/,
        ParamGetFunction Func_,
        AddBulletFunc AddBulletFunc_);
    ~LastBoss() override = default;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fInitialize() override;
    void fStopEffect() override;
    void fRender(GraphicsPipeline& graphics_) override;
private:
    std::unique_ptr<Turret> mpTurret{ nullptr };

};
