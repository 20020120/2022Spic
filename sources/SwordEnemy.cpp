#include"SwordEnemy.h"
#include"Operators.h"
SwordEnemy::SwordEnemy(GraphicsPipeline& graphics_, 
    int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_,ParamGetFunction Func_ )
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_sword.fbx")
{
    // 位置の初期化
    mPosition = EmitterPoint_;
    mScale = { 0.05f,0.05f,0.05f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    // ステートマシンを初期化
    SwordEnemy::fRegisterFunctions();
    mParam.mHitPoint = 100;
    fGetParam(this, Func_);

    // 攻撃に関するparameterを初期化
    mAttackInformation.mInvincible_time = { 0.2f };
     // ボーンを初期化
    mSwordBone = mpSkinnedMesh->get_bone_by_name("hand_r_joint");
}

void SwordEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fUpdateBase(elapsedTime_,Graphics_);
    fSetAttackCapsuleCollider(); // 攻撃用のカプセル位置を更新
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
    {
        InitFunc ini = [=]()->void
        {
            fAttackBeginInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fAttackBeginUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::AttackBegin, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fAttackPreActionInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fAttackPreActionUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::AttackMiddle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fAttackEndInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fAttackEndUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::AttackEnd, tuple));
    }

    fChangeState(DivedState::Start);
}

void SwordEnemy::fSetAttackCapsuleCollider()
{
    //--------------------<剣のカプセルの位置を決定する>--------------------//
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    // ボーンの名前から位置と上ベクトルを取得
    mpSkinnedMesh->fech_by_bone(mAnimPara,
        Math::calc_world_matrix(mScale, mOrientation, mPosition),
        mSwordBone, position, up);
    up = Math::Normalize(up);
    
    mAttackCapsuleCollider.mPointA = position + up * 5.0f;
    mAttackCapsuleCollider.mPointB = position + up * 1.0f;
    mAttackCapsuleCollider.mRadius = 2.0f;

}

void SwordEnemy::fSpawnInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::idle, true);
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
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::walk,true);
    mWaitTimer = 0.0f;
}

void SwordEnemy::fWalkUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<プレイヤーのいる向きに移動>--------------------//
    const auto moveVec = mPlayerPosition - mPosition;
    fMoveVelocity(elapsedTime_, moveVec, mMoveSpeed);
    //--------------------<プレイヤーの方向に回転>--------------------//
    fTurnToTarget(elapsedTime_, mPlayerPosition);

    // プレイヤーとの距離が一定以下になったら
    if(mAttackRange >= Math::Length(mPlayerPosition-mPosition))
    {
        fChangeState(DivedState::AttackBegin);
    }
}

//--------------------<剣を振り上げる>--------------------//
void SwordEnemy::fAttackBeginInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::attack_idle);
    mWaitTimer = 0.0f;
}
void SwordEnemy::fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fTurnToTarget(elapsedTime_, mPlayerPosition, 20.0f);

    // タイマーを加算
    mWaitTimer += elapsedTime_;
    if(mWaitTimer>=mAttackBeginTimeSec*mAnimationSpeed)
    {
        fChangeState(DivedState::AttackMiddle);
    }
}
//--------------------<剣を振り下ろす予備動作>--------------------//
void SwordEnemy::fAttackPreActionInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_up, false, false);
    mWaitTimer = 0.0f;
}
void SwordEnemy::fAttackPreActionUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    if (mWaitTimer >= mAttackPreActionTimeSec * mAnimationSpeed)
    {
        fChangeState(DivedState::AttackEnd);
    }
}
//--------------------<攻撃中（当たり判定ON）>--------------------//
void SwordEnemy::fAttackEndInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_down, false, false);
    mWaitTimer = 0.0f;
    mAttackInformation.mIsAttack = true;
}
void SwordEnemy::fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    if(mWaitTimer>=mAttackDownSec)
    {
        fChangeState(DivedState::Start);
        mAttackInformation.mIsAttack = false;
    }
}


void SwordEnemy::fDieInit()
{
    throw std::logic_error("Not implemented");
}

void SwordEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}
