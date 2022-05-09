//****************************************************************
// 
// タレットの実装
// 
//****************************************************************

#include <utility>

#include"LastBoss.h"
LastBoss::Turret::Turret(GraphicsPipeline& Graphics_)
{
   // モデルの初期化
    mpModel = std::make_unique<SkinnedMesh>(Graphics_.get_device().Get(),
        "./resources/Models/Enemy/boss_turret.fbx");

}

void LastBoss::Turret::fUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}

void LastBoss::Turret::fRender(GraphicsPipeline& graphics_, 
    const DirectX::XMFLOAT4X4& ParentWorld_,
    const DirectX::XMFLOAT3& Position_)
{
    const auto transMat = DirectX::XMMatrixTranslation(Position_.x,
    Position_.y,Position_.z);

    const DirectX::XMMATRIX matrix =
        DirectX::XMLoadFloat4x4(&ParentWorld_) * transMat;
    DirectX::XMFLOAT4X4 trans{};
    DirectX::XMStoreFloat4x4(&trans, matrix);
    mpModel->render(graphics_.get_dc().Get(), Turret::mAnimPara, 
        trans, { 1.0f,1.0f,1.0f,1.0f },1.0f);
}