#include "BaseEnemy.h"
#include"user.h"
#include"collision.h"
//****************************************************************
// 
// 敵の基底クラス 
// 
//****************************************************************
BaseEnemy::BaseEnemy(ID3D11Device* pDevice_, const char* ModelName_)
{
    // モデルを初期化
    mpSkinnedMesh = std::make_unique<SkinnedMesh>(pDevice_, ModelName_);
}

BaseEnemy::~BaseEnemy() = default;

void BaseEnemy::fRender(ID3D11DeviceContext* pDeviceContext_) const
{
    // ワールド行列を作成
    const auto worldMatrix = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    mpSkinnedMesh->render(pDeviceContext_, worldMatrix, { 1.0f,1.0f,1.0f,1.0f });
}

void BaseEnemy::fDamaged(int Damage_)
{
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

void BaseEnemy::fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_)
{
    mPlayerPosition = PlayerPosition_;
}

void BaseEnemy::fUpdateBase(float elapsedTime_)
{
    //--------------------<移動量を更新>--------------------//
    fUpdateVelocity(elapsedTime_, mPosition, mOrientation);
    //--------------------<姿勢を更新>--------------------//
    fGetEnemyDirections();
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


void BaseEnemy::fChangeState(int i)
{
    mCurrentTuple = mFunctionMap.at(i);
    mIsInitialize = false;
}

void BaseEnemy::fGetEnemyDirections()
{
    using namespace DirectX;
    XMVECTOR orientation_vec{ XMLoadFloat4(&mOrientation) };
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);
    DirectX::XMVECTOR right_vec, forward_vec, up_vec;

    right_vec = { m4x4._11, m4x4._12, m4x4._13 };
    up_vec = { m4x4._21, m4x4._22, m4x4._23 };
    forward_vec = { m4x4._31, m4x4._32, m4x4._33 };

    XMStoreFloat3(&right, right_vec);
    XMStoreFloat3(&up, up_vec);
    XMStoreFloat3(&forward, forward_vec);

}


bool BaseEnemy::fTurnToPlayer(float elapsedTime_, float end_turn_angle)
{
    using namespace DirectX;
    XMVECTOR orientation_vec = XMLoadFloat4(&mOrientation);

    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMVECTOR forward, up;
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMStoreFloat4x4(&m4x4, m);
    up = { m4x4._21, m4x4._22, m4x4._23 };
    forward = { m4x4._31, m4x4._32, m4x4._33 };

    XMVECTOR P_Pos = XMLoadFloat3(&mPlayerPosition);
    XMVECTOR E_Pos = XMLoadFloat3(&mPosition);
    XMVECTOR Vec = P_Pos - E_Pos;
    Vec = XMVector3Normalize(Vec);
    XMVECTOR Dot = XMVector3Dot(Vec, forward);
    float angle = XMVectorGetX(Dot);
    angle = acos(angle);
    if (fabs(angle) > 1e-8f)
    {
        DirectX::XMFLOAT3 f{};
        DirectX::XMFLOAT3 v{};
        DirectX::XMStoreFloat3(&f, forward);
        DirectX::XMStoreFloat3(&v, Vec);
        float cross{ (f.x * v.z) - (f.z * v.x) };

        if (cross < 0.0f)
        {
            XMVECTOR q;
            q = XMQuaternionRotationAxis(up, angle);
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 0.5f * elapsedTime_);
        }
        else
        {
            XMVECTOR q;
            q = XMQuaternionRotationAxis(up, -angle);
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 0.5f * elapsedTime_);
        }
        XMStoreFloat4(&mOrientation, orientation_vec);
    }
    int a = XMConvertToDegrees(angle);
    //プレイヤーへの向きと敵の向きの角度がend_turn_angle度以下ならtrueを返す
    if (angle < XMConvertToRadians(end_turn_angle))
    {
        return true;
    }

    return false;
}

void BaseEnemy::fUpdateVelocity(float elapsedTime_, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation)
{
    //経過フレーム
    float elapsed_frame = 60.0f * elapsedTime_;
    fUpdateVerticalVelocity(elapsed_frame);
    fUpdateVerticalMove(elapsedTime_, position);
    fUpdateHrizontalVelocity(elapsed_frame);
    fUpdateHorizontalMove(elapsedTime_, position);
}

void BaseEnemy::fUpdateVerticalVelocity(float elapsedFrame)
{
    //特に処理なし
}

void BaseEnemy::fUpdateVerticalMove(float elapsedTime_, DirectX::XMFLOAT3& position)
{
    //垂直方向の移動量
    float my = velocity.y * elapsedTime_;

    if (my != 0.0f)
    {
    	position.y += my;
    }
}

void BaseEnemy::fUpdateHrizontalVelocity(float elasedFrame_)
{
    //XZ平面の速力を減速する
    float length{ sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) };
    if (length > 0.0f)
    {

        //摩擦力
        float friction{ this->friction * elasedFrame_ };
        //摩擦による横方向の減速処理
        if (length > friction)
        {
            (velocity.x < 0.0f) ? velocity.x += friction : velocity.x -= friction;
            (velocity.z < 0.0f) ? velocity.z += friction : velocity.z -= friction;
        }
        //横方向の速力が摩擦力以下になったので速力を無効化 GetMoveVec()
        else
        {
            velocity.x = 0;
            velocity.z = 0;
        }
    }
    //XZ平面の速力を加速する
    if (length <= max_move_speed)
    {
        //移動ベクトルが0でないなら加速する
        float moveveclength{ sqrtf((move_vec_x * move_vec_x) + (move_vec_z * move_vec_z)) };
        if (moveveclength > 0.0f)
        {
            //加速力
            float acceleration{ this->acceleration * elasedFrame_ };
            //移動ベクトルによる加速処理
            velocity.x += move_vec_x * acceleration;
            velocity.z += move_vec_z * acceleration;
            float length{ sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) };
            if (length > max_move_speed)
            {
                float vx{ velocity.x / length };
                float vz{ velocity.z / length };

                velocity.x = vx * max_move_speed;
                velocity.z = vz * max_move_speed;
            }
        }
    }
    move_vec_x = 0.0f;
    move_vec_z = 0.0f;
}

void BaseEnemy::fUpdateHorizontalMove(float elapsedTime_, DirectX::XMFLOAT3& position)
{
    using namespace DirectX;
    // 水平速力計算
    float velocity_length_xz = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    //水平移動値
    float mx{ velocity.x * 100.0f * elapsedTime_ };
    float mz{ velocity.z * 100.0f * elapsedTime_ };
    if (velocity_length_xz > 0.0f)
    {
        position.x += velocity.x * elapsedTime_;
        position.z += velocity.z * elapsedTime_;
    }
}
