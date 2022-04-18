#include "BaseEnemy.h"
#include"user.h"
#include"collision.h"
#include"Operators.h"
#include"Effekseer.h"
#include<typeinfo>
//****************************************************************
//
// 敵の基底クラス
//
//****************************************************************
BaseEnemy::BaseEnemy(GraphicsPipeline& graphics_,int UniqueId_, const char* ModelName_)
{
    // モデルを初期化
    mpSkinnedMesh = resource_manager->load_model_resource(graphics_.get_device().Get(), ModelName_,true);
    mUniqueId = UniqueId_;
    mDieEffect = std::make_unique<Effect>(graphics_, effect_manager->get_effekseer_manager(),
        "./resources/Effect/bomb_2.efk");
    mDissolveThreshold = 1.0f;
}

BaseEnemy::~BaseEnemy() = default;

void BaseEnemy::fRender(GraphicsPipeline& graphics_)
{
    graphics_.set_pipeline_preset(SHADER_TYPES::PBR);
    mDissolveThreshold = (std::max)(0.0f, mDissolveThreshold);
    // ワールド行列を作成
    const auto worldMatrix = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    mpSkinnedMesh->render(graphics_.get_dc().Get(), mAnimPara, worldMatrix, { 1.0f,1.0f,1.0f,1.0f },mDissolveThreshold);
}

void BaseEnemy::fGetParam(BaseEnemy* This_, std::function<EnemyData(std::string)> Function_)
{
    // 敵のデータをロード
    const type_info& id = typeid(*This_);
    mData = Function_(id.name());
    mAttackInformation.mDamage = mData.mAttackPower;
    mParam.mAttackSpeed = mData.mAttackSpeed;
    mParam.mHitPoint = mData.mMaxHitPoint;
    mParam.mMoveSpeed = mData.mMoveSpeed;
}

void BaseEnemy::fDieEffect() const
{
    mDieEffect->play(effect_manager->get_effekseer_manager(), mPosition,2.0f);
    hit_stop->damage_hit_stop();
}

void BaseEnemy::fDamaged(int Damage_, float InvinsibleTime_)
{
    //ダメージが0の場合は健康状態を変更する必要がない
    if (Damage_ == 0)return ;

    //死亡している場合は健康状態を変更しない
    if (mParam.mHitPoint <= 0)return ;


    if (mInvinsibleTimer > 0.0f)return ;

    //無敵時間設定
    mInvinsibleTimer = InvinsibleTime_;
    //ダメージ処理
    mParam.mHitPoint -= Damage_;

    // 死亡したら爆発エフェクトを出す
    if (mParam.mHitPoint <= 0)
    {
        fDieEffect();
    }
}

void BaseEnemy::fCalcNearestEnemy(DirectX::XMFLOAT3 NearPosition_)
{
    mLengthFromTargetEnemy = Math::calc_vector_AtoB_length(mPosition, NearPosition_);
}

bool BaseEnemy::fGetIsFrustum() const
{
    return mIsFrustum;
}

float BaseEnemy::fGetLengthFromPlayer() const
{
    return mLengthFromPlayer;
}

DirectX::XMFLOAT3 BaseEnemy::fGetPosition() const
{
    return mPosition;
}

bool BaseEnemy::fGetIsAlive() const
{
    return mParam.mHitPoint > 0;
}

BaseEnemy::CapsuleCollider BaseEnemy::fGetCapsuleData() const
{
    return mCapsuleCollider;
}

BaseEnemy::CapsuleCollider BaseEnemy::fGetAttackCapsuleData() const
{
    return mAttackCapsuleCollider;
}

int BaseEnemy::fGetUniqueId() const
{
    return mUniqueId;
}

float BaseEnemy::fGetLengthFromNearEstEnemy() const
{
    return  mLengthFromTargetEnemy;
}

const float BaseEnemy::fGetPercentHitPoint() const
{
    if (mParam.mHitPoint <= 0)
        return 0.0f;
    else
        return static_cast<float>(mParam.mHitPoint) / static_cast<float>(mData.mMaxHitPoint);
}

std::string BaseEnemy::fGetType() const
{
    return mData.mDivideClass;
}

BaseEnemy::AttackInformation BaseEnemy::fGetAttackInfo() const
{
    return mAttackInformation;

}

bool BaseEnemy::fGetAttacking() const
{
    return mAttackInformation.mIsAttack;
}

void BaseEnemy::fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_)
{
    mPlayerPosition = PlayerPosition_;
}

