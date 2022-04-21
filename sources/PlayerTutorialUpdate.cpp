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
    //もし今のチュートリアルステートよりも小さい値が入ってきたら
    //ひとつ前にもどるからはいれないようにする
    if (tutorial_state > static_cast<TutorialState>(state)) return;
    tutorial_state = static_cast<TutorialState>(state);
    //ここに入るのは次のステートだけになるから各種遷移関数を書く
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
