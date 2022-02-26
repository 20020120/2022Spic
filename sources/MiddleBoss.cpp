#include"MiddleBoss.h"

MiddleBoss::MiddleBoss(GraphicsPipeline& Graphics_, std::function<EnemyData(std::string)> Function_)
    :BaseEnemy(Graphics_,0, "./resources/Models/Enemy/enemy_proto.fbx")
{
    mScale = { 0.03f,0.03f,0.03f };
    mPosition = { 0.0f,0.0f,0.0f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    fGetParam(this, Function_);
    fRegisterFunctions();
}

void MiddleBoss::fInitialize()
{
    
}
void MiddleBoss::fUpdate(float elapsedTime_)
{
    fUpdateBase(elapsedTime_);
}

void MiddleBoss::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Begin("MiddleBoss");
    ImGui::DragFloat("Position", &mPosition.x);
    ImGui::End();
#endif
}

void MiddleBoss::fDamaged(int damage, float invinsible_time)
{
    BaseEnemy::fDamaged(damage, invinsible_time);
}

void MiddleBoss::fSetCapsulePoint()
{
    BaseEnemy::fSetCapsulePoint();
}

void MiddleBoss::fRegisterFunctions()
{
    {
        auto Ini = [=]()->void
        {
            fStartInit();
        };
        auto up = [=](float elapsedTime_)->void
        {
            fStartUpdate(elapsedTime_);
        };

        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::Start, tuple));
    }
    {
        auto Ini = [=]()->void
        {
            fTourInit();
        };
        auto up = [=](float elapsedTime_)->void
        {
            fTourUpdate(elapsedTime_);
        };

        FunctionTuple tuple = std::make_tuple(Ini, up);
        mFunctionMap.insert(std::make_pair(State::Tour, tuple));
    }
    fChangeState(State::Tour);
}

void MiddleBoss::fStartInit()
{
  // �{�X�o�ꎞ�̃C�x���g�V�[���i�������j
}

void MiddleBoss::fStartUpdate(float elapsedTime_)
{
    // �{�X�o�ꎞ�̃C�x���g�V�[���i�������j
}

void MiddleBoss::fTourInit()
{
    // �X�e�[�W�̎��͂����񂷂铮��
    mPosition = { 0.0f,150.0f,150.0f };
}

void MiddleBoss::fTourUpdate(float elapsedTime_)
{
    // �X�e�[�W�̌��_�𒆐S�ɂ��邮����

    // �ʒu�v�Z
    mPosition = {
        sinf(mTourRadian) * TourLength,
        mPosition.y ,
        cosf(mTourRadian) * TourLength
    };

    // �p�x���X�V
    mTourRadian += elapsedTime_;
}