void BaseEnemy::fSetPosition(DirectX::XMFLOAT3 Position_)
{
    mPosition = Position_;
}

void BaseEnemy::fSetAttackPower(int Damage_, float InvincibleTime_)
{
    mAttackInformation.mDamage = Damage_;
    mAttackInformation.mInvincible_time = InvincibleTime_;

}

void BaseEnemy::fSetAttackRange(DirectX::XMFLOAT3 Position_, DirectX::XMFLOAT3 Up_, float LengthFromPositionA_, float LengthFromPositionB_)
{
    mAttackInformation.mPosition = Position_;
    mAttackInformation.mUp = Up_;
    mAttackInformation.mLengthFromPositionA = LengthFromPositionA_;
    mAttackInformation.mLengthFromPositionB = LengthFromPositionB_;
}

void BaseEnemy::fAttackStart()
{
    mAttackInformation.mIsAttack = true;
}

void BaseEnemy::fAttackEnd()
{
    mAttackInformation.mIsAttack = false;
}

void BaseEnemy::fSetHitVector(DirectX::XMFLOAT3 Arg_)
{
    mHitVector = Arg_;
}

void BaseEnemy::fUpdateBase(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<移動量を更新>--------------------//
    fUpdateVelocity(elapsedTime_, mPosition, mOrientation);
    //--------------------<姿勢を更新>--------------------//
    fGetDirections(mOrientation);
    //--------------------<無敵時間の更新>--------------------//
    fUpdateInvicibleTimer(elapsedTime_);
    //--------------------<アニメーション更新>--------------------//
    mpSkinnedMesh->update_animation(mAnimPara, elapsedTime_*mAnimationSpeed);
    //--------------------<視錐台カリング>--------------------//
    fCalcFrustum();
    //--------------------<ステートマシン>--------------------//
    fUpdateStateMachine(elapsedTime_,Graphics_);
    //--------------------<プレイヤーとの距離を計算>--------------------//
    fCalcLength();

    fSetCapsulePoint();
}


void BaseEnemy::fUpdateStateMachine(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // 中身が何も設定されていなかった場合の挙動は未知数なので気を付けて

    // ステートを初期化
    if (!mIsInitialize)
    {
        std::get<0>(mCurrentTuple)();
        mIsInitialize = true;
    }
    // ステートを更新
    std::get<1>(mCurrentTuple)(elapsedTime_,Graphics_);
}

void BaseEnemy::fCalcFrustum()
{
    const DirectX::XMFLOAT3 minPoint{
        mPosition.x - mCubeData.mHalfSize.x,
        mPosition.y - mCubeData.mHalfSize.y,
        mPosition.z - mCubeData.mHalfSize.z
    };
    const DirectX::XMFLOAT3 maxPoint{
        mPosition.x + mCubeData.mHalfSize.x,
        mPosition.y + mCubeData.mHalfSize.y,
        mPosition.z + mCubeData.mHalfSize.z
    };

    mIsFrustum=Collision::frustum_vs_cuboid(minPoint, maxPoint);
}

void BaseEnemy::fCalcLength()
{
    mLengthFromPlayer=Math::calc_vector_AtoB_length(mPlayerPosition, mPosition);
}

void BaseEnemy::fSetCapsulePoint()
{
    mCapsuleCollider.mRadius = mData.mCapsule.mRadius;
    mCapsuleCollider.mPointA = mPosition + (up * mData.mCapsule.mLengthFromPositionA);
    mCapsuleCollider.mPointB = mPosition - (up * mData.mCapsule.mLengthFromPositionB);
}

void BaseEnemy::fSetAttackCapsulePoint()
{
    mAttackCapsuleCollider.mRadius = mAttackInformation.mRadius;
    mAttackCapsuleCollider.mPointA = mAttackInformation.mPosition + (mAttackInformation.mUp * mAttackInformation.mLengthFromPositionA);
    mAttackCapsuleCollider.mPointB = mAttackInformation.mPosition - (mAttackInformation.mUp * mAttackInformation.mLengthFromPositionB);
}

void BaseEnemy::fUpdateInvicibleTimer(float elapsedTime_)
{
    if (mInvinsibleTimer > 0.0f)
    {
        mInvinsibleTimer -= elapsedTime_;
    }
    else
    {
        mInvinsibleTimer = 0.0f;
    }
}

void BaseEnemy::fChangeState(std::string Tag_)
{
    mCurrentTuple = mFunctionMap.at(Tag_);
    mIsInitialize = false;
}


