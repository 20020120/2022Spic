#pragma once
#include"BasePlayer.h"
#include"PlayerMove.h"
#include"collision.h"
#include"graphics_pipeline.h"

class Player :public BasePlayer, private PlayerMove
{
public:
    Player(GraphicsPipeline& graphics);
    ~Player();
private:
    enum AnimationClips
    {
        Idle,//�ҋ@
        Move,//�ړ�
        Avoidance,//���
        ChargeInit,//�ːi�J�n
        Charge,//�ːi��
        AttackType1,//�U��1����
        AttackType2,//�U��2����
        AttackType3,//�U��3����
    };
public:
    void Initialize()override;
    void Update(float elapsed_time, SkyDome* sky_dome)override;
    void Render(GraphicsPipeline& graphics, float elapsed_time)override;
private:
    //�ːi����
    static constexpr float CHARGE_MAX_TIME = 3.0f;
    //�U��1���ڂ̗P�\����
    static constexpr float ATTACK_TYPE1_MAX_TIME = 0.2f;
    //�U��2���ڂ̗P�\����
    static constexpr float ATTACK_TYPE2_MAX_TIME = 0.3f;
private:
    DirectX::XMFLOAT3 camera_forward{};//�J�����̑O����
    DirectX::XMFLOAT3 camera_right{};//�J�����̉E����
private:
    //�v���C���[�̊e����
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up;
private:
    //�J�����̌��������Z�b�g
    bool camera_reset{ false };
private:
    //�G�ɓ����������ǂ���
    bool is_enemy_hit{ false };
private:
    //�ːi���̎���(�ːi���n�܂�����^�C�}�[������)
    float charge_time{ 0 };
    //�ːi���̎��Ԃ��ǂꂾ�����₷��
    float charge_add_time{ 1.0f };
    //�U���̎���(�A�j���[�V�������I������瑝����)
    float attack_time{ 0 };
    //�U�����̎��Ԃ��ǂꂾ�����₷��
    float attack_add_time{ 1.0f };
private:
    //������n�܂�������
    float avoidance_boost_time{ 0 };
    //����
    float  avoidance_velocity{ 15.0f };
    //�C�[�W���O�̌��ʎ���
    float easing_time{ 1.0f };
private:
    //�v���C���[�̍U����(�R���{�ɂ���ĕω����Ă���)
    int player_attack_power{ 1 };
    //�R���{��
    int combo_count{ 0 };
public:
    DirectX::XMFLOAT3 GetForward() { return forward; }
    DirectX::XMFLOAT3 GetRight() { return right; }
    DirectX::XMFLOAT3 GetUp() { return up; }
    DirectX::XMFLOAT3 GetPosition() { return position; }
    DirectX::XMFLOAT3 GetVelocity() { return velocity; }
    HitResult& GetPlayerHitResult() { return hit; }
    bool GetCameraReset() { return camera_reset; }
    void SetRaycast(bool r) { raycast = r; }
    bool GetEnemyLockOn() { return is_lock_on; }
public:
    void FalseCameraReset() { camera_reset = false; }
private:
    void GetPlayerDirections();
public:
    void SetCameraDirection(const DirectX::XMFLOAT3& c_forward,  const DirectX::XMFLOAT3& c_right)
    {
        camera_forward = c_forward;
        camera_right = c_right;
    }
private:
    //����̉���
    void AvoidanceAcceleration(float elapse_time);
    //�ːi�̉���(���`���)
    void ChargeAcceleration(float elapse_time);
private:
    //���b�N�I��
    void LockOn();
private:
    //-----------�A�j���[�V�����Ɋ֌W����֐�,�ϐ�------------//

    //�A�j���[�V�����J�ڂ̊֐��|�C���^//
    //�֐��|�C���^
    typedef void(Player::* PlayerActivity)(float elapsed_time, SkyDome* sky_dome);
    //�֐��|�C���^�̕ϐ�
    PlayerActivity player_activity = &Player::IdleUpdate;
    //�����̃����o�֐��̊֐��|�C���^���Ă�
    void ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome);

    void IdleUpdate(float elapsed_time, SkyDome* sky_dome);//�ҋ@�A�j���[�V�������̍X�V����
    void MoveUpdate(float elapsed_time, SkyDome* sky_dome);//�ړ��A�j���[�V�������̍X�V����
    void AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome);//����A�j���[�V�������̍X�V����
    void ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome);//�ːi�J�n�A�j���[�V�������̍X�V����
    void ChargeUpdate(float elapsed_time, SkyDome* sky_dome);//�ːi���̍X�V����
    void AttackType1Update(float elapsed_time, SkyDome* sky_dome);//�U��1���ڂ̍X�V����
    void AttackType2Update(float elapsed_time, SkyDome* sky_dome);//�U��2���ڂ̍X�V����
    void AttackType3Update(float elapsed_time, SkyDome* sky_dome);//�U��3���ڂ̍X�V����

    //�A�j���[�V�����J��(1frame���������Ă΂Ȃ�����)
    void TransitionIdle();
    void TransitionMove();
    void TransitionAvoidance();
    void TransitionChargeInit();
    void TransitionCharge();
    void TransitionAttackType1(float blend_seconds);
    void TransitionAttackType2(float blend_seconds);
    void TransitionAttackType3(float blend_seconds);
};