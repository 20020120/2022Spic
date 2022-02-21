#include "MoveBehavior.h"

DirectX::XMFLOAT3 MoveBehavior::SetMoveVec(const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right)
{
    //入力情報を取得
    float ax = game_pad->get_axis_LX();
    float ay = game_pad->get_axis_LY();

    //移動ベクトルはXZ平面に水平なベクトルになるようにする

    //カメラ右方向ベクトルをXZ単位ベクトルに変換

    float camerarightlengh = sqrtf((camera_right.x * camera_right.x) + (camera_right.y * camera_right.y) + (camera_right.z * camera_right.z));
    DirectX::XMFLOAT3 camera_right_normal{};
    if (camerarightlengh > 0.0f)
    {
        camera_right_normal = Math::Normalize(camera_right);
    }

    float cameraforwardlength = sqrtf((camera_forward.x * camera_forward.x) + (camera_forward.y * camera_forward.y) + (camera_forward.z * camera_forward.z));
    DirectX::XMFLOAT3 camera_forward_normal{};
    if (cameraforwardlength > 0.0f)
    {
        camera_forward_normal = Math::Normalize(camera_forward);
    }

    DirectX::XMFLOAT3 vec{};
    vec.x = (camera_forward_normal.x * ay) + (camera_right_normal.x * ax);
    vec.y = (camera_forward_normal.y * ay) + (camera_right_normal.y * ax);
    vec.z = (camera_forward_normal.z * ay) + (camera_right_normal.z * ax);

    return vec;
}

void MoveBehavior::MovingProcess(float vx, float vz, float speed)
{
    //移動方向ベクトルを設定
    move_vec_x = vx;
    move_vec_z = vz;
    //最大速度設定
    max_move_speed = speed;
}

void MoveBehavior::Turn(float elapsed_time, float vx, float vz, float speed, DirectX::XMFLOAT4& orientation)
{
    speed *= elapsed_time;
    DirectX::XMVECTOR orientation_vec{ DirectX::XMLoadFloat4(&orientation) };

    float length = sqrtf(vx * vx + vz * vz);
    if (length < 0.001f) return;

    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMVECTOR forward, up;
    m4x4 = QuaternionConvertXMFLOAT4X4(orientation);
    up = { 0, 1, 0 };
    //up = { m4x4._21, m4x4._22, m4x4._23 };
    forward = { m4x4._31, m4x4._32, m4x4._33 };

    DirectX::XMVECTOR axis_up = up;//Yの回転軸

    DirectX::XMVECTOR velocity_vec{ DirectX::XMLoadFloat3(&velocity) };
    DirectX::XMVECTOR velocity_vec_normal{ DirectX::XMVector3Normalize(velocity_vec) };

    //前方向(forward)と回転仕切った後の姿勢との内積を取る
    DirectX::XMVECTOR a;
    a = DirectX::XMVector3Dot(velocity_vec_normal, forward);
    float angle;
    DirectX::XMStoreFloat(&angle, a);
    angle = acos(angle);
    if (fabs(angle) > 1e-8f)
    {
        DirectX::XMFLOAT3 f{};
        DirectX::XMStoreFloat3(&f, forward);
        float cross{ (f.x * velocity.z) - (f.z * velocity.x) };

        if (cross < 0.0f)
        {
            DirectX::XMVECTOR q;
            q = DirectX::XMQuaternionRotationAxis(axis_up, angle);
            DirectX::XMVECTOR Q = DirectX::XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = DirectX::XMQuaternionSlerp(orientation_vec, Q, speed);
        }
        else
        {
            DirectX::XMVECTOR q;
            q = DirectX::XMQuaternionRotationAxis(axis_up, -angle);
            DirectX::XMVECTOR Q = DirectX::XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = DirectX::XMQuaternionSlerp(orientation_vec, Q, speed);
        }
    }
    DirectX::XMStoreFloat4(&orientation, orientation_vec);

}
