#include"NonerEnemy.h"

//****************************************************************
// 
// 移動だけのザコ敵 
// 
//****************************************************************

NoneEnemy::NoneEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 Point_, int UniqueId_)
    :BaseEnemy(pDevice_, UniqueId_, "./resources/Models/Enemy/character_5.fbx")
{
    //初期化
    mPosition = Point_;
    mScale = { 0.03f,0.03f,0.03f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };

    // パラメーターを初期化
    mParam.mAttackPower = 0;
    mParam.mAttackSpeed = 0.0f;
    mParam.mHitPoint = 10;
    mParam.mMoveSpeed = static_cast<float>((rand() % 20) + 10.0f);

    // 距離を初期化
    mDistance = static_cast<float>((rand() % 20) + 10.0f);

    
}

void NoneEnemy::fInitialize()
{
   
}

 void NoneEnemy::fUpdate(float elapsedTime_)
{
   
}

 void NoneEnemy::fSetCapsulePoint()
{
    throw std::logic_error("Not implemented");
}

 void NoneEnemy::fMove(float elapsedTime_)
{
    throw std::logic_error("Not implemented");
}

void NoneEnemy::fRegisterFunctions()
{
    InitFunc ini = [=]()->void
    {
        fStartInit();
    };
    UpdateFunc up = [=](float elapsedTIme_)->void
    {
        fStartUpdate(elapsedTIme_);
    };

    FunctionTuple tuple = std::make_tuple(ini, up);
    mFunctionMap.insert(std::make_pair(State::Start, tuple));

}

void NoneEnemy::fStartInit()
{
    // 待機時間を設定
    mTimerComponent.StartTimer(5.0f);
}

void NoneEnemy::fStartUpdate(float elapsedTime_)
{
    if (mTimerComponent.fGetOver())
    {
        fChangeState(State::Chase);
    }

}

void NoneEnemy::fChaseInit()
{
    throw std::logic_error("Not implemented");
}

void NoneEnemy::fChaseUpdate(float elapsedTime_)
{
    throw std::logic_error("Not implemented");
}

void NoneEnemy::fIntimidationInit()
{
    throw std::logic_error("Not implemented");
}

void NoneEnemy::fIntimidationUpdate(float elapsedTime_)
{
    throw std::logic_error("Not implemented");
}
