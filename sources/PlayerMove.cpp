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

    //�G�Ƀ��b�N�I��������G�̕���������
    if (is_lock_on)
    {
        RotateToTarget(elapsed_time, position,orientation);
    }
    //���b�N�I�����Ă��Ȃ���������͕���������
    else
    {
        //���񏈗�
        Turn(elapsed_time, movevec.x, movevec.z, turn_speed, orientation);
    }

    //�o�߃t���[��
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
    //���������̈ړ���
    float my = velocity.y * elapsed_time;

    if (my != 0.0f)
    {
        //���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start = { position.x, position.y, position.z };
        //���C�̏I�_�ʒu�͈ړ���̈ʒu
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
    //XZ���ʂ̑��͂���������
    float length{ sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) };
    if (length > 0.0f)
    {

        //���C��
        float friction{ this->friction * elapsed_frame };
        //���C�ɂ�鉡�����̌�������
        if (length > friction)
        {
            (velocity.x < 0.0f) ? velocity.x += friction : velocity.x -= friction;
            (velocity.z < 0.0f) ? velocity.z += friction : velocity.z -= friction;
        }
        //�������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌��� GetMoveVec()
        else
        {
            velocity.x = 0;
            velocity.z = 0;
        }
    }
    //XZ���ʂ̑��͂���������
    if (length <= max_move_speed)
    {
        //�ړ��x�N�g����0�łȂ��Ȃ��������
        float moveveclength{ sqrtf((move_vec_x * move_vec_x) + (move_vec_z * move_vec_z)) };
        if (moveveclength > 0.0f)
        {
            //������
            float acceleration{ this->acceleration * elapsed_frame };
            //�ړ��x�N�g���ɂ���������
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
    // �������͌v�Z
    float velocity_length_xz = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
        //�����ړ��l
    float mx{ velocity.x*100.0f * elapsed_time };
    float mz{ velocity.z*100.0f * elapsed_time };
    if (velocity_length_xz > 0.0f)
    {
        // ���C�̊J�n�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x, position.y, position.z };
        DirectX::XMFLOAT3 end = { position.x + mx, start.y, position.z + mz};

        if (sky_dome->RayCast(start, end, hit))
        {
            using namespace DirectX;
            // �ǂ܂ł̃x�N�g��
            auto start_vec = XMLoadFloat3(&start);
            auto end_vec = XMLoadFloat3(&end);
            auto vec = end_vec - start_vec;
            // �ǂ̖@��
            auto normal_vec = XMLoadFloat3(&hit.normal);
            // ���˃x�N�g����@���Ɏˉe
            auto dot_vec = XMVector3Dot(vec, normal_vec);
            // �␳�ʒu�̌v�Z
            auto corrected_position_vec = end_vec - (normal_vec * dot_vec);
            XMVECTOR interpolationvec{ corrected_position_vec - start_vec };//�ǂ���x�N�g���̌���

            XMFLOAT3 corrected_position{};
            XMStoreFloat3(&corrected_position, corrected_position_vec);
            // ���C�̊J�n�ʒu�ƏI�_�ʒu
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

void PlayerMove::RotateToTarget(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation)
{
    using namespace DirectX;

    //�^�[�Q�b�g�Ɍ������ĉ�]
    XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&orientation);
    DirectX::XMVECTOR forward, right, up;
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);
    right = { m4x4._11, m4x4._12, m4x4._13 };
    up = { m4x4._21, m4x4._22, m4x4._23 };
    forward = { m4x4._31, m4x4._32, m4x4._33 };

    XMVECTOR d;//�ڕW�ւ̕���
    XMVECTOR pos_vec;//�����̈ʒu
    XMVECTOR target_vec;//�^�[�Q�b�g�̈ʒu
    pos_vec = XMLoadFloat3(&position);
    target_vec = XMLoadFloat3(&target);
    d = target_vec - pos_vec;
    d = XMVector3Normalize(d);
    //***********************************************************
//���݂̎p���ɂ�����O��(forward)���z�[�������Ƃ�
//����(d)�ւ̉�]��(axis)�Ɖ�]�p(angle)�����߂�
//***********************************************************
    XMVECTOR axis;//��]��
    float an;
    axis = XMVector3Cross(forward,d);
    forward = XMVector3Normalize(forward);

    XMVECTOR a = XMVector3Dot(d, forward);
    XMStoreFloat(&an, a);
    an = static_cast<float>(acos(an));
    XMFLOAT3 forw;//forward�̒l��float3��
    XMFLOAT3 d_;//d�̒l��float3��

    XMStoreFloat3(&forw, forward);
    XMStoreFloat3(&d_, d);

    if (fabs(an) > XMConvertToRadians(5.0f))
    {

        float cross{ (forw.x * d_.z) - (forw.z * d_.x) };

        //�N�I�[�^�j�I���͉�]�̎d��(�ǂ̌�����)

        if (cross < 0.0f)
        {
            //��]���Ɖ�]�p�����]�N�I�[�^�j�I�������߂�
            XMVECTOR q;
            q = XMQuaternionRotationAxis(axis, an);//���̕����ɓ����N�I�[�^�j�I��

            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsed_time);
        }
        else
        {
            XMVECTOR q;
            q = XMQuaternionRotationAxis(axis, -an);//���̕����ɓ����N�I�[�^�j�I��
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);

            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsed_time);
        }
    }

    DirectX::XMStoreFloat4(&orientation, orientation_vec);
}