bool BaseEnemy::fTurnToTarget(float elapsedTime_, DirectX::XMFLOAT3 target_pos, float turn_speed)
{
    using namespace DirectX;
    //ターゲットに向かって回転
    XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&mOrientation);
    DirectX::XMVECTOR forward, right, up;
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);
    right = { m4x4._11, m4x4._12, m4x4._13 };
    //up = { m4x4._21, m4x4._22, m4x4._23 };
    up = { 0, 1, 0 };
    forward = { m4x4._31, m4x4._32, m4x4._33 };

    XMVECTOR pos_vec = XMLoadFloat3(&mPosition);//自分の位置
    DirectX::XMFLOAT3 front{};
    DirectX::XMStoreFloat3(&front, forward);
    DirectX::XMFLOAT3 t{ target_pos };
    XMVECTOR target_vec = XMLoadFloat3(&t);
    XMVECTOR d = XMVector3Normalize(target_vec - pos_vec);
    float d_length = Math::calc_vector_AtoB_length(mPosition, t);
    {
        DirectX::XMFLOAT3 point = Math::calc_designated_point(mPosition, front, d_length);
        //point.y = target.y;
        DirectX::XMVECTOR point_vec = DirectX::XMLoadFloat3(&point);


        XMVECTOR d2 = XMVector3Normalize(point_vec - pos_vec);

        float an;
        XMVECTOR a = XMVector3Dot(d2, d);
        XMStoreFloat(&an, a);
        an = acosf(an);
        float de = DirectX::XMConvertToDegrees(an);

        if (fabs(an) > DirectX::XMConvertToRadians(0.1f))
        {
            XMVECTOR q;
            DirectX::XMFLOAT3 a{};
            DirectX::XMStoreFloat3(&a, d2);
            DirectX::XMFLOAT3 b{};
            DirectX::XMStoreFloat3(&b, d);
            float cross{ (b.x * a.z) - (b.z * a.x) };

            if (cross > 0)
            {
                q = XMQuaternionRotationAxis(up, an);//正の方向に動くクオータニオン
            }
            else
            {
                q = XMQuaternionRotationAxis(up, -an);//正の方向に動くクオータニオン
            }
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, turn_speed * elapsedTime_);
        }
    }
    //right
    {

        DirectX::XMFLOAT3 point = Math::calc_designated_point(mPosition, front, d_length);
        //point.x = target.x;
        //point.z = target.z;
        DirectX::XMVECTOR point_vec = DirectX::XMLoadFloat3(&point);


        XMVECTOR d2 = XMVector3Normalize(point_vec - pos_vec);

        float an;
        XMVECTOR a = XMVector3Dot(d2, d);
        XMStoreFloat(&an, a);
        an = acosf(an);
        float de = DirectX::XMConvertToDegrees(an);
        if (fabs(an) > DirectX::XMConvertToRadians(0.1f) && fabs(an) < DirectX::XMConvertToRadians(170.0f))
        {
            //回転軸と回転角から回転クオータニオンを求める
            XMVECTOR q;
            if (point.y > target_pos.y)
            {
                q = XMQuaternionRotationAxis(right, an);//正の方向に動くクオータニオン
            }
            else
            {
                q = XMQuaternionRotationAxis(right, -an);//正の方向に動くクオータニオン
            }
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, turn_speed * elapsedTime_);
        }
    }
    DirectX::XMStoreFloat4(&mOrientation, orientation_vec);
    return false;
}


void BaseEnemy::fGetDirections(DirectX::XMFLOAT4& o_)
{
    using namespace DirectX;
    const XMVECTOR orientation_vec{ XMLoadFloat4(&o_) };
    const DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);

    const DirectX::XMVECTOR right_vec = { m4x4._11, m4x4._12, m4x4._13 };
    const DirectX::XMVECTOR up_vec = { m4x4._21, m4x4._22, m4x4._23 };
    const DirectX::XMVECTOR forward_vec = { m4x4._31, m4x4._32, m4x4._33 };

    XMStoreFloat3(&right, right_vec);
    XMStoreFloat3(&up, up_vec);
    XMStoreFloat3(&forward, forward_vec);

}

void BaseEnemy::fMoveVelocity(float elapsedTime_, DirectX::XMFLOAT3 Velocity_, float Speed_)
{
    // 速度を正規化する
    const auto v = Math::Normalize(Velocity_);
    mPosition += v * Speed_*elapsedTime_;

}
