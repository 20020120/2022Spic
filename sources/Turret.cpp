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
    mpModel->update_animation(elapsedTime_);
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

//****************************************************************
// 
// 第二砲身
// 
//****************************************************************
LastBoss::SecondGun::SecondGun(GraphicsPipeline& Graphics_)
{
    // モデルの初期化
    mpModel = std::make_unique<SkinnedMesh>(Graphics_.get_device().Get(),
        "./resources/Models/Enemy/boss_secondarygun.fbx");
}

void LastBoss::SecondGun::fUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mpModel->update_animation(elapsedTime_);
}

void LastBoss::SecondGun::fRender(GraphicsPipeline& graphics_, const DirectX::XMFLOAT4X4& ParentWorld_,
    const DirectX::XMFLOAT3& Position_)
{
    constexpr float scale = 1.0f;

    const auto transMat = DirectX::XMMatrixTranslation(Position_.x,
        Position_.y, Position_.z);
    const auto scaleMat = DirectX::XMMatrixScaling(scale, scale, scale);
    const auto rotMat = DirectX::XMMatrixIdentity();


    const DirectX::XMMATRIX matrix =
        DirectX::XMLoadFloat4x4(&ParentWorld_) * scaleMat * rotMat * transMat;
    DirectX::XMFLOAT4X4 trans{};
    DirectX::XMStoreFloat4x4(&trans, matrix);

    mpModel->render(graphics_.get_dc().Get(), SecondGun::mAnimPara,
        trans, { 1.0f,1.0f,1.0f,1.0f }, 1.0f);
}
