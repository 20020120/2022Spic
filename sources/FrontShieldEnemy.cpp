#include "FrontShieldEnemy.h"
#include "Operators.h"
//    //�_���[�W���󂯂��Ƃ��ɐ��ʂ���̍U���Ȃ�,�V�[���h���h���Ń_���[�W������������
// int FrontShieldEnemy::fJudge_Front_Attacked(int damage_) const
//{
//    //�v���C���[�Ƃ̈ʒu�֌W�𔻒肵�A�U�����ꂽ�Ƃ��Ɏ����̎��E45�x���Ȃ��Ȃ琳�ʂƔ��肷��
//    DirectX::XMVECTOR EtoP_vec = Math::calc_vector_AtoB_normalize(mPosition, mPlayerPosition);
//    DirectX::XMVECTOR Forward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&forward));
//    DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(EtoP_vec, Forward);
//    float dot;
//    DirectX::XMStoreFloat(&dot, Dot);
//    dot = acosf(dot);
//    if (DirectX::XMConvertToDegrees(dot) < 45.0f)
//    {
//        //���ʂ���̍U���Ȃ�_���[�W��3����1�ɂ���
//        return damage_ /= 3;
//    }
//
//	//�����łȂ��Ȃ�A�󂯂��_���[�W�����̂܂܂ɂ���
//    return damage_;
//
//}
//
ShieldEnemy::ShieldEnemy(GraphicsPipeline& Graphics_,
    const DirectX::XMFLOAT3& EmitterPoint_,
    const EnemyParamPack& ParamPack_)
    :BaseEnemy(Graphics_,
        "./resources/Models/Enemy/SwordEnemy.fbx",
        ParamPack_,
        EmitterPoint_)
{
    ShieldEnemy::fRegisterFunctions();
    // �{�[����������
    mSwordBone = mpModel->get_bone_by_name("hand_r_joint");
    mScale = { 0.05f,0.05f,0.05f };

}

ShieldEnemy::ShieldEnemy(GraphicsPipeline& Graphics_)
    :BaseEnemy(Graphics_, "./resources/Models/Enemy/SwordEnemy.fbx")
{}

void ShieldEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    fBaseUpdate(elapsedTime_, Graphics_);
    fUpdateAttackCapsule(); // �U���p�̃J�v�Z���ʒu���X�V
}

void ShieldEnemy::fRegisterFunctions()
{
    // �֐����X�e�[�g�}�V���ɓo�^
    {
        InitFunc ini = [=]()->void
        {
            fSpawnInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fSpawnUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Start, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fWalkInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fWalkUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Move, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShieldReadyInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShieldReadyUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ShieldReady, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShieldAttackInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShieldAttackUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::ShieldAttack, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fShieldInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fShieldUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Shield, tuple));
    }
    
    {
        InitFunc ini = [=]()->void
        {
            fStunInit();
        };
        UpdateFunc up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
        {
            fStunUpdate(elapsedTime_, Graphics_);
        };
        auto tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(DivedState::Stun, tuple));
    }
    fChangeState(DivedState::Start);
}


void ShieldEnemy::fUpdateAttackCapsule()
{
}
//
//void ShieldEnemy::fDamaged(int Damage_, float InvincibleTime_)
//{
//
//
//    if (mInvincibleTime <= 0.0f)
//    {
//        mCurrentHitPoint -= Damage_;
//        mInvincibleTime = InvincibleTime_;
//    }
//    if (mCurrentHitPoint <= 0)
//    {
//        fDie();
//    }
//}

void ShieldEnemy::fSpawnInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::idle, true);
    // �ėp�^�C�}�[��������
    mWaitTimer = 0.0f;
}

void ShieldEnemy::fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mWaitTimer += elapsedTime_;
    mDissolve -= elapsedTime_;
    // ��莞�Ԍo�߂ňړ��ɑJ��
    if (mWaitTimer >= mSpawnDelaySec)
    {
        fChangeState(DivedState::Move);
    }
}

void ShieldEnemy::fWalkInit()
{
    // �A�j���[�V�������Đ�
    mpModel->play_animation(mAnimPara, AnimationName::walk, true);
    mWaitTimer = 0.0f;
}

void ShieldEnemy::fWalkUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    //--------------------<�v���C���[�̂�������Ɉړ�>--------------------//

    fMoveFront(elapsedTime_, 10.0f);
    //--------------------<�v���C���[�̕����ɉ�]>--------------------//
    fTurnToPlayer(elapsedTime_, 2.0f);

    // �v���C���[�Ƃ̋��������ȉ��ɂȂ�����
    if (mAttackRange >= Math::Length(mPlayerPosition - mPosition))
    {
        fChangeState(DivedState::ShieldReady);
    }
}

void ShieldEnemy::fShieldReadyInit()
{
    mpModel->play_animation(mAnimPara, AnimationName::shield_ready);
    mWaitTimer = 0.0f;
}

void ShieldEnemy::fShieldReadyUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

    // �^�C�}�[�����Z
    mWaitTimer += elapsedTime_;
    //�V�[���h�\�����Ԃ���莞�Ԃ�������
    if (mWaitTimer >= mShieldReadySec * mAnimationSpeed)
    {
        fChangeState(DivedState::Idle);
    }
    //�\�����ɐ��ʂ���U�����ꂽ��

    //�\�����Ɍ�납��U�����ꂽ��
}

void ShieldEnemy::fShieldAttackInit()
{
}

void ShieldEnemy::fShieldAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}

void ShieldEnemy::fShieldInit()
{
}

void ShieldEnemy::fShieldUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}

void ShieldEnemy::fEscapeInit()
{
}

void ShieldEnemy::fEscapeUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}

void ShieldEnemy::fStunInit()
{
}

void ShieldEnemy::fStunUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}

void ShieldEnemy::fDieInit()
{
}

void ShieldEnemy::fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
}
