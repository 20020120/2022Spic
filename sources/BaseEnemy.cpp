#include "BaseEnemy.h"
#include"user.h"
#include"collision.h"
#include"Operators.h"
#include<typeinfo>
//****************************************************************
// 
// 敵の基底クラス 
// 
//****************************************************************
BaseEnemy::BaseEnemy(ID3D11Device* pDevice_,int UniqueId_, const char* ModelName_)
{
    // モデルを初期化
    mpSkinnedMesh = std::make_unique<SkinnedMesh>(pDevice_, ModelName_);
    mUniqueId = UniqueId_;

}

BaseEnemy::~BaseEnemy() = default;

void BaseEnemy::fRender(ID3D11DeviceContext* pDeviceContext_) const
{
    // ワールド行列を作成
    const auto worldMatrix = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    mpSkinnedMesh->render(pDeviceContext_, worldMatrix, { 1.0f,1.0f,1.0f,1.0f });
}

void BaseEnemy::fGetParam(BaseEnemy* This_, std::function<EnemyData(std::string)> Function_)
{
    // 敵のデータをロード
    const type_info& id = typeid(*This_);
    mData = Function_(id.name());
    mParam.mAttackPower = mData.mAttackPower;
    mParam.mAttackSpeed = mData.mAttackSpeed;
    mParam.mHitPoint = mData.mMaxHitPoint;
    mParam.mMoveSpeed = mData.mMoveSpeed;
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

void BaseEnemy::fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_)
{
    mPlayerPosition = PlayerPosition_;
}

void BaseEnemy::fSetPosition(DirectX::XMFLOAT3 Position_)
{
    mPosition = Position_;
}

void BaseEnemy::fUpdateBase(float elapsedTime_)
{
    //--------------------<移動量を更新>--------------------//
    fUpdateVelocity(elapsedTime_, mPosition, mOrientation);
    //--------------------<姿勢を更新>--------------------//
    fGetEnemyDirections();
    //--------------------<無敵時間の更新>--------------------//
    fUpdateInvicibleTimer(elapsedTime_);
    //--------------------<アニメーション更新>--------------------//
  //  mpSkinnedMesh->update_animation(elapsedTime_);
    //--------------------<視錐台カリング>--------------------//
    fCalcFrustum();
    //--------------------<ステートマシン>--------------------//
    fUpdateStateMachine(elapsedTime_);
    //--------------------<プレイヤーとの距離を計算>--------------------//
    fCalcLength();

    fSetCapsulePoint();
}


void BaseEnemy::fUpdateStateMachine(float elapsedTime_)
{
    // 中身が何も設定されていなかった場合の挙動は未知数なので気を付けて
   
    // ステートを初期化
    if (!mIsInitialize)
    {
        std::get<0>(mCurrentTuple)();
        mIsInitialize = true;
    }
    // ステートを更新
    std::get<1>(mCurrentTuple)(elapsedTime_);
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

void BaseEnemy::fChangeState(int i)
{
    mCurrentTuple = mFunctionMap.at(i);
    mIsInitialize = false;
}

void BaseEnemy::fGetEnemyDirections()
{
    using namespace DirectX;
    const XMVECTOR orientation_vec{ XMLoadFloat4(&mOrientation) };
    const DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);

    const DirectX::XMVECTOR right_vec = {m4x4._11, m4x4._12, m4x4._13};
    const DirectX::XMVECTOR up_vec = {m4x4._21, m4x4._22, m4x4._23};
    const DirectX::XMVECTOR forward_vec = {m4x4._31, m4x4._32, m4x4._33};

    XMStoreFloat3(&right, right_vec);
    XMStoreFloat3(&up, up_vec);
    XMStoreFloat3(&forward, forward_vec);

}


bool BaseEnemy::fTurnToPlayer(float elapsedTime_)
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
    DirectX::XMFLOAT3 t{ mPlayerPosition };
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
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsedTime_);
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
            if (point.y > mPlayerPosition.y)
            {
                q = XMQuaternionRotationAxis(right, an);//正の方向に動くクオータニオン
            }
            else
            {
                q = XMQuaternionRotationAxis(right, -an);//正の方向に動くクオータニオン
            }
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsedTime_);
        }
    }
    DirectX::XMStoreFloat4(&mOrientation, orientation_vec);
    return false;
}

void BaseEnemy::fUpdateVelocity(float elapsedTime_, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation)
{
    //経過フレーム
    float elapsed_frame = 60.0f * elapsedTime_;
    fCalcVelocity(elapsed_frame);
    fUpdateMove(elapsedTime_, position);
}

void BaseEnemy::fCalcVelocity(float elasedFrame_)
{
    //XZ平面の速力を減速する
    const float length{ sqrtf((velocity.x * velocity.x) + (velocity.y * velocity.y) + (velocity.z * velocity.z)) };
    if (length > 0.0f)
    {

        //摩擦力
        const float friction{ this->friction * elasedFrame_ };
        //摩擦による横方向の減速処理
        if (length > friction)
        {
            (velocity.x < 0.0f) ? velocity.x += friction : velocity.x -= friction;
            (velocity.y < 0.0f) ? velocity.y += friction : velocity.y -= friction;
            (velocity.z < 0.0f) ? velocity.z += friction : velocity.z -= friction;
        }
        //横方向の速力が摩擦力以下になったので速力を無効化 GetMoveVec()
        else
        {
            velocity.x = 0;
            velocity.y = 0;
            velocity.z = 0;
        }
    }
    //XZ平面の速力を加速する
    if (length <= max_move_speed)
    {
        //移動ベクトルが0でないなら加速する
        const float moveveclength{ sqrtf((move_vec_x * move_vec_x) + (move_vec_y * move_vec_y) + (move_vec_z * move_vec_z)) };
        if (moveveclength > 0.0f)
        {
            //加速力
            const float acceleration{ this->acceleration * elasedFrame_ };
            //移動ベクトルによる加速処理
            velocity.x += move_vec_x * acceleration;
            velocity.y += move_vec_y * acceleration;
            velocity.z += move_vec_z * acceleration;
            float length{ sqrtf((velocity.x * velocity.x) + (velocity.y * velocity.y) + (velocity.z * velocity.z)) };
            if (length > max_move_speed)
            {
                const float vx{ velocity.x / length };
                const float vy{ velocity.y / length };
                const float vz{ velocity.z / length };

                velocity.x = vx * max_move_speed;
                velocity.y = vy * max_move_speed;
                velocity.z = vz * max_move_speed;
            }
        }
    }
    move_vec_x = 0.0f;
    move_vec_y = 0.0f;
    move_vec_z = 0.0f;
}

void BaseEnemy::fUpdateMove(float elapsedTime_, DirectX::XMFLOAT3& position)
{
    using namespace DirectX;
    // 水平速力計算
    const float velocity_length_xyz = sqrtf((velocity.x * velocity.x) + (velocity.y * velocity.y) + (velocity.z * velocity.z));
    if (velocity_length_xyz > 0.0f)
    {
        position.x += velocity.x * elapsedTime_;
        position.y += velocity.y * elapsedTime_;
        position.z += velocity.z * elapsedTime_;
    }
}
