#include "PlayerMove.h"

PlayerMove::PlayerMove()
    :MoveBehavior()
{
}

PlayerMove::~PlayerMove()
{
}

void PlayerMove::UpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation, const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right)
{
    DirectX::XMFLOAT3 movevec = SetMoveVec(camera_forward, camera_right);
    MovingProcess(movevec.x, movevec.z, move_speed);
    //���񏈗�
    Turn(elapsed_time, movevec.x, movevec.z, turn_speed, orientation);

    //�o�߃t���[��
    float elapsed_frame = 60.0f * elapsed_time;
    UpdateVerticalVelocity(elapsed_frame);
    UpdateVerticalMove(elapsed_time, position);
    UpdateHrizontalVelocity(elapsed_frame);
    UpdateHorizontalMove(elapsed_time, position);
}

void PlayerMove::UpdateVerticalVelocity(float elapsed_frame)
{
    //velocity.y += gravity * elapsed_frame;
}

void PlayerMove::UpdateVerticalMove(float elapsed_time, DirectX::XMFLOAT3& position)
{
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

void PlayerMove::UpdateHorizontalMove(float elapsed_time, DirectX::XMFLOAT3& position)
{
    using namespace DirectX;
    position.x += velocity.x * elapsed_time;
    position.y += velocity.y * elapsed_time;
    position.z += velocity.z * elapsed_time;

    //�����ړ��l
    float mx{ velocity.x * elapsed_time };
    float mz{ velocity.z * elapsed_time };

}
