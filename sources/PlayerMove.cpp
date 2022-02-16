#include "PlayerMove.h"

PlayerMove::PlayerMove()
    :MoveBehavior()
{
}

PlayerMove::~PlayerMove()
{
}

void PlayerMove::UpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation, const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right, SkyDome* sky_dome)
{
    DirectX::XMFLOAT3 movevec = SetMoveVec(camera_forward, camera_right);
    MovingProcess(movevec.x, movevec.z, move_speed);
    //旋回処理
    Turn(elapsed_time, movevec.x, movevec.z, turn_speed, orientation);

    //経過フレーム
    float elapsed_frame = 60.0f * elapsed_time;
    UpdateVerticalVelocity(elapsed_frame);
    UpdateVerticalMove(elapsed_time, position, sky_dome);
    UpdateHrizontalVelocity(elapsed_frame);
    UpdateHorizontalMove(elapsed_time, position, sky_dome);
}

void PlayerMove::UpdateVerticalVelocity(float elapsed_frame)
{
    //velocity.y += gravity * elapsed_frame;
}

void PlayerMove::UpdateVerticalMove(float elapsed_time, DirectX::XMFLOAT3& position, SkyDome* sky_dome)
{
    //垂直方向の移動量
    float my = velocity.y * elapsed_time;

    if (my != 0.0f)
    {
        //レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start = { position.x, position.y, position.z };
        //レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y - my + step_offset_y, position.z };

        if (sky_dome->RayCast(start, end, hit))
        {
            position.y = hit.position.y;
            velocity.y = 0.0f;
        }
        else
        {
            position.y += my;
        }
    }
}

void PlayerMove::UpdateHrizontalVelocity(float elapsed_frame)
{
    //XZ平面の速力を減速する
    float length{ sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) };
    if (length > 0.0f)
    {

        //摩擦力
        float friction{ this->friction * elapsed_frame };
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
            float acceleration{ this->acceleration * elapsed_frame };
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

void PlayerMove::UpdateHorizontalMove(float elapsed_time, DirectX::XMFLOAT3& position, SkyDome* sky_dome)
{
    using namespace DirectX;
    // 水平速力計算
    float velocity_length_xz = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
        //水平移動値
    float mx{ velocity.x*100.0f * elapsed_time };
    float mz{ velocity.z*100.0f * elapsed_time };
    if (velocity_length_xz > 0.0f)
    {
        // レイの開始位置と終点位置
        DirectX::XMFLOAT3 start = { position.x, position.y, position.z };
        DirectX::XMFLOAT3 end = { position.x + mx, start.y, position.z + mz};

        if (sky_dome->RayCast(start, end, hit))
        {
            using namespace DirectX;
            // 壁までのベクトル
            auto start_vec = XMLoadFloat3(&start);
            auto end_vec = XMLoadFloat3(&end);
            auto vec = end_vec - start_vec;
            // 壁の法線
            auto normal_vec = XMLoadFloat3(&hit.normal);
            // 入射ベクトルを法線に射影
            auto dot_vec = XMVector3Dot(vec, normal_vec);
            // 補正位置の計算
            auto corrected_position_vec = end_vec - (normal_vec * dot_vec);
            XMVECTOR interpolationvec{ corrected_position_vec - start_vec };//壁ずりベクトルの向き

            XMFLOAT3 corrected_position{};
            XMStoreFloat3(&corrected_position, corrected_position_vec);
            // レイの開始位置と終点位置
            DirectX::XMFLOAT3 start2 = { position.x, position.y, position.z };
            DirectX::XMFLOAT3 end2 = { corrected_position };
            if (sky_dome->RayCast(start2, end2, hit))
            {
                position.x = hit.position.x;
                position.z = hit.position.z;
            }
            else
            {
                XMFLOAT3 interpolation{};
                XMStoreFloat3(&interpolation, interpolationvec);
                position.x += interpolation.x;
                position.z += interpolation.z;

            }
        }
        else
        {
            position.x += velocity.x * elapsed_time;
            position.z += velocity.z * elapsed_time;
        }
    }
}
