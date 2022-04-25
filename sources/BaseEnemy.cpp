#include"BaseEnemy.h"
#include"resource_manager.h"
BaseEnemy::BaseEnemy(GraphicsPipeline& Graphics_,
                     const char* FileName_,
                     const EnemyParamPack& Param_,
                     const DirectX::XMFLOAT3& EntryPosition_)
{
    mpModel = resource_manager->load_model_resource(Graphics_.get_device().Get(), FileName_);
    mCurrentHitPoint = Param_.MaxHp;
    mBodyCapsule.mRadius = Param_.BodyCapsuleRad;
    mAttackCapsule.mRadius = Param_.AttackCapsuleRad;
    mPosition = EntryPosition_;

    mVernierEffect = std::make_unique<Effect>(Graphics_, 
      effect_manager->get_effekseer_manager(), mkVernierPath);
    mVernierEffect->play(effect_manager->get_effekseer_manager(), mPosition);
}

BaseEnemy::BaseEnemy(GraphicsPipeline& Graphics_, const char* FileName_)
{
    mpModel = resource_manager->load_model_resource(Graphics_.get_device().Get(), FileName_);
}

BaseEnemy::~BaseEnemy()
{
    mVernierEffect->stop(effect_manager->get_effekseer_manager());
}

void BaseEnemy::fBaseUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mInvincibleTime -= elapsedTime_;
    mInvincibleTime = (std::max)(-1.0f, mInvincibleTime);

    fUpdateVernierEffectPos();

}

void BaseEnemy::fRender(GraphicsPipeline& Graphics_) 
{
    const DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    const DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
    mpModel->render(Graphics_.get_dc().Get(), mAnimPara, world, color,mDissolve);
}

void BaseEnemy::fDamaged(int Damage_, float InvincibleTime_)
{
    if(mInvincibleTime<=0.0f)
    {
        mCurrentHitPoint -= Damage_;
        mInvincibleTime = InvincibleTime_;
    }
    if(mCurrentHitPoint<0)
    {
        fDie();
    }
}

void BaseEnemy::fDie()
{
    
}

void BaseEnemy::fUpdateVernierEffectPos()
{
    //--------------------<バーニアのの位置を決定する>--------------------//
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 up{};
    DirectX::XMFLOAT4X4 q{};


    // ボーンの名前から位置と上ベクトルを取得
    mpModel->fech_by_bone(mAnimPara,
        Math::calc_world_matrix(mScale, mOrientation, mPosition),
        mVernierBone, position, up, q);

    mVernierEffect->set_position(effect_manager->get_effekseer_manager(), position);
    DirectX::XMFLOAT4X4 corfinate_mat = Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP);
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
    mVernierEffect->set_posture(effect_manager->get_effekseer_manager(), r_mat, ang);
}

void BaseEnemy::fSetStun(bool Arg_)
{
    mIsStun = Arg_;
}

void BaseEnemy::fSetPlayerPosition(const DirectX::XMFLOAT3& PlayerPosition_)
{
    mPlayerPosition = PlayerPosition_;
}

void BaseEnemy::fSetAttack(bool Arg_)
{
    mIsAttack = Arg_;
}

bool BaseEnemy::fGetAttack() const
{
    return mIsAttack;
}

void BaseEnemy::fChangeState(const char* Tag_)
{
    mCurrentTuple = mFunctionMap.at(Tag_);
}
