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
    // �X�e�[�W�̌��_�𒆐S�ɂ��邮����
    mLuaWorld.fSetFunction("fTourMove");
    mLuaWorld.fSetNumeric(elapsedTime_);
    mLuaWorld.fCallFunc(1, 2);

    // �l��Lua����󂯎��
    mPosition.x = static_cast<float>(mLuaWorld.fGetDouble(-1));
    mPosition.z = static_cast<float>(mLuaWorld.fGetDouble(-2));

    // �e�U���ɑJ�ڂ���Ƃ��̏���
    // �T�b���Ƃɒ��I
    if(static_cast<int>(mTimer)%5==0)
    {
        const int selectedNum = rand() % 4;
        if(selectedNum==0)
        {
            fChangeState(State::TourBeamReady);
        }
        else if(selectedNum<=2)
        {
            fChangeState(State::TourShot);
        }

    }


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
    auto v = mPlayerPosition - mPosition;
    v = Math::Normalize(v);
    //v* TourBulletSpeed;
    mfAddFunc(new StraightBullet(Graphics_, mPosition, v));
    fChangeState(State::Tour);

}
