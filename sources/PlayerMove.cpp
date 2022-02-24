#include "PlayerMove.h"

PlayerMove::PlayerMove()
    :MoveBehavior()
{
}

PlayerMove::~PlayerMove()
{
}

void PlayerMove::UpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation, const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right,const DirectX::XMFLOAT3& camera_pos ,SkyDome* sky_dome)
{
    DirectX::XMFLOAT3 movevec = SetMoveVec(camera_forward, camera_right);
    MovingProcess(movevec.x, movevec.z, move_speed);
    move_vec_y = movevec.y;//MovingProcess�֐��͓G�ł��g���Ă邩�炱���ɏ����Ă���
    SetDirections(orientation);
    //�G�Ƀ��b�N�I��������G�̕���������
    if (is_enemy && is_lock_on)
    {
        RotateToTarget(elapsed_time, position,orientation);
        RollTurn(position, orientation, elapsed_time);
    }
    //���b�N�I�����Ă��Ȃ���������͕���������
    else
    {
        //���񏈗�
        Turn(elapsed_time, movevec, turn_speed, position, orientation);
        PitchTurn(position, camera_pos, camera_forward, orientation, elapsed_time);
        RollTurn(position, orientation, elapsed_time);
    }

    //�o�߃t���[��
    float elapsed_frame = 60.0f * elapsed_time;
    UpdateVerticalVelocity(elapsed_frame);
    UpdateVerticalMove(elapsed_time, position, sky_dome);
    UpdateHrizontalVelocity(elapsed_frame);
    UpdateHorizontalMove(elapsed_time, position, sky_dome);
}

void PlayerMove::UpdateAvoidanceVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation, const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right, const DirectX::XMFLOAT3& camera_pos, SkyDome* sky_dome)
{
    if (is_enemy && is_lock_on)
    {
        RotateToTarget(elapsed_time, position, orientation);
        RollTurn(position, orientation, elapsed_time);
    }

    SetDirections(orientation);

    //�o�߃t���[��
    float elapsed_frame = 60.0f * elapsed_time;
    UpdateVerticalVelocity(elapsed_frame);
    UpdateVerticalMove(elapsed_time, position, sky_dome);
    UpdateHrizontalVelocity(elapsed_frame);
    UpdateHorizontalMove(elapsed_time, position, sky_dome);

}

void PlayerMove::UpdateBehindAvoidanceVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation, const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right, const DirectX::XMFLOAT3& camera_pos, SkyDome* sky_dome)
{
    if (is_enemy && is_lock_on)
    {
        RotateToTarget(elapsed_time, position, orientation);
        RollTurn(position, orientation, elapsed_time);
    }

    SetDirections(orientation);

    //�o�߃t���[��
    float elapsed_frame = 60.0f * elapsed_time;
    //UpdateVerticalVelocity(elapsed_frame);
    //UpdateVerticalMove(elapsed_time, position, sky_dome);
    //UpdateHrizontalVelocity(elapsed_frame);
    //UpdateHorizontalMove(elapsed_time, position, sky_dome);
}

void PlayerMove::UpdateAttackVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation, const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right, const DirectX::XMFLOAT3& camera_pos, SkyDome* sky_dome)
{
    //�o�߃t���[��
    float elapsed_frame = 60.0f * elapsed_time;

    if (is_enemy && is_lock_on)
    {
        RotateToTarget(elapsed_time, position, orientation);
        RollTurn(position, orientation, elapsed_time);
    }
    UpdateVerticalVelocity(elapsed_frame);
    UpdateVerticalMove(elapsed_time, position, sky_dome);
    UpdateHrizontalVelocity(elapsed_frame);
    UpdateHorizontalMove(elapsed_time, position, sky_dome);
    SetDirections(orientation);



}

