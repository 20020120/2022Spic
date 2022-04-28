#include "FrontShieldEnemy.h"
#include "Operators.h"
//    //ダメージを受けたときに正面からの攻撃なら,シールドが防いでダメージを減少させる
// int FrontShieldEnemy::fJudge_Front_Attacked(int damage_) const
//{
//    //プレイヤーとの位置関係を判定し、攻撃されたときに自分の視界45度いないなら正面と判定する
//    DirectX::XMVECTOR EtoP_vec = Math::calc_vector_AtoB_normalize(mPosition, mPlayerPosition);
//    DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
//    DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(EtoP_vec, Forward);
//    float dot;
//    DirectX::XMStoreFloat(&dot, Dot);
//    dot = acosf(dot);
//    if (DirectX::XMConvertToDegrees(dot) < 45.0f)
//    {
//        //正面からの攻撃ならダメージを3分の1にする
//        return damage_ /= 3;
//    }
//
//	//そうでないなら、受けたダメージをそのままにする
//    return damage_;
//
//}
//
ShieldEnemy::ShieldEnemy(GraphicsPipeline& Graphics_,
    const DirectX::XMFLOAT3& EmitterPoint_,
    const EnemyParamPack& ParamPack_)
    :BaseEnemy(Graphics_,
        "./resources/Models/Enemy/SwordEnemy.fbx",
        ParamPack_,
        EmitterPoint_)
{
    ShieldEnemy::fRegisterFunctions();
    // ボーンを初期化
    mSwordBone = mpModel->get_bone_by_name("hand_r_joint");
    mScale = { 0.05f,0.05f,0.05f };

}

ShieldEnemy::ShieldEnemy(GraphicsPipeline& Graphics_)
    :BaseEnemy(Graphics_, "./resources/Models/Enemy/SwordEnemy.fbx")
{}

void ShieldEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fBaseUpdate(elapsedTime_, Graphics_);
    fUpdateAttackCapsule(); // 攻撃用のカプセル位置を更新
}

void ShieldEnemy::fRegisterFunctions()
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
            fShieldReadyInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShieldReadyUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ShieldReady, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShieldAttackInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShieldAttackUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ShieldAttack, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShieldInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShieldUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Shield, tuple));
    }
    
    {
        InitFunc ini = [=]()->void
        {
            fStunInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fStunUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Stun, tuple));
    }
    fChangeState(DivedState::Start);
}


void ShieldEnemy::fUpdateAttackCapsule()
{
}
//
//void ShieldEnemy::fDamaged(int Damage_, float InvincibleTime_)
//{
//
//
//    if (mInvincibleTime <= 0.0f)
//    {
//        mCurrentHitPoint -= Damage_;
//        mInvincibleTime = InvincibleTime_;
//    }
//    if (mCurrentHitPoint <= 0)
//    {
//        fDie();
//    }
//}

void ShieldEnemy::fSpawnInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::idle, true);
    // 汎用タイマーを初期化
    mWaitTimer = 0.0f;
}

void ShieldEnemy::fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    mDissolve -= elapsedTime_;
    // 一定時間経過で移動に遷移
    if (mWaitTimer >= mSpawnDelaySec)
    {
        fChangeState(DivedState::Move);
    }
}

void ShieldEnemy::fWalkInit()
{
    // アニメーションを再生
    mpModel->play_animation(mAnimPara, AnimationName::walk, true);
    mWaitTimer = 0.0f;
}

void ShieldEnemy::fWalkUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<プレイヤーのいる向きに移動>--------------------//

    fMoveFront(elapsedTime_, 10.0f);
    //--------------------<プレイヤーの方向に回転>--------------------//
    fTurnToPlayer(elapsedTime_, 2.0f);

    // プレイヤーとの距離が一定以下になったら
    if (mAttackRange >= Math::Length(mPlayerPosition - mPosition))
    {
        fChangeState(DivedState::ShieldReady);
    }
}

void ShieldEnemy::fShieldReadyInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::shield_ready);
    mWaitTimer = 0.0f;
}

void ShieldEnemy::fShieldReadyUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

    // タイマーを加算
    mWaitTimer += elapsedTime_;
    //シールド構え時間が一定時間たったら
    if (mWaitTimer >= mShieldReadySec * mAnimationSpeed)
    {
        fChangeState(DivedState::Idle);
    }
    //構え中に正面から攻撃されたら

    //構え中に後ろから攻撃されたら
}

void ShieldEnemy::fShieldAttackInit()
{
}

void ShieldEnemy::fShieldAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}

void ShieldEnemy::fShieldInit()
{
}

void ShieldEnemy::fShieldUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}

void ShieldEnemy::fEscapeInit()
{
}

void ShieldEnemy::fEscapeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}

void ShieldEnemy::fStunInit()
{
}

void ShieldEnemy::fStunUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}

void ShieldEnemy::fDieInit()
{
}

void ShieldEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}
