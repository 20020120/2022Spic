#include"Player.h"
#include"imgui_include.h"
#include"user.h"
#include"easing.h"

void Player::UpdateTutorial(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome)
{
    ExecFuncTutorialUpdate(elapsed_time, sky_dome);
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
    case Player::TutorialState::First:
        break;
    case Player::TutorialState::Second:
        TransitionTutorialSecond();
        break;
    case Player::TutorialState::Third:
        break;
    default:
        break;
    }
}

void Player::TutolialFirstUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::TutolialSecondUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::TransitionTutorialSecond()
{
    player_tutorial = &Player::TutolialSecondUpdate;
}
