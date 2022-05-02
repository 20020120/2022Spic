#include"Player.h"
#include"imgui_include.h"
#include"user.h"
#include"easing.h"

void Player::UpdateTutorial(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome)
{
    ExecFuncTutorialUpdate(elapsed_time, sky_dome);
    //velocity�X�V����
    UpdateTutorialVelocity(static_cast<int>(tutorial_state), elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    //�A�j���[�V�����X�V����
    if (is_update_animation)model->update_animation(elapsed_time);
}


void Player::ExecFuncTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{
    (this->*player_tutorial)(elapsed_time, sky_dome);
}

void Player::ChangeTutorialState(int state)
{
    //�������̃`���[�g���A���X�e�[�g�����������l�������Ă�����
    //�ЂƂO�ɂ��ǂ邩��͂���Ȃ��悤�ɂ���
    if (tutorial_state > static_cast<TutorialState>(state)) return;
    tutorial_state = static_cast<TutorialState>(state);
    //�����ɓ���͎̂��̃X�e�[�g�����ɂȂ邩��e��J�ڊ֐�������
    switch (tutorial_state)
    {
    case Player::TutorialState::MoveTutorial:
        break;
    case Player::TutorialState::AvoidanceTutorial:
        TransitionAvoidanceTutorial();
        break;
    case Player::TutorialState::LockOnTutorial:
        TransitionLockOnTutorial();
        break;
    case Player::TutorialState::AttackTutorial:
        TransitionAttackTutorial();
        break;
    case Player::TutorialState::BehindAvoidanceTutorial:
        TransitionBehindAvoidance();
        break;
    case Player::TutorialState::ChainAttackTutorial:
        TransitionChainAttackTutorial();
        break;
    case Player::TutorialState::AwaikingTutorial:
        TransitionAwaikingTutorial();
        break;
    default:
        break;
    }
}

void Player::MoveTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{

}

void Player::AvoidanceTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::LockOnTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::AttackTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::BehindAvoidanceTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::ChainAttackTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::AwaikingTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::TransitionAvoidanceTutorial()
{
    player_tutorial = &Player::AvoidanceTutorialUpdate;
}

void Player::TransitionLockOnTutorial()
{
    player_tutorial = &Player::LockOnTutorialUpdate;
}

void Player::TransitionAttackTutorial()
{
    player_tutorial = &Player::AttackTutorialUpdate;
}

void Player::TransitionBehindAvoidanceTutorial()
{
    player_tutorial = &Player::BehindAvoidanceTutorialUpdate;
}

void Player::TransitionChainAttackTutorial()
{
    player_tutorial = &Player::ChainAttackTutorialUpdate;
}

void Player::TransitionAwaikingTutorial()
{
    player_tutorial = &Player::AwaikingTutorialUpdate;
}
