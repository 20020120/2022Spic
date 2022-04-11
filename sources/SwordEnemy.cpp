#include"SwordEnemy.h"
#include"Operators.h"
SwordEnemy::SwordEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_sword.cereal")
{
    // 位置の初期化
    mPosition = EmitterPoint_;
    // ステートマシンを初期化
    SwordEnemy::fRegisterFunctions();
}

void SwordEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fUpdateBase(elapsedTime_,Graphics_);
}

void SwordEnemy::fRegisterFunctions()
{
    // 関数をステートマシンに登録
    {
        InitFunc ini = [=]()->void
        {
            fSpawnInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fSpawnUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Start, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fWalkInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fWalkUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Move, tuple));
    }

    fChangeState(DivedState::Start);
}

void SwordEnemy::fSpawnInit()
{
    mpSkinnedMesh->play_animation(AnimationName::idle, true);
    // 汎用タイマーを初期化
    mWaitTimer = 0.0f;
}

void SwordEnemy::fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;

    // 一定時間経過で移動に遷移
    if(mWaitTimer>=mSpawnDelaySec)
    {
        fChangeState(DivedState::Move);
    }
}

void SwordEnemy::fWalkInit()
{
    // アニメーションを再生
    mpSkinnedMesh->play_animation(AnimationName::walk);
    mWaitTimer = 0.0f;
}

void SwordEnemy::fWalkUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    if(Math::Length(mPlayerPosition-mPosition))
    {
        
    }
}

//--------------------<攻撃はじめ>--------------------//
void SwordEnemy::fAttackBeginInit()
{
    
}
void SwordEnemy::fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}

//--------------------<攻撃中（当たり判定ON）>--------------------//
void SwordEnemy::fAttackInit()
{
    throw std::logic_error("Not implemented");
}
void SwordEnemy::fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

//--------------------<攻撃終わり（後隙)>--------------------//
void SwordEnemy::fAttackEndInit()
{
    
}
void SwordEnemy::fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}


void SwordEnemy::fDieInit()
{
    throw std::logic_error("Not implemented");
}

void SwordEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}
