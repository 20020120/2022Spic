#include"NormalEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
#include "user.h"
//****************************************************************
// 
// ���Ȃ��ʏ�U���̎G���G�̔h���N���X
// 
//****************************************************************
NormalEnemy::NormalEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(pDevice_, "./resources/Models/Enemy/character_4.fbx")
{
    // �ʒu��������
    mPosition = EmitterPoint_;
    mPosition = { 10,0, 10 };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.1f,0.1f,0.1f };
    //�p�����[�^�[�̏�����
	fParamInitialize();
    fRegisterFunctions();

}

void NormalEnemy::fInitialize()
{
}

void NormalEnemy::fUpdate(float elapsedTime_)
{
    //--------------------<�X�V����>--------------------//
    fUpdateBase(elapsedTime_);


}

void NormalEnemy::fRegisterFunctions()
{
    InitFunc Ini = [=]()->void
    {
        fIdleInit();
    };
    UpdateFunc Up = [=](float elapsedTime_)->void
    {
        fIdleUpdate(elapsedTime_);
    };
    FunctionTuple tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(IDLE, tuple));

    Ini = [=]()->void
    {
        fMoveInit();
    };
    Up = [=](float elapsedTime_)->void
    {
        fmoveUpdate(elapsedTime_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(MOVE, tuple));

    Ini = [=]()->void
    {
        fAttackInit();
    };
    Up = [=](float elapsedTime_)->void
    {
        fAttackUpdate(elapsedTime_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(MOVE, tuple));

    fChangeState(IDLE);

}
void NormalEnemy::fParamInitialize()
{
    mParam.mHitPoint = 10;      // �̗�
    mParam.mAttackPower = 10;   // �U����
    mParam.mMoveSpeed = 10;   // �ړ����x
    mParam.mAttackSpeed = 2; // �U���Ԋu
    
}


void NormalEnemy::fIdleInit()
{
}


void NormalEnemy::fIdleUpdate(float elapsedTime_)
{
	if(fTurnToPlayer(elapsedTime_, 10))
    {
        fChangeState(MOVE);
    }
}

void NormalEnemy::fMoveInit()
{

}

void NormalEnemy::fmoveUpdate(float elapsedTime_)
{
    velocity.x += forward.x * move_speed * elapsedTime_;
    velocity.y += forward.y * move_speed * elapsedTime_;
    velocity.z += forward.z * move_speed * elapsedTime_;
    if (!fTurnToPlayer(elapsedTime_,5))
    {
        fChangeState(IDLE);
    }
}

void NormalEnemy::fAttackInit()
{
}

void NormalEnemy::fAttackUpdate(float elapsedTime_)
{
}
