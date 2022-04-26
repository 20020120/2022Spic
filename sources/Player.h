#pragma once
#include<memory>
#include"BasePlayer.h"
#include"PlayerMove.h"
#include"collision.h"
#include"BaseEnemy.h"
#include"player_config.h"
#include"SwordTrail.h"
#include"post_effect.h"
#include"graphics_pipeline.h"
#include "Common.h"
class Player :public BasePlayer, private PlayerMove
{
public:
    Player(GraphicsPipeline& graphics);
    ~Player();
private:
    enum AnimationClips
    {
        Idle,//�ҋ@
        TransformWing,//��s�@���[�h�ɂȂ�
        IdleWing,//�ό`�ҋ@
        TransformHum,//�l�^���[�h�ɂȂ�
        Move,//�ړ�
        Avoidance,//���
        ChargeInit,//�ːi�J�n
        Charge,//�ːi��
        AttackType1,//�U��1����
        AttackType2,//�U��2����
        AttackType3,//�U��3����
        Damage,//�_���[�W
        Awaking,//�o����ԂɂȂ�
        InvAwaking,//�ʏ��ԂɂȂ�
        AwakingIdle,//�o����Ԃ̑ҋ@
        AwakingMove,//�o����Ԃ̈ړ�
        AwakingAvoidance,//�o����Ԃ̉��
        AwakingChargeInit,//�o����Ԃ̓ːi�J�n
        AwakingCharge,//�o����Ԃ̓ːi
        AwakingAttackType1,//�o����Ԃ̍U��1����
        AwakingAttackType2,//�o����Ԃ̍U��2����
        AwakingAttackType3,//�o����Ԃ̍U��3����
        AwakingDamage,//�o����Ԃ̃_���[�W
    };
public:
    void Initialize()override;
    void Update(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome)override;
    //�`���[�g���A���p�̃A�b�v�f�[�g
    void UpdateTutorial(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome);
    void Render(GraphicsPipeline& graphics, float elapsed_time)override;
private:
    //�ːi����
    static constexpr float MAX_HEALTH = 50.0f;
    //�ːi����
    static constexpr float CHARGE_MAX_TIME = 1.0f;
    //�U��1���ڂ̗P�\����
    static constexpr float ATTACK_TYPE1_MAX_TIME = 0.7f;
    //�U��2���ڂ̗P�\����
    static constexpr float ATTACK_TYPE2_MAX_TIME = 0.7f;
    //���b�N�I���ł��鋗��
    static constexpr float LOCK_ON_LANGE = 100.0f;
    //���ɉ�荞�߂鋗��
    static constexpr float  BEHIND_LANGE_MAX = 45.0f;
    static constexpr float  BEHIND_LANGE_MIN = 20.0f;
    //�U���͂̍Œ�l
    static const int MIN_PLAYER_ATTACK_POWER = 1;
    //�U���͂̍ő�l
    static const int MAX_PLAYER_ATTACK_POWER = 20;
    //�R���{�̍ő吔
    static constexpr float MAX_COMBO_COUNT = 100.0f;
    //����̎��̃A�j���[�V�����X�s�[�h
    static constexpr float AVOIDANCE_ANIMATION_SPEED = 1.0f;
    //�ːi�J�n�̎��̃A�j���[�V�����X�s�[�h
    static constexpr float CHARGEINIT_ANIMATION_SPEED = 1.5f;
    //�ːi�̎��̃A�j���[�V�����X�s�[�h
    static constexpr float CHARGE_ANIMATION_SPEED = 1.0f;
    //�U��1�̎��̃A�j���[�V�����X�s�[�h
    static constexpr float ATTACK1_ANIMATION_SPEED = 2.0f;
    //�U��2�̎��̃A�j���[�V�����X�s�[�h
    static constexpr float ATTACK2_ANIMATION_SPEED = 2.0f;
    //�U��3�̎��̃A�j���[�V�����X�s�[�h
    static constexpr float ATTACK3_ANIMATION_SPEED = 1.5f;
    //�Q�[�W�����(�ːi)
    static constexpr float GAUGE_CONSUMPTION = 5.0f;
    //�l�^�ɖ߂�Ƃ��̃A�j���[�V�����X�s�[�h
    static constexpr float TRANSFORM_HUM_ANIMATION_SPEED = 2.0f;
    //��s�@���[�h�ɂȂ�Ƃ��̃A�j���[�V�����X�s�[�h
    static constexpr float TRANSFORM_WING_ANIMATION_SPEED = 2.0f;


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
    //�U���̎���
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
    //��荞�݉�����ǂ���
    bool is_behind_avoidance{ false };
    //�|�����G�̈ʒu��ۑ�
    DirectX::XMFLOAT3 old_target{};
    //�J�����̕�Ԃ̃S�[���n�_
    DirectX::XMFLOAT3 end_target{};
    //��ԗ�
    float target_lerp_rate{ 0 };
    //���b�N�I�����Ă��Ȃ��Ƃ��̃J�����̃^�[�Q�b�g
    DirectX::XMFLOAT3 camera_target{};
    //�J�����̃^�[�Q�b�g�̕��
    void LerpCameraTarget(float elapsed_time);

private:
    //�^�[�Q�b�g�̓G
    const  BaseEnemy* target_enemy;
    float enemy_length{ 0 };//�G�ƃv���C���[�̋���
    int target_count{ 0 };
    int old_target_count{ 0 };
private:
    //���ɉ�荞�ނ��߂̌v�Z����֐�
    void BehindAvoidancePosition();
    //�X�v���C���Ȑ����g�����߂̓r���̓_
    DirectX::XMFLOAT3 behind_point_0{};//�X�^�[�g
    DirectX::XMFLOAT3 behind_point_1{};//���p�n�_
    DirectX::XMFLOAT3 behind_point_2{};//���p�n�_
    DirectX::XMFLOAT3 behind_point_3{};//�S�[��
    //�w��ɉ�荞�ޓ_
    std::vector<DirectX::XMFLOAT3> behind_point{};
    void InterpolateCatmullRomSpline(float elapsed_time);
    //�w��ɉ�荞�ނƂ��ɐi�ރ^�C�}�[
    float behind_timer{};
    //�w��ɉ�荞�ނƂ��̃��[�g
    float behind_late{};
private:
    //�v���C���[�̍U����(�R���{�ɂ���ĕω����Ă���)
    int player_attack_power{ MIN_PLAYER_ATTACK_POWER };
    //�R���{��
    float combo_count{ 0 };
    //�Q�[�W����̓ːi���ɓ���������
    float special_surge_combo_count{ 0 };
    //�v���C���[�����U�����������łȂ���
    bool is_attack{ false };
    //�v���C���[���Q�[�W����̓ːi�����Ă��邩
    bool is_special_surge{ false };
    //�Q�[�W����̓ːi�̌�
    float special_surge_opportunity{ 1.5f };
    float special_surge_timer{ 0 };
    float opportunity_timer{ 0 };
    //�v���C���[�̗̑�
    int player_health = 50;
    //�v���C���[���񂾂��ǂ���
    bool is_alive{ true };
    //���G����
    float invincible_timer{};
    //�R���{�̎�������
    float duration_combo_timer{};
    //���b�N�I�����Ă��Ȃ��Ƃ��̓ːi�̃^�[�Q�b�g
    DirectX::XMFLOAT3 charge_point{};
    //�_�b�V���G�t�F�N�g�J�n
    bool start_dash_effect{ false };
    //�_�b�V���G�t�F�N�g�I��
    bool end_dash_effect{ false };
    //�o����Ԃ��ǂ���
    bool is_awakening{ false };
    //�ːi�����ǂ���
    bool is_charge{ false };
    //�v���C���[�̃A�j���[�V�����X�s�[�h
    float animation_speed{ 1.0f };
    //�ːi�̉����p��velocity
    DirectX::XMFLOAT3 acceleration_velocity;
    //�ːi��velocity�̕�Ԃ̃��[�g
    float lerp_rate{ 1.0f };
    //�ːi�̃^�[�Q�b�g�܂ł̋����̔{��(�ǂꂾ���L�΂���)
    float charge_length_magnification{ 75.0f };
    //�U���̃A�j���[�V�����X�s�[�h�̃f�o�b�O�p
    DirectX::XMFLOAT4 attack_animation_speeds{ 1.0f,1.0f,1.0f,1.0f };
    //�U���̃A�j���[�V������Ԃ̎���
    DirectX::XMFLOAT4 attack_animation_blends_speeds{ 0.3f,0.0f,0.0f,0.0f };
    //�A�j���[�V���������Ă������ǂ���
    bool is_update_animation{ true };
    //�v���C���[�̃p�����[�^
    std::unique_ptr<PlayerConfig> player_config{ nullptr };
    //--------------------<SwordTrail�`���̋O�Ձ`>--------------------//
    SwordTrail mSwordTrail[2]{};
    float mTrailEraseTimer{};


