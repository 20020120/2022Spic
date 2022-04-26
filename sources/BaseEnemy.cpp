#include"BaseEnemy.h"
#include "collision.h"
#include"resource_manager.h"
#include"Operators.h"
BaseEnemy::BaseEnemy(GraphicsPipeline& Graphics_,
                     const char* FileName_,
                     const EnemyParamPack& Param_,
                     const DirectX::XMFLOAT3& EntryPosition_)
:mPosition(EntryPosition_),mCurrentHitPoint(Param_.MaxHp)
,mMaxHp(Param_.MaxHp),mAttackPower(Param_.AttackPower)
{
    mpModel = resource_manager->load_model_resource(Graphics_.get_device().Get(), FileName_);
    mBodyCapsule.mRadius = Param_.BodyCapsuleRad;
    mAttackCapsule.mRadius = Param_.AttackCapsuleRad;

    mVernierEffect = std::make_unique<Effect>(Graphics_, 
      effect_manager->get_effekseer_manager(), mkVernierPath);
    mVernierEffect->play(effect_manager->get_effekseer_manager(), mPosition);
    mCubeHalfSize = mScale.x * 100.0f;
    mDissolve = 1.0f;
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
    std::get<1>(mCurrentTuple)(elapsedTime_, Graphics_);
    mpModel->update_animation(mAnimPara, elapsedTime_);
}

void BaseEnemy::fRender(GraphicsPipeline& Graphics_) 
{
    Graphics_.set_pipeline_preset(SHADER_TYPES::PBR);
    mDissolve = (std::max)(0.0f, mDissolve);
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
    if (mCurrentHitPoint <= 0)
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

void BaseEnemy::fTurnToPlayer(float elapsedTime_,float RotSpeed_)
{
    // プレイヤーの方向に回転
    constexpr DirectX::XMFLOAT3 up = { 0.001f,1.0f,0.0f };

    // プレイヤーとのベクトル
    const DirectX::XMFLOAT3 vToPlayer = Math::Normalize(mPlayerPosition - mPosition);
    // 自分の正面ベクトル
    const auto front = Math::Normalize(Math::GetFront(mOrientation));
    const float dot = Math::Dot(vToPlayer, front);
    mOrientation = Math::RotQuaternion(mOrientation, up,fabsf(acosf(dot)) * RotSpeed_ * elapsedTime_);

}

void BaseEnemy::fMoveFront(float elapsedTime_, float MoveSpeed_)
{
    // 前方向に進
    const auto velocity = Math::Normalize(Math::GetFront(mOrientation)) * MoveSpeed_;
    mPosition += (velocity * elapsedTime_);
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

const Capsule& BaseEnemy::fGetBodyCapsule() const
{
    return mBodyCapsule;
}

const Capsule& BaseEnemy::fGetAttackCapsule() const
{
    return mAttackCapsule;
}

const DirectX::XMFLOAT3& BaseEnemy::fGetPosition() const
{
    return mPosition;
}

bool BaseEnemy::fGetIsAlive() const
{
    return mCurrentHitPoint > 0;
}

bool BaseEnemy::fComputeAndGetIntoCamera() const
{
    const DirectX::XMFLOAT3 minPoint{
        mPosition.x - mCubeHalfSize,
        mPosition.y - mCubeHalfSize,
        mPosition.z - mCubeHalfSize
    };
    const DirectX::XMFLOAT3 maxPoint{
        mPosition.x + mCubeHalfSize,
        mPosition.y + mCubeHalfSize,
        mPosition.z + mCubeHalfSize
    };

    return Collision::frustum_vs_cuboid(minPoint, maxPoint);
}

int BaseEnemy::fGetAttackPower() const
{
    return mAttackPower;
}

float BaseEnemy::fGetAttackInvTime() const
{
    return mAttackInvTime;
}

float BaseEnemy::fGetLengthFromPlayer() const
{
    return Math::Length(mPlayerPosition - mPosition);
}

float BaseEnemy::fGetPercentHitPoint() const
{
    if (mCurrentHitPoint <= 0)
        return 0.0f;
    else
        return static_cast<float>(mCurrentHitPoint) / static_cast<float>(mMaxHp);
}

bool BaseEnemy::fGetStun() const
{
    return mIsStun;
}


void BaseEnemy::fChangeState(const char* Tag_)
{
    mCurrentTuple = mFunctionMap.at(Tag_);
    std::get<0>(mCurrentTuple)();
}
