#include"MiddleBoss.h"
#include"Operators.h"
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
    lua_getglobal(mLuaState, "fTourMove");
    lua_pushnumber(mLuaState, static_cast<double>(elapsedTime_));

    ULua::fLuaCallFunc(mLuaState, 1, 2);

    // �l��Lua����󂯎��
    mPosition.x = lua_tonumber(mLuaState, -1);
    mPosition.z = lua_tonumber(mLuaState, -2);

}

//--------------------<�r�[���̏���>--------------------//
void MiddleBoss::fTourLaserReadyInit()
{
    // �Ə���������
    mLaserPointerLength = 0.0f;
    mLaserBeamLength = 0.0f;
    mLaserShotTimer = 0.0f;

}
void MiddleBoss::fTourLaserReadyUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    // �r�[���̏Ə���L�΂�
    if(mLaserPointerLength<1.0f)
    {
        mLaserPointerLength += elapsedTime_;
    }
    else // �ő�Ȃ�Œ�
    {
        mLaserPointerLength = 1.0f;
    }
    // �^�C�}�[���X�V����
    mLaserShotTimer += elapsedTime_;
    // ���Ԃ��߂����甭�˂���
    if (mLaserShotTimer >= TourLaserTimer)
    {
        fChangeState(State::TourBeam);
    }
    else
    {
        // �c�莞�Ԃɉ����ē_�ő��x��ς���
        if (TourLaserTimer - mLaserShotTimer > 3.0f)
        {
            // �x��
            switch (static_cast<int>(mLaserShotTimer*7) % 2)
            {
            case 0:
                mLaserPointer.fSetAlpha(1.0f);
                break;
            case 1:
                mLaserPointer.fSetAlpha(0.0f);
                break;
            default:;
            }
        }
        else
        {
            // ����
            switch (static_cast<int>(mLaserShotTimer * 20) % 2)
            {
            case 0:
                mLaserPointer.fSetAlpha(1.0f);
                break;
            case 1:
                mLaserPointer.fSetAlpha(0.0f);
                break;
            default:;
            }
        }
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
    
}
void MiddleBoss::fTourShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}