    skeleton::bone player_bones[8];
private:
    //�v���C���[�̃p�����[�^�̕ω�
    void InflectionParameters(float elpased_time);
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

    CapsuleParam sword_capsule_param[2]{};
    CapsuleParam charge_capsule_param{};
    CapsuleParam body_capsule_param{};

    DirectX::XMFLOAT3 capsule_body_start{ 0,2.6f,0 };
    DirectX::XMFLOAT3 capsule_body_end{ 0,0.2f,0 };

    void BodyCapsule();
    //���̃J�v�Z������
    void SwordCapsule();
public:
    void SetEndDashEffect(bool a) { end_dash_effect = a; }
    void SetCameraTarget(DirectX::XMFLOAT3 p) { camera_target = p; }
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
    bool GetIsSpecialSurge() { return is_special_surge; }
    bool GetStartDashEffect() { return start_dash_effect; }
    bool GetEndDashEffect() { return end_dash_effect; }
    bool GetIsAwakening() { return is_awakening; }
    bool GetIsAlive() { return is_alive; }
    CapsuleParam GetBodyCapsuleParam() { return body_capsule_param; }
    CapsuleParam GetSwordCapsuleParam(int i)
    {
        if (is_charge)
        {
            return charge_capsule_param;
        }
        if (is_awakening)
        {
            return sword_capsule_param[i];
        }
        return sword_capsule_param[0];
    }
    std::vector<DirectX::XMFLOAT3> GetBehindPoint() {return behind_point;}
    void SetRaycast(bool r) { raycast = r; }
    int GetPlayerPower() { return player_attack_power; }
    [[nodiscard("Not used")]] const AddDamageFunc GetDamagedFunc() { return damage_func; }

