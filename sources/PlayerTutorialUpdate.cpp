#include"Player.h"
#include"imgui_include.h"
#include"user.h"
#include"easing.h"

void Player::UpdateTutorial(float elapsed_time, GraphicsPipeline& graphics, SkyDome* sky_dome)
{
}


void Player::ExecFuncTutorialUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::ChangeTutorialState(int state)
{
    if(tutorial_state > static_cast<TutorialState>(state))
}
