#include "MoveBehavior.h"

DirectX::XMFLOAT3 MoveBehavior::SetMoveVec(const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right)
{
    //入力情報を取得
    float ax = game_pad->get_axis_LX();
    float ay = game_pad->get_axis_LY();

    //移動ベクトルはXZ平面に水平なベクトルになるようにする

    //カメラ右方向ベクトルをXZ単位ベクトルに変換
    float camerarightx = camera_right.x;
    float camerarightz = camera_right.z;
    float camerarightlengh = sqrtf((camerarightx * camerarightx) + (camerarightz * camerarightz));

    if (camerarightlengh > 0.0f)
    {
        camerarightx /= camerarightlengh;
        camerarightz /= camerarightlengh;
    }

    float cameraforwardx = camera_forward.x;
    float cameraforwardz = camera_forward.z;
    float cameraforwardlength = sqrtf((cameraforwardx * cameraforwardx) + (cameraforwardz * cameraforwardz));
    if (cameraforwardlength > 0.0f)
    {
        cameraforwardx /= cameraforwardlength;
        cameraforwardz /= cameraforwardlength;
    }

    DirectX::XMFLOAT3 vec{};
    vec.x = (cameraforwardx * ay) + (camerarightx * ax);
    vec.z = (cameraforwardz * ay) + (camerarightz * ax);

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

    vx /= length;
    vz /= length;

    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMVECTOR forward, up;
    //m4x4 = Convert::QuaternionConvertXMFLOAT4X4(orientation);
    up = { m4x4._21, m4x4._22, m4x4._23 };
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
            orientation_vec = DirectX::XMQuaternionSlerp(orientation_vec, Q, 0.005);
        }
        else
        {
            DirectX::XMVECTOR q;
            q = DirectX::XMQuaternionRotationAxis(axis_up, -angle);
            DirectX::XMVECTOR Q = DirectX::XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = DirectX::XMQuaternionSlerp(orientation_vec, Q, 0.005);
        }
    }
    DirectX::XMStoreFloat4(&orientation, orientation_vec);

}
