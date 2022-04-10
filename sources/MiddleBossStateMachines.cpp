#include"MiddleBoss.h"
#include"Operators.h"
#include"StraightBullet.h"
//****************************************************************
//
// ���^�̃{�X�̃X�e�[�g�}�V�����`����
//
//****************************************************************


void MiddleBoss::fStartInit()
{
    // �{�X�o�ꎞ�̃C�x���g�V�[���i�������j
}

void MiddleBoss::fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // �{�X�o�ꎞ�̃C�x���g�V�[���i�������j
}

void MiddleBoss::fTourInit()
{
    // �X�e�[�W�̎��͂����񂷂铮��
    mPosition = { 0.0f,150.0f,150.0f };

}

void MiddleBoss::fTourUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // ��]�O�̊�x�N�g��
    DirectX::XMFLOAT3 baseForward{ 0.0f,0.0f,1.0f };
    // ��]��̃x�N�g��
    DirectX::XMFLOAT3 forward{};

    DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(0.0f, 150.0f, 0.0f) - mPosition;
    up = Math::Normalize(up);

    // �X�e�[�W�̌��_�𒆐S�ɂ��邮����
    mLuaWorld.fDestroyStack();
    mLuaWorld.fSetFunction("fTourMove");
    mLuaWorld.fSetNumeric(elapsedTime_);
    mLuaWorld.fCallFunc(1, 4);

    // �l��Lua����󂯎��
    mPosition.x = static_cast<float>(mLuaWorld.fGetDouble(1));
    mPosition.z = static_cast<float>(mLuaWorld.fGetDouble(2));
    forward.x = static_cast<float>(mLuaWorld.fGetDouble(3));
    forward.z = static_cast<float>(mLuaWorld.fGetDouble(4));

    // ���K��
    baseForward = Math::Normalize(baseForward);
    forward = Math::Normalize(forward);

    // ��]�ʂ��Z�o
    float dot = Math::Dot(baseForward, forward);
    dot = acosf(dot);

    // �O�Ə�̃x�N�g�������]�����Z�o
    auto rotAxis = Math::Cross(baseForward, forward, true);

    mOrientation = Math::RotQuaternion(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), rotAxis, dot);


    //// �e�U���ɑJ�ڂ���Ƃ��̏���
    //// �T�b���Ƃɒ��I
    //if(static_cast<int>(mTimer)%5==0)
    //{
    //    const int selectedNum = rand() % 4;
    //    if(selectedNum==0)
    //    {
    //        fChangeState(State::TourBeamReady);
    //    }
    //    else if(selectedNum<=2)
    //    {
    //        fChangeState(State::TourShot);
    //    }

    //}


}

//--------------------<�r�[���̏���>--------------------//
void MiddleBoss::fTourLaserReadyInit()
{
    // �Ə���������
    mLaserBeamLength = 0.0f;
    mLaserShotTimer = 0.0f;
    mLuaWorld.fSetFunction("fTourLaserReadyInit");
    mLuaWorld.fCallFunc(0, 1);
}
void MiddleBoss::fTourLaserReadyUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mLuaWorld.fDestroyStack();
    mLuaWorld.fSetFunction("fTourLaserReady");
    mLuaWorld.fSetNumeric(elapsedTime_);
    mLuaWorld.fCallFunc(1, 3);

    //mLuaWorld.fDebugShowStack();
    // �|�C���^�[�̓����x���X�V
    mLaserPointer.fSetAlpha(mLuaWorld.fGetDouble(-2));
    // �|�C���^�[�̒������X�V
    mLaserPointer.fSetLengthThreshold(mLuaWorld.fGetDouble(-1));

    // �����𖞂����Ă�����X�e�[�g��ύX
    if (mLuaWorld.fGetBool(-3))
    {
        fChangeState(State::TourBeam);
        mLaserPointer.fSetLengthThreshold(0.0f);
    }
}
//--------------------<�r�[������>--------------------//
void MiddleBoss::fTourLaserInit()
{
    mLaserPointerLength = 0.0f;
    mLaserBeamLength = 0.0f;
    mLaserShotTimer = 0.0f;
    mBeamRadius = 3.0f;
    // �I�_���v�Z
    auto endPoint = mPlayerPosition;
    endPoint.y += 1.5f;

    // �x�N�g�����Z�o
    DirectX::XMFLOAT3 v = endPoint - mPosition;
    v = Math::Normalize(v);
    endPoint = mPosition + (v * 500.0f);

    mLaserBeam.fSetPosition(mPosition, endPoint);
}
void MiddleBoss::fTourLaserUpdate(float elapsedTime_, GraphicsPipeline& Graphic_)
{
    // �r�[����L�΂�
    if (mLaserBeamLength < 1.0f)
    {
        mLaserBeamLength += (elapsedTime_ * 5.0f);
    }
    else // �ő�Ȃ�Œ�
    {
        mLaserBeamLength = 1.0f;
    }


    mLaserShotTimer += elapsedTime_;
    if(mLaserShotTimer>=5.0f)
    {
        fChangeState(State::Tour);
    }
    else if(mLaserShotTimer>=3.0f)
    {
        if (mBeamRadius > 0.0f)
        {
            mBeamRadius -= (elapsedTime_ * 10.0f);
        }
        else
        {
            mBeamRadius = 0.0f;
        }
    }
}

//--------------------<�P���ˌ�>--------------------//
void MiddleBoss::fTourShotInit()
{
    // ������
}
void MiddleBoss::fTourShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // �ˌ��������Z�o
    fShotToTarget(mPlayerPosition,Graphics_);
    fChangeState(State::Tour);

}

void MiddleBoss::fTourToSecondModeInit()
{
    // �Ăяo���ꂽ�G�����Ȃ��Ȃ�����

    // �ʒu���X�e�[�W���ʂɈړ�
    mPosition = { 0.0f,0.0f,150.0f };

    // �ό`
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::inv_transform);
}

void MiddleBoss::fTourToSecondModeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // �ό`���I��������ҋ@��ԂɑJ��
    if (mpSkinnedMesh->end_of_animation(mAnimPara))
    {
        fChangeState(State::SecondIdle);
    }
}

void MiddleBoss::fSecondIdleInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::wait, true);
}

void MiddleBoss::fSecondIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

}

void MiddleBoss::fSecondShotInit()
{
    mpSkinnedMesh->play_animation(mAnimPara, AnimationName::Shot_Recoil);
    mShotTimer = 0.0f;

}

void MiddleBoss::fSecondShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mShotTimer += elapsedTime_;

    if (mShotTimer > 0.6f)
    {
        fShotToTarget(mPlayerPosition,Graphics_);
        // ���̏�ԂɑJ�ڂ����邩�ǂ����̃t���O
        mLuaWorld.fSetFunction("SelectedNextState");
        mLuaWorld.fCallFunc(0, 1);
        const bool goNextState = mLuaWorld.fGetBool(-1);

        fChangeState(goNextState ? State::SecondIdle : State::SecondShot);
    }
}
