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

void BaseEnemy::fUpdateStateMachine(float elapsedTime_)
{
    // 中身が何も設定されていなかった場合の挙動は未知数なので気を付けて
   
    // ステートを初期化
    if (!mIsInitialize)
    {
        std::get<0>(mCurrentTuple)();
        mIsInitialize = true;
    }
    // ステートを更新
    std::get<1>(mCurrentTuple)(elapsedTime_);
}


void BaseEnemy::fChangeState(int i)
{
    mCurrentTuple = mFunctionMap.at(i);
    mIsInitialize = false;
}