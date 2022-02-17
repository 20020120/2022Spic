#include"NormalEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
#include "user.h"
//****************************************************************
// 
// 盾なし通常攻撃の雑魚敵の派生クラス
// 
//****************************************************************
NormalEnemy::NormalEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(pDevice_, "./resources/Models/Enemy/character_4.fbx")
{
    // 位置を初期化
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.1f,0.1f,0.1f };
    //パラメーターの初期化
	fParamInitialize();
    fRegisterFunctions();

}

void NormalEnemy::fInitialize()
{
}

void NormalEnemy::fUpdate(float elapsedTime_)
{
    //--------------------<更新処理>--------------------//
    fUpdateStateMachine(elapsedTime_);

}

void NormalEnemy::fRegisterFunctions()
{
    InitFunc Ini = [=]()->void
    {
        fIdleInit();
    };
    UpdateFunc Up = [=](float elapsedTime_)->void
    {
        fIdleUpdate(elapsedTime_);
    };
    FunctionTuple tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(0, tuple));

    mCurrentTuple = mFunctionMap.at(0);
}

void NormalEnemy::fParamInitialize()
{
    mParam.mHitPoint = 10;      // 体力
    mParam.mAttackPower = 10;   // 攻撃力
    mParam.mMoveSpeed = 10;   // 移動速度
    mParam.mAttackSpeed = 2; // 攻撃間隔

}

void NormalEnemy::fTurnToThePlayer()
{
    using namespace DirectX;
    XMVECTOR P_Pos = XMLoadFloat3(&mPlayerPosition);
    XMVECTOR E_Pos = XMLoadFloat3(&mPosition);
    XMVECTOR Vec = P_Pos - E_Pos;
    XMVECTOR Orientation = XMLoadFloat4(&mOrientation);
    XMVECTOR Forward = XMLoadFloat3(&forward);
    XMVECTOR Dot = XMVector3Dot(Vec, Forward);
}

void NormalEnemy::fIdleInit()
{
}


void NormalEnemy::fIdleUpdate(float elapsedTime_)
{
}

void NormalEnemy::fMoveInit()
{

}

void NormalEnemy::fmoveUpdate(float elapsedTime_)
{

}
