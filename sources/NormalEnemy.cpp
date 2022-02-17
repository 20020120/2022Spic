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
    :BaseEnemy(pDevice_, "./resources/Models/stage/asa.fbx")
{
    // �ʒu��������
    mPosition = EmitterPoint_;
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
    fUpdateStateMachine(elapsedTime_);

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

    fChangeState(IDLE);

}
void NormalEnemy::fParamInitialize()
{
    mParam.mHitPoint = 10;      // �̗�
    mParam.mAttackPower = 10;   // �U����
    mParam.mMoveSpeed = 10;   // �ړ����x
    mParam.mAttackSpeed = 2; // �U���Ԋu
    
}

bool NormalEnemy::fIsTurnToThePlayer()
{
    using namespace DirectX;
    XMVECTOR orientation_vec = XMLoadFloat4(&mOrientation);
    XMVECTOR P_Pos = XMLoadFloat3(&mPlayerPosition);
    XMVECTOR E_Pos = XMLoadFloat3(&mPosition);
    XMVECTOR Vec = P_Pos - E_Pos;
    XMVECTOR Forward = XMLoadFloat3(&forward);
    XMVECTOR Up = XMLoadFloat3(&up);
    XMVECTOR Dot = XMVector3Dot(Vec, Forward);
    float angle = XMVectorGetX(Dot);
    angle = acos(angle);
    if (fabs(angle) > 1e-8f)
    {
        DirectX::XMFLOAT3 f{};
        DirectX::XMStoreFloat3(&f, Forward);
        float cross{ f.x  - f.z  };

        if (cross < 0.0f)
        {
            XMVECTOR q;
            q = XMQuaternionRotationAxis(Up, angle);
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 0.05f);
        }
        else
        {
            XMVECTOR q;
            q = XMQuaternionRotationAxis(Up, -angle);
            XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
            orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 0.05f);
        }
		XMStoreFloat4(&mOrientation, orientation_vec);
        return false;
    }
    else
    {
        return true;
    }
}

void NormalEnemy::fIdleInit()
{
}


void NormalEnemy::fIdleUpdate(float elapsedTime_)
{
   /* if(fIsTurnToThePlayer())
    {
        fChangeState(MOVE);
    }*/
}

void NormalEnemy::fMoveInit()
{

}

void NormalEnemy::fmoveUpdate(float elapsedTime_)
{
    mPosition.x += 2.0f * elapsedTime_;
}
