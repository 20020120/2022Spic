#pragma once
#include"BasePlayer.h"
#include"PlayerMove.h"
#include"collision.h"
#include"BaseEnemy.h"
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
    //���b�N�I���ł��鋗��
    static constexpr float LOCK_ON_LANGE = 70.0f;
    //���ɉ�荞�߂鋗��
    static constexpr float  BEHIND_LANGE = 20.0f;
    //�U���͂̍Œ�l
    static const int MIN_PLAYER_ATTACK_POWER = 1;
    //�U���͂̍ő�l
    static const int MAX_PLAYER_ATTACK_POWER = 100;
private:
    DirectX::XMFLOAT3 camera_forward{};//�J�����̑O����
    DirectX::XMFLOAT3 camera_right{};//�J�����̉E����
    DirectX::XMFLOAT3 camera_position{};//�J�����̉E����
private:
    //�v���C���[�̊e����
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up;
private:
    //�J�����̌��������Z�b�g
    bool camera_reset{ false };
    //�G�����b�N�I���������ǂ���(�J�����p�̃��b�N�I���������ǂ���)
    bool is_camera_lock_on{ false };
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
    //����u�[�X�g�J�n�Ɠ��B
    DirectX::XMFLOAT3 avoidance_start{};
    DirectX::XMFLOAT3 avoidance_end{};
    float avoidance_easing_time{ 0.6f };
    //�u�[�X�g�̔{��
    float leverage{ 15.0f };
    //�J�����p�̉�������u��
    bool is_avoidance{ false };
private:
    //�^�[�Q�b�g�̓G
    const  BaseEnemy* target_enemy;
private:
    //���ɉ�荞�ނ��߂̌v�Z����֐�
    void BehindAvoidancePosition();
    //�X�v���C���Ȑ����g�����߂̓r���̓_
    DirectX::XMFLOAT3 behind_point_0{};//�X�^�[�g
    DirectX::XMFLOAT3 behind_point_1{};//���p�n�_
    DirectX::XMFLOAT3 behind_point_2{};//���p�n�_
    DirectX::XMFLOAT3 behind_point_3{};//�S�[��
    std::vector<DirectX::XMFLOAT3> behind_point{};
    std::vector<DirectX::XMFLOAT3> behind_point_create{};
    void InterpolateCatmullRomSpline(float elapsed_time);
    //�w��ɉ�荞�ނƂ��ɐi�ރ^�C�}�[
    float behind_timer{};
    //�w��ɉ�荞�ނƂ��̃��[�g
    float behind_late{};
private:
    //�v���C���[�̍U����(�R���{�ɂ���ĕω����Ă���)
    int player_attack_power{ MIN_PLAYER_ATTACK_POWER };
    //�R���{��
    int combo_count{ 0 };
    //�v���C���[�����U�����������łȂ���
    bool is_attack{ false };
    //�v���C���[�̗̑�
    int player_health = 100;
    //���G����
    float invincible_timer{};
    //�R���{�̎�������
    float duration_combo_timer{};
private:
    //�U���͂̕ω�
    void InflectionPower(float elapsed_time);
    //�R���{�̕ω�
    void InflectionCombo(float elapsed_time);
private:
    //�J�v�Z���G�Ƃ̓����蔻��
    struct CapsuleParam
    {
        DirectX::XMFLOAT3 start{};
        DirectX::XMFLOAT3 end{};
        float rasius{ 1.2f };
    };

    CapsuleParam sword_capsule_param{};
    CapsuleParam body_capsule_param{};

    DirectX::XMFLOAT3 capsule_body_start{0,2.6f,0};
    DirectX::XMFLOAT3 capsule_body_end{0,0.2f,0};

    void BodyCapsule();
    //���̃J�v�Z������
    void SwordCapsule();
public:
    DirectX::XMFLOAT3 GetForward() { return forward; }
    DirectX::XMFLOAT3 GetRight() { return right; }
    DirectX::XMFLOAT3 GetUp() { return up; }
    DirectX::XMFLOAT3 GetPosition() { return position; }
    DirectX::XMFLOAT3 GetVelocity() { return velocity; }
    HitResult& GetPlayerHitResult() { return hit; }
    bool GetCameraReset() { return camera_reset; }
    bool GetCameraLockOn() { return is_camera_lock_on; }
    bool GetEnemyLockOn() { return is_lock_on; }
    bool GetAvoidance() { return is_avoidance; }
    bool GetIsPlayerAttack() { return is_attack; }
    CapsuleParam GetBodyCapsuleParam() { return body_capsule_param; }
    CapsuleParam GetSwordCapsuleParam() { return sword_capsule_param; }
    void SetRaycast(bool r) { raycast = r; }
    int GetPlayerPower() { return player_attack_power; }
    const  BaseEnemy* GetPlayerTargetEnemy() const
    {
        if (target_enemy != nullptr && target_enemy->fGetIsFrustum())
        {
            return target_enemy;
        }
        return nullptr;
    }
    //��ԋ߂��G�������ė��Ă��̈ʒu���Z�b�g����
    void SetTarget(const BaseEnemy* target_enemy);
    DirectX::XMFLOAT3 GetTarget() { return target; };
    void AddCombo(int count);
public:
    void FalseCameraReset() { camera_reset = false; }
    void FalseCameraLockOn() { is_camera_lock_on = false; }
    void FalseAvoidance() { is_avoidance = false; }
private:
    void GetPlayerDirections();
public:
    void SetCameraDirection(const DirectX::XMFLOAT3& c_forward,  const DirectX::XMFLOAT3& c_right)
    {
        camera_forward = c_forward;
        camera_right = c_right;
    }
    void SetCameraPosition(DirectX::XMFLOAT3 p) { camera_position = p; }
private:
    //����̉���
    void AvoidanceAcceleration(float elapse_time);
    //�ːi�̉���(���`���)
    void ChargeAcceleration(float elapse_time);
private:
    //���b�N�I��
    void LockOn();
    //�J�������Z�b�g
    void CameraReset();
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
    void BehindAvoidanceUpdate(float elapsed_time, SkyDome* sky_dome);//���ɉ�荞�މ���̍X�V����
    void ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome);//�ːi�J�n�A�j���[�V�������̍X�V����
    void ChargeUpdate(float elapsed_time, SkyDome* sky_dome);//�ːi���̍X�V����
    void AttackType1Update(float elapsed_time, SkyDome* sky_dome);//�U��1���ڂ̍X�V����
    void AttackType2Update(float elapsed_time, SkyDome* sky_dome);//�U��2���ڂ̍X�V����
    void AttackType3Update(float elapsed_time, SkyDome* sky_dome);//�U��3���ڂ̍X�V����

    //�A�j���[�V�����J��(1frame���������Ă΂Ȃ�����)
    void TransitionIdle();
    void TransitionMove();
    void TransitionAvoidance();
    void TransitionBehindAvoidance();//�w��ɉ�荞�މ��
    void TransitionChargeInit();
    void TransitionCharge();
    void TransitionAttackType1(float blend_seconds);
    void TransitionAttackType2(float blend_seconds);
    void TransitionAttackType3(float blend_seconds);
};