    [[nodiscard]] const  BaseEnemy* GetPlayerTargetEnemy() const
    {
        if (target_enemy != nullptr && target_enemy->fComputeAndGetIntoCamera())
        {
            return target_enemy;
        }
        return nullptr;
    }
    //��ԋ߂��G�������ė��Ă��̈ʒu���Z�b�g����
    void SetTarget(const BaseEnemy* target_enemy);
    DirectX::XMFLOAT3 GetTarget() { return target; };
    void AddCombo(int count);
    //--------------------<�G����_���[�W���󂯂�>--------------------//
    void DamagedCheck(int damage, float InvincibleTime);
    void PlayerKnocKback(float elapsed_time);
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
    //�U���̉����̐ݒ�
    void SetAccelerationVelocity();
    //�Q�[�W����̓ːi
    void SpecialSurgeAcceleration();
private:
    //���b�N�I��
    void LockOn();
    //�`�F�C���U���̎��̃��b�N�I��
    void ChainLockOn();
    //�J�������Z�b�g
    void CameraReset();
private:
    enum class Behavior
    {
        //�ʏ���
        Normal,
        //�X�^�������G�ɍU��������
        Chain
    };
    Behavior behavior_state{ Behavior::Normal };
private:
    //-----------�A�j���[�V�����Ɋ֌W����֐�,�ϐ�------------//
    //�A�j���[�V�����J�ڂ̊֐��|�C���^//
    //�֐��|�C���^
    typedef void(Player::* PlayerActivity)(float elapsed_time, SkyDome* sky_dome);
    //�֐��|�C���^�̕ϐ�
    PlayerActivity player_activity = &Player::IdleUpdate;
    AddDamageFunc damage_func;
    //�����̃����o�֐��̊֐��|�C���^���Ă�
    void ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome);
    //�ҋ@�A�j���[�V�������̍X�V����
    void IdleUpdate(float elapsed_time, SkyDome* sky_dome);
    //�ړ��A�j���[�V�������̍X�V����
    void MoveUpdate(float elapsed_time, SkyDome* sky_dome);
    //����A�j���[�V�������̍X�V����
    void AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome);
    //���ɉ�荞�މ���̍X�V����
    void BehindAvoidanceUpdate(float elapsed_time, SkyDome* sky_dome);
    //�ːi�J�n�A�j���[�V�������̍X�V����
    void ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome);
    //�ːi���̍X�V����
    void ChargeUpdate(float elapsed_time, SkyDome* sky_dome);
    //�U��1���ڂ̍X�V����
    void AttackType1Update(float elapsed_time, SkyDome* sky_dome);
    //�U��2���ڂ̍X�V����
    void AttackType2Update(float elapsed_time, SkyDome* sky_dome);
    //�U��3���ڂ̍X�V����
    void AttackType3Update(float elapsed_time, SkyDome* sky_dome);
    //�Q�[�W�����ːi
    void SpecialSurgeUpdate(float elapsed_time, SkyDome* sky_dome);
    //�Q�[�W����ːi���I����Ă���̌�
    void OpportunityUpdate(float elapsed_time, SkyDome* sky_dome);
    //�_���[�W�󂯂��Ƃ�
    void DamageUpdate(float elapsed_time, SkyDome* sky_dome);
    //�l�^�ɖ߂�
    void TransformHumUpdate(float elapsed_time, SkyDome* sky_dome) ;
    //��s�@���[�h
    void TransformWingUpdate(float elapsed_time, SkyDome* sky_dome);
    //�o����Ԃɕό`����Ƃ��̍X�V
    void AwakingUpdate(float elapsed_time, SkyDome* sky_dome);
    //�ʏ��Ԃɕό`����Ƃ��̍X�V
    void InvAwakingUpdate(float elapsed_time, SkyDome* sky_dome);
    //�X�e�[�W�ړ��̎��̍X�V
    void StageMoveUpdate(float elapsed_time, SkyDome* sky_dome);

    void Awaiking();//�o����Ԃ�ON,OFF
    //�A�j���[�V�����J��(1frame���������Ă΂Ȃ�����)