void PlayerMove::UpdateVerticalVelocity(float elapsed_frame)
{
    float length{ sqrtf(velocity.y * velocity.y)};
    if (length > 0.0f)
    {
        //���C��
        float friction{ this->friction * elapsed_frame };
        //���C�ɂ�鉡�����̌�������
        if (length > friction)
        {
            (velocity.y < 0.0f) ? velocity.y += friction : velocity.y -= friction;
        }
        else
        {
            velocity.y = 0;
        }
    }
    if (length <= max_move_speed)
    {
        //�ړ��x�N�g����0�łȂ��Ȃ��������
        float moveveclength{ sqrtf((move_vec_y * move_vec_y)) };
        if (moveveclength > 0.0f)
        {
            //������
            float acceleration{ this->acceleration * elapsed_frame };
            //�ړ��x�N�g���ɂ���������
            velocity.y += move_vec_y * acceleration;
            float length{ sqrtf((velocity.y * velocity.y)) };
            if (length > max_move_speed)
            {
                float vy{ velocity.y / length };
                velocity.y = vy * max_move_speed;
            }
        }
    }
    move_vec_y = 0.0f;
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
    //up = { m4x4._21, m4x4._22, m4x4._23 };
    up = { 0, 1, 0 };
    forward = { m4x4._31, m4x4._32, m4x4._33 };

    XMVECTOR pos_vec = XMLoadFloat3(&position);//�����̈ʒu
    DirectX::XMFLOAT3 front{};
    DirectX::XMStoreFloat3(&front, forward);
    DirectX::XMFLOAT3 t{ target.x, target.y, target.z };
    XMVECTOR target_vec = XMLoadFloat3(&t);
    XMVECTOR d = XMVector3Normalize(target_vec - pos_vec);
    float d_length = Math::calc_vector_AtoB_length(position, t);
    {
        DirectX::XMFLOAT3 point = Math::calc_designated_point(position, front, d_length);
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
                q = XMQuaternionRotationAxis(up, an);//���̕����ɓ����N�I�[�^�j�I��
            }
            else
            {
                q = XMQuaternionRotationAxis(up, -an);//���̕����ɓ����N�I�[�^�j�I��
            }
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsed_time);
        }
    }
    //right
    {

        DirectX::XMFLOAT3 point = Math::calc_designated_point(position, front, d_length);
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
            //��]���Ɖ�]�p�����]�N�I�[�^�j�I�������߂�
            XMVECTOR q;
            if (point.y > target.y)
            {
                q = XMQuaternionRotationAxis(right, an);//���̕����ɓ����N�I�[�^�j�I��
            }
            else\
            {
                q = XMQuaternionRotationAxis(right, -an);//���̕����ɓ����N�I�[�^�j�I��
            }
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsed_time);
        }
    }
    DirectX::XMStoreFloat4(&orientation, orientation_vec);
}

void PlayerMove::PitchTurn(DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& camera_pos, const DirectX::XMFLOAT3& camera_forward, DirectX::XMFLOAT4& orientation,float elapsed_time)
{
    using namespace DirectX;

    //�^�[�Q�b�g�Ɍ������ĉ�]
    XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&orientation);
    DirectX::XMVECTOR f, right;
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);
    right = { m4x4._11, m4x4._12, m4x4._13 };
    f = { m4x4._31, m4x4._32, m4x4._33 };

    XMFLOAT3 forw{};
    XMStoreFloat3(&forw, f);
    forw.x = camera_forward.x;
    forw.z = camera_forward.z;


    XMVECTOR camera_forward_vec{ XMVector3Normalize(XMLoadFloat3(&camera_forward)) };
    XMVECTOR player_forward_vec{ XMVector3Normalize(XMLoadFloat3(&forw)) };

    DirectX::XMFLOAT3 point1 = Math::calc_designated_point(position, Math::Normalize(forw), Math::Length(Math::Normalize(forw)));
    DirectX::XMVECTOR point1_vec = DirectX::XMLoadFloat3(&point1);
    DirectX::XMVECTOR d = point1_vec - DirectX::XMLoadFloat3(&position);

    DirectX::XMFLOAT3 point2 = Math::calc_designated_point(position, Math::Normalize(camera_forward), Math::Length(Math::Normalize(camera_forward)));
    DirectX::XMVECTOR point2_vec = DirectX::XMLoadFloat3(&point2);
    DirectX::XMVECTOR d2 = point2_vec - DirectX::XMLoadFloat3(&position);

    float an;
    XMVECTOR dot{ XMVector3Dot(d,d2) };
    //XMVECTOR dot{ XMVector3Dot(player_forward_vec,camera_forward_vec) };
    XMStoreFloat(&an, dot);
    an = acosf(an);

    if (fabs(an) > DirectX::XMConvertToRadians(10.0f) && fabs(an) < DirectX::XMConvertToRadians(170.0f))
    {
        //��]���Ɖ�]�p�����]�N�I�[�^�j�I�������߂�
        XMVECTOR q;
        if (point2.y < point1.y)
        {
            q = XMQuaternionRotationAxis(right, an);//���̕����ɓ����N�I�[�^�j�I��
        }
        else
        {
            q = XMQuaternionRotationAxis(right, -an);//
        }
        XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
        orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsed_time);
    }
    DirectX::XMStoreFloat4(&orientation, orientation_vec);
}

