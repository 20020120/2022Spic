#include"SwordEnemy.h"
#include"Operators.h"
SwordEnemy::SwordEnemy(GraphicsPipeline& Graphics_, 
    const DirectX::XMFLOAT3& EmitterPoint_,
    EnemyParamPack ParamPack_)
        :BaseEnemy(Graphics_, 
                  "./resources/Models/Enemy/enemy_sword.fbx",
                  ParamPack_,
                  EmitterPoint_)
{
    SwordEnemy::fRegisterFunctions();
    // ボーンを初期化
    mSwordBone = mpModel->get_bone_by_name("hand_r_joint");
}

SwordEnemy::SwordEnemy(GraphicsPipeline& Graphics_)
    :BaseEnemy(Graphics_, "./resources/Models/Enemy/enemy_sword.fbx")
{}

void SwordEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fBaseUpdate(elapsedTime_,Graphics_);
    fUpdateAttackCapsule(); // 攻撃用のカプセル位置を更新
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

void SwordEnemy::fUpdateAttackCapsule()
{
    //--------------------<剣のカプセルの位置を決定する>--------------------//
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    // ボーンの名前から位置と上ベクトルを取得
    mpModel->fech_by_bone(mAnimPara,
        Math::calc_world_matrix(mScale, mOrientation, mPosition),
        mSwordBone, position, up);
    up = Math::Normalize(up);

    mAttackCapsule.mTop = position + up * 5.0f;
    mAttackCapsule.mBottom = position + up * 1.0f;
}


void SwordEnemy::fSpawnInit()
{
    mpModel->play_animation(mAnimPara,AnimationName::idle, true);
    // 汎用タイマーを初期化
    mWaitTimer = 0.0f;
}

void SwordEnemy::fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    mDissolve -= elapsedTime_;
    // 一定時間経過で移動に遷移
    if(mWaitTimer>=mSpawnDelaySec)
    {
        fChangeState(DivedState::Move);
    }
}

void SwordEnemy::fWalkInit()
{
    // アニメーションを再生
    mpModel->play_animation(mAnimPara,AnimationName::walk,true);
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
    mpModel->play_animation(mAnimPara,AnimationName::attack_idle);
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
    mpModel->play_animation(mAnimPara, AnimationName::attack_up, false, false);
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
    mpModel->play_animation(mAnimPara, AnimationName::attack_down, false, false);
    mWaitTimer = 0.0f;
    fSetAttack(true);
}
void SwordEnemy::fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    if(mWaitTimer>=mAttackDownSec)
    {
        fChangeState(DivedState::Start);
        fSetAttack(false);
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