public:
    //�ҋ@�ɑJ��
    void TransitionIdle(float blend_second = 0.3f);
private:
    //�ړ��ɑJ��
    void TransitionMove(float blend_second = 0.3f);
    //����ɑJ��
    void TransitionAvoidance();
    //�w��ɉ�荞�މ���ɑJ��
    void TransitionBehindAvoidance();
    //�ːi�J�n�ɑJ��
    void TransitionChargeInit();
    //�ːi�ɑJ��
    void TransitionCharge(float blend_seconds = 0.3f);
    //�P���ڂɑJ��
    void TransitionAttackType1(float blend_seconds = 0.3f);
    //�Q���ڂɑJ��
    void TransitionAttackType2(float blend_seconds = 0.3f);
    //�R���ڂɑJ��
    void TransitionAttackType3(float blend_seconds = 0.3f);
    //�Q�[�W����̓ːi�ɑJ��
    void TransitionSpecialSurge();
    //�Q�[�W����̓ːi��̌��ɑJ��
    void TransitionOpportunity();
    //�_���[�W�󂯂��Ƃ��ɑJ��
    void TransitionDamage();
    //�l�^�ɕό`�ɑJ��
    void TransitionTransformHum();
    //��s�@���[�h�ɑJ��
    void TransitionTransformWing();
    //�o����ԂɑJ��
    void TransitionAwaking();
    //�ʏ��ԂɑJ��
    void TransitionInvAwaking();
public:
    //�X�e�[�W�ړ��ɑJ��
    void TransitionStageMove();


private:
    //--------<�����p�[�g>--------//
    //�֐��|�C���^
    typedef void(Player::* PlayerChainActivity)(float elapsed_time);
    //�֐��|�C���^�̕ϐ�
    PlayerChainActivity player_chain_activity = &Player::chain_search_update;
    // ���G
    void chain_search_update(float elapsed_time);
    void transition_chain_search();
    // ���b�N�I��
    void chain_lockon_update(float elapsed_time);
    void transition_chain_lockon();
    // �ړ�
    void chain_move_update(float elapsed_time);
    void transition_chain_move();
    // �U��
    void chain_attack_update(float elapsed_time);
    void transition_chain_attack();

private:
    //------------------------------------------------------------------------------------------//
    //                        �`���[�g���A���Ɋւ���֐�,�ϐ�
    //------------------------------------------------------------------------------------------//
    //�֐��|�C���^
    typedef void(Player::* PlayerTutorial)(float elapsed_time, SkyDome* sky_dome);
    //�֐��|�C���^�̕ϐ�
    PlayerTutorial player_tutorial = &Player::TutolialFirstUpdate;
    //�`���[�g���A���̊֐��|�C���^���Ă�
    void ExecFuncTutorialUpdate(float elapsed_time, SkyDome* sky_dome);
public:
    //�`���[�g���A���̃X�e�[�g�ύX
    void ChangeTutorialState(int state);
private:
    //-------------------------------------�e��X�V����------------------------------------------//
    //1�ԍŏ��̃`���[�g���A���̍X�V����
    void TutolialFirstUpdate(float elapsed_time, SkyDome* sky_dome);
    //2�ԍŏ��̃`���[�g���A���̍X�V����
    void TutolialSecondUpdate(float elapsed_time, SkyDome* sky_dome);
    //--------------------------------------�e��J�ڏ���-----------------------------------------//
    //2�Ԗڂ̃`���[�g���A���̃X�e�[�g�ɑJ��
    void TransitionTutorialSecond();
    //0���ŏ��ő傫���Ȃ��Ă����悤�ɂ���
    enum class TutorialState
    {
        First = 1,
        Second,
        Third
    };
    TutorialState tutorial_state{ TutorialState::First };
};