void PlayerMove::RollTurn(DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation, float elapsed_time)
{
#if 0
    using namespace DirectX;
    //�^�[�Q�b�g�Ɍ������ĉ�]
    XMVECTOR player_orientation_vec = DirectX::XMLoadFloat4(&orientation);
    DirectX::XMVECTOR player_forward;
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(player_orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);
    player_forward = { m4x4._31, m4x4._32, m4x4._33 };

    XMFLOAT3 player_front{};
    XMStoreFloat3(&player_front, player_forward);

    DirectX::XMFLOAT4 initial_orientation{ 0,0,0,1.0f };
    XMVECTOR initial_orientation_vec = DirectX::XMLoadFloat4(&initial_orientation);
    DirectX::XMVECTOR initial_forward;
    DirectX::XMMATRIX i_m = DirectX::XMMatrixRotationQuaternion(initial_orientation_vec);
    DirectX::XMFLOAT4X4 i_m4x4 = {};
    DirectX::XMStoreFloat4x4(&i_m4x4, i_m);
    initial_forward = { i_m4x4._31, i_m4x4._32, i_m4x4._33 };

    XMFLOAT3 initial_front{};
    XMStoreFloat3(&initial_front, initial_forward);

    player_front.z = 0;
    initial_front.z = 0;

    XMFLOAT3 point1 = Math::calc_designated_point(position, Math::Normalize(player_front), Math::Length(Math::Normalize(player_front)));
    XMVECTOR point1_vec = XMLoadFloat3(&point1);
    XMVECTOR d = point1_vec - XMLoadFloat3(&position);

    XMFLOAT3 point2 = Math::calc_designated_point(position, Math::Normalize(initial_front), Math::Length(Math::Normalize(initial_front)));
    XMVECTOR point2_vec = XMLoadFloat3(&point2);
    XMVECTOR d2 = point2_vec - XMLoadFloat3(&position);

    float angle;
    XMVECTOR dot{ XMVector3Dot(d,d2) };
    XMStoreFloat(&angle, dot);
    angle = acosf(angle);

    if (fabs(angle) > XMConvertToRadians(10.0f))
    {
        XMVECTOR q;
        float cross{ (initial_front.x * player_front.z) - (initial_front.z * player_front.x) };
        if (cross > 0)
        {
            q = XMQuaternionRotationAxis(player_forward, angle);
        }
        else
        {
            q = XMQuaternionRotationAxis(player_forward, -angle);
        }
        XMVECTOR Q = XMQuaternionMultiply(player_orientation_vec, q);
        player_orientation_vec = XMQuaternionSlerp(player_orientation_vec, Q, 10.0f * elapsed_time);
    }
    DirectX::XMStoreFloat4(&orientation, player_orientation_vec);

#endif // 0

}


void PlayerMove::SetDirections(DirectX::XMFLOAT4 o)
{
    using namespace DirectX;
    //�^�[�Q�b�g�Ɍ������ĉ�]
    XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&o);
    DirectX::XMVECTOR forward;
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);
    forward = { m4x4._31, m4x4._32, m4x4._33 };

    XMStoreFloat3(&player_forward, forward);

}
