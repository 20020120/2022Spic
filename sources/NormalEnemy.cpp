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
    mPosition = { 10,0, 10 };
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
    fUpdateBase(elapsedTime_);


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
    mFunctionMap.insert(std::make_pair(IDLE, tuple));

    Ini = [=]()->void
    {
        fMoveInit();
    };
    Up = [=](float elapsedTime_)->void
    {
        fmoveUpdate(elapsedTime_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(MOVE, tuple));

    Ini = [=]()->void
    {
        fAttackInit();
    };
    Up = [=](float elapsedTime_)->void
    {
        fAttackUpdate(elapsedTime_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(MOVE, tuple));

    fChangeState(IDLE);

}
void NormalEnemy::fParamInitialize()
{
    mParam.mHitPoint = 10;      // 体力
    mParam.mAttackPower = 10;   // 攻撃力
    mParam.mMoveSpeed = 10;   // 移動速度
    mParam.mAttackSpeed = 2; // 攻撃間隔
    
}


void NormalEnemy::fIdleInit()
{
}


void NormalEnemy::fIdleUpdate(float elapsedTime_)
{
	if(fTurnToPlayer(elapsedTime_, 10))
    {
        fChangeState(MOVE);
    }
}

void NormalEnemy::fMoveInit()
{

}

void NormalEnemy::fmoveUpdate(float elapsedTime_)
{
    velocity.x += forward.x * move_speed * elapsedTime_;
    velocity.y += forward.y * move_speed * elapsedTime_;
    velocity.z += forward.z * move_speed * elapsedTime_;
    if (!fTurnToPlayer(elapsedTime_,5))
    {
        fChangeState(IDLE);
    }
}

void NormalEnemy::fAttackInit()
{
}

void NormalEnemy::fAttackUpdate(float elapsedTime_)
{
}
