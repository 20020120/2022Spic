#include"NormalEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
//****************************************************************
// 
// テストとサンプルを兼ねた敵の派生クラス 
// 
//****************************************************************
NormalEnemy::NormalEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(pDevice_, "./resources/Models/Enemy/character_3.fbx")
{
    // 位置を初期化
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.1f,0.1f,0.1f };
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

void NormalEnemy::fIdleInit()
{

}


void NormalEnemy::fIdleUpdate(float elapsedTime_)
{
    mPosition.z += 10.0f * elapsedTime_;
}
