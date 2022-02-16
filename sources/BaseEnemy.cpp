#include "BaseEnemy.h"
#include"user.h"
//****************************************************************
// 
// �G�̊��N���X 
// 
//****************************************************************
BaseEnemy::BaseEnemy(ID3D11Device* pDevice_, const char* ModelName_)
{
    // ���f����������
    mpSkinnedMesh = std::make_unique<SkinnedMesh>(pDevice_, ModelName_);
}

BaseEnemy::~BaseEnemy() = default;

void BaseEnemy::fRender(ID3D11DeviceContext* pDeviceContext_) const
{
    // ���[���h�s����쐬
    const auto worldMatrix = Math::calc_world_matrix(mScale, mOrientation, mPosition);
    mpSkinnedMesh->render(pDeviceContext_, worldMatrix, { 1.0f,1.0f,1.0f,1.0f });
}

void BaseEnemy::fUpdateStateMachine(float elapsedTime_)
{
    // ���g�������ݒ肳��Ă��Ȃ������ꍇ�̋����͖��m���Ȃ̂ŋC��t����
   
    // �X�e�[�g��������
    if (!mIsInitialize)
    {
        std::get<0>(mCurrentTuple)();
        mIsInitialize = true;
    }
    // �X�e�[�g���X�V
    std::get<1>(mCurrentTuple)(elapsedTime_);
}


void BaseEnemy::fChangeState(int i)
{
    mCurrentTuple = mFunctionMap.at(i);
    mIsInitialize = false;
}