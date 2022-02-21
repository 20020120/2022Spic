#pragma once
#include"MoveBehavior.h"
class PlayerMove : public MoveBehavior
{
public:
    PlayerMove();
    ~PlayerMove();
public:
    void UpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation , const DirectX::XMFLOAT3& camera_forward, const DirectX::XMFLOAT3& camera_right, SkyDome* sky_dome);
private:
    //�������͍X�V����
    void UpdateVerticalVelocity(float elapsedFrame);
    //�����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime, DirectX::XMFLOAT3& position, SkyDome* sky_dome);
    //�������͍X�V����
    void UpdateHrizontalVelocity(float elasedFrame);
    //�����ړ��X�V����
    void UpdateHorizontalMove(float elapsedTime, DirectX::XMFLOAT3& position, SkyDome* sky_dome);
    //�^�[�Q�b�g�Ɍ������ĉ�]
    void RotateToTarget(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);

protected:
    //�G�����b�N�I���������ǂ���
    bool is_lock_on{ false };
    //�G�����邩�ǂ���
    bool is_enemy{ false };
    //�^�[�Q�b�g
    DirectX::XMFLOAT3 target{};
private:
    //�v���C���[�̑O����
    DirectX::XMFLOAT3 player_forward{};
    //�����擾
    void SetDirections(DirectX::XMFLOAT4 o);
};