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

    // �r�[���e�X�g
    auto endPoint = mPlayerPosition;
    endPoint.y += 1.5f;
    mLaserBeam.fSetPosition(mPosition, endPoint);
    mLaserBeam.fSetRadius(0.02f);
}