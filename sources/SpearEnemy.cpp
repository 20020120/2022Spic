#include"SpearEnemy.h"
#include"Operators.h"
SpearEnemy::SpearEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_,
    ParamGetFunction Func_)
    :BaseEnemy(graphics_,UniqueId_, "./resources/Models/Enemy/enemy_spear.fbx")
{
   // 姿勢を初期化
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.05f,0.05f,0.05f };
    mParam.mHitPoint = 1;
    fRegisterFunctions();
    fGetParam(this, Func_);

    //スラスターエフェクト
    mVernier_effect = std::make_unique<Effect>(graphics_, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\sluster_enemy2.efk");
    mVernier_effect->play(effect_manager->get_effekseer_manager(), mPosition);
    // ボーンを初期化
    mVernierBone = mpSkinnedMesh->get_bone_by_name("burner_back_center_fire");
}


void SpearEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fUpdateBase(elapsedTime_, Graphics_);
    fSetEffectPosition();
}

void SpearEnemy::fStopEffect()
{
    mVernier_effect->stop(effect_manager->get_effekseer_manager());
}

void SpearEnemy::fSetEffectPosition()
{
    //--------------------<バーニアのの位置を決定する>--------------------//
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    DirectX::XMFLOAT4X4 q{};


    // ボーンの名前から位置と上ベクトルを取得
    mpSkinnedMesh->fech_by_bone(mAnimPara,
        Math::calc_world_matrix(mScale, mOrientation, mPosition),
        mVernierBone, position, up, q);

    mVernier_effect->set_position(effect_manager->get_effekseer_manager(), position);
    DirectX::XMFLOAT4X4 corfinate_mat = Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP);
    /* DirectX::XMMATRIX R_MAT = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&rotate_mat));
    DirectX::XMFLOAT4X4 r_mat;
    DirectX::XMStoreFloat4x4(&r_mat, R_MAT);*/
    auto transformQuaternionToRotMat = [&](DirectX::XMFLOAT4X4& q,
        float qx, float qy, float qz, float qw)
    {
        q._11 = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
        q._12 = 2.0f * qx * qy + 2.0f * qw * qz;
        q._13 = 2.0f * qx * qz - 2.0f * qw * qy;

        q._21 = 2.0f * qx * qy - 2.0f * qw * qz;
        q._22 = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
        q._23 = 2.0f * qy * qz + 2.0f * qw * qx;

        q._31 = 2.0f * qx * qz + 2.0f * qw * qy;
        q._32 = 2.0f * qy * qz - 2.0f * qw * qx;
        q._33 = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;
    };
    DirectX::XMFLOAT4X4 r_mat;

    transformQuaternionToRotMat(r_mat, mOrientation.x, mOrientation.y, mOrientation.z, mOrientation.w);
    static float ang = 0;
    //mVernier_effect->set_rotationY(effect_manager->get_effekseer_manager(), DirectX::XMConvertToRadians(ang));
    mVernier_effect->set_posture(effect_manager->get_effekseer_manager(), r_mat, ang);
}

void SpearEnemy::fRegisterFunctions()
{
    // 関数をステートマシンに登録
    {
        InitFunc ini = [=]()->void
        {
            fIdleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fIdleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Idle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fStartInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fStartUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Start, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fMoveInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fMoveUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Move, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fThrustBeginInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fThrustBeginUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ThrustBegin, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fThrustMiddleInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fThrustMiddleUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ThrustMiddle, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fThrustEndInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fThrustEndUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ThrustEnd, tuple));
    }
    fChangeState(DivedState::Start);
}

//--------------------<開始時>--------------------//
void SpearEnemy::fStartInit()
{
    
}

void SpearEnemy::fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mDissolveThreshold -= elapsedTime_;
    if (mDissolveThreshold <= 0.0f)
    {
        fChangeState(DivedState::Idle);
    }
}

//--------------------<待機>--------------------//
void SpearEnemy::fIdleInit()
{
    // アニメーションを再生
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::idle, true);
    
}
void SpearEnemy::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTime += elapsedTime_;
    if(mWaitTime>=mIdleSec)
    {
        fChangeState(DivedState::Move);
    }
}
//--------------------<移動>--------------------//
void SpearEnemy::fMoveInit()
{
    mpSkinnedMesh->play_animation(mAnimPara,AnimationName::walk, true);
    // 突進先の座標を初期化
    mThrustTarget = mPlayerPosition;

}
void SpearEnemy::fMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<プレイヤーの方向に回転>--------------------//
    fTurnToTarget(elapsedTime_, mPlayerPosition,10.0f);
    // プレイヤーと突進方向のなす角が一定以下になったら突進する
    auto v1 = mThrustTarget- mPosition;
    auto v2 = forward;
    v1 = Math::Normalize(v1);
    v2 = Math::Normalize(v2);
    float dot = Math::Dot(v1, v2);
    dot = acosf(dot);
    dot = DirectX::XMConvertToDegrees(dot);

    // 一定距離になったら突進モーションに入る
    if(mLengthFromPlayer<=mAttackLength&&
        fabs(dot)<=mThrustDegree)
    {
        fChangeState(DivedState::ThrustBegin);
    }
    else
    {
        //--------------------<プレイヤーのいる向きに移動>--------------------//
        auto moveVec = mPlayerPosition - mPosition;
        moveVec = Math::Normalize(moveVec);
        velocity = moveVec * mMoveSpeed;
        mPosition += velocity * elapsedTime_;
    }
}
//--------------------<突進準備>--------------------//
void SpearEnemy::fThrustBeginInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_idle);
    mWaitTime = 0.0f;
}
void SpearEnemy::fThrustBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTime += elapsedTime_;
   if(mWaitTime>=mThrustBeginSec)
   {
       fChangeState(DivedState::ThrustMiddle);
   }
}
//--------------------<槍を突き出すアニメーション>--------------------//
void SpearEnemy::fThrustMiddleInit()
{
    // 突き中のアニメーションを再生
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_up);
    mWaitTime = 0.0f;
}

void SpearEnemy::fThrustMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTime += elapsedTime_;
    if(mWaitTime >= mThrustMiddleSec)
    {
        fChangeState(DivedState::ThrustEnd);
    }
}
//--------------------<突進中>--------------------//
void SpearEnemy::fThrustEndInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::attack_down, true);
    mWaitTime = 0.0f;
}

void SpearEnemy::fThrustEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // 正面方向に突撃する
    const auto V = forward * mThrustSpeed * elapsedTime_;
    mPosition += V;

    // 突進中はプレイヤーの方向に軽くホーミングする
    fTurnToTarget(elapsedTime_, mPlayerPosition, 0.7f);

    mWaitTime += elapsedTime_;
    if(mWaitTime>=mThrustEndSec)
    {
        fChangeState(DivedState::Idle);
    }
}

void SpearEnemy::fDamageInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDamageUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDieInit()
{
    throw std::logic_error("Not implemented");
}

void SpearEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    throw std::logic_error("Not implemented");
}

inline SpearEnemy::~SpearEnemy()
{
    mVernier_effect->stop(effect_manager->get_effekseer_manager());
}

void SpearEnemy::fInitialize()
{
    
}
