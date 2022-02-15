#include "BaseEnemy.h"
#include"user.h"
//****************************************************************
// 
// 敵の基底クラス 
// 
//****************************************************************
BaseEnemy::BaseEnemy(ID3D11Device* pDevice_, const char* ModelName_)
{
    // モデルを初期化
    mpSkinnedMesh = std::make_unique<SkinnedMesh>(pDevice_, ModelName_);
}

BaseEnemy::~BaseEnemy() = default;

void BaseEnemy::fRender(ID3D11DeviceContext* pDeviceContext_) const
{
    // ワールド行列を作成
    const auto worldMatrix = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    mpSkinnedMesh->render(pDeviceContext_, worldMatrix, { 1.0f,1.0f,1.0f,1.0f });
}
