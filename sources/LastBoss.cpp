#include"LastBoss.h"
#include"Operators.h"
Turret::Turret(ID3D11Device* pDevice_)
{
    mpTurret = std::make_unique<SkinnedMesh>(pDevice_, "./resources/Models/Enemy/boss_animation_second.fbx");
}

void Turret::fUpdate(float elapsedTime_)
{
    mpTurret->update_animation(mAnimPara, elapsedTime_);
}

void Turret::fRender(GraphicsPipeline& Graphics_)
{
    const DirectX::XMFLOAT4X4 world=
    Math::calc_world_matrix(mScale, mOrientation, mPosition)* mParentMatrix;
    mpTurret->render(Graphics_.get_dc().Get(), mAnimPara, world, { 1.0f,1.0f,1.0f,1.0f });
}

void Turret::fSetParentMat(const DirectX::XMFLOAT4X4& Mat_)
{
    mParentMatrix = Mat_;
}

LastBoss::LastBoss(GraphicsPipeline& graphics_,
                   int UniqueId_, 
                   DirectX::XMFLOAT3 EmitterPoint_,
                   ParamGetFunction Func_,
                   AddBulletFunc AddBulletFunc_)
:BaseEnemy(graphics_,UniqueId_,
                           "./resources/Models/Enemy/boss_animation_second.fbx")
{
    fGetParam(this, Func_);
    mpTurret = std::make_unique<Turret>(graphics_.get_device().Get());
    mPosition = {};
    mScale = { 0.05f,0.05f,0.05f };
}

void LastBoss::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    mpTurret->fSetParentMat(Math::calc_world_matrix(mScale, mOrientation, mPosition));
    mpTurret->fUpdate(elapsedTime_);
    ImGui::Begin("Boss");
    ImGui::DragFloat3("position", &mPosition.x);
    ImGui::DragFloat3("Scale", &mScale.x);
    ImGui::End();
}

void LastBoss::fInitialize()
{
  
}

void LastBoss::fStopEffect()
{
    
}

void LastBoss::fRender(GraphicsPipeline& graphics)
{
    BaseEnemy::fRender(graphics);
    mpTurret->fRender(graphics);
}
