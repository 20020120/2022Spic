#include"MiddleBoss.h"

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

    // �ʒu�v�Z
    mPosition = {
        sinf(mTourRadian) * TourLength,
        mPosition.y ,
        cosf(mTourRadian) * TourLength
    };

    // �p�x���X�V
    mTourRadian += elapsedTime_;
 
}

//--------------------<�r�[���̏���>--------------------//
void MiddleBoss::fTourLaserReadyInit()
{
    // �Ə���������
    mLaserPointerLength = 0.0f;
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
        return;
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
            // 
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
  
}
void MiddleBoss::fTourLaserUpdate(float elapsedTime_, GraphicsPipeline& Graphic_)
{
    
}

//--------------------<�P���ˌ�>--------------------//
void MiddleBoss::fTourShotInit()
{
    
}
void MiddleBoss::fTourShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    
}
