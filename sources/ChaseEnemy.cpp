#include"ChaseEnemy.h"
#include"Operators.h"
//****************************************************************
// 
// �ړ������̃U�R�G 
// 
//****************************************************************

ChaseEnemy::ChaseEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 Point_, int UniqueId_)
    :BaseEnemy(pDevice_, UniqueId_, "./resources/Models/Enemy/character_5.fbx")
{
    //������
    mPosition = Point_;
    mScale = { 0.03f,0.03f,0.03f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };

    // �p�����[�^�[��������
    mParam.mAttackPower = 0;
    mParam.mAttackSpeed = 0.0f;
    mParam.mHitPoint = 10;
    mParam.mMoveSpeed = static_cast<float>((rand() % 20) + 10.0f);

    // ������������
    mDistance = static_cast<float>((rand() % 20) + 10.0f);
    fRegisterFunctions();
    
}

void ChaseEnemy::fInitialize()
{
   
}

 void ChaseEnemy::fUpdate(float elapsedTime_)
{
     fUpdateBase(elapsedTime_);
     mTimerComponent.fUpdate(elapsedTime_);
}

 void ChaseEnemy::fSetCapsulePoint()
{
    
}

 void ChaseEnemy::fMove(float elapsedTime_)
{
   
}

void ChaseEnemy::fRotate(float elapsedTime_)
{
    // ��]�����Z�o
    const DirectX::XMFLOAT3 V1 = Math::Normalize(mPlayerPosition - mPosition);
    const DirectX::XMFLOAT3 V2 = Math::Normalize(forward);
    auto axis = Math::Cross(V1, V2);

    // ��]�p���Z�o
    auto rad = Math::Dot(V1, V2);

}

void ChaseEnemy::fRegisterFunctions()
{
    {
        InitFunc ini = [=]()->void
        {
            fStartInit();
        };
        UpdateFunc up = [=](float elapsedTIme_)->void
        {
            fStartUpdate(elapsedTIme_);
        };

        FunctionTuple tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(State::Start, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fChaseInit();
        };
        UpdateFunc up = [=](float elapsedTIme_)->void
        {
            fChaseUpdate(elapsedTIme_);
        };

        FunctionTuple tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(State::Chase, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fIntimidationInit();
        };
        UpdateFunc up = [=](float elapsedTIme_)->void
        {
            fIntimidationUpdate(elapsedTIme_);
        };

        FunctionTuple tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(State::Intimidation, tuple));
    }

    mCurrentTuple = mFunctionMap.at(Start);
}

void ChaseEnemy::fStartInit()
{
    // �ҋ@���Ԃ�ݒ�
    mTimerComponent.StartTimer(5.0f);
}

void ChaseEnemy::fStartUpdate(float elapsedTime_)
{
    if (mTimerComponent.fGetOver())
    {
        fChangeState(State::Chase);
    }
}

void ChaseEnemy::fChaseInit()
{
    
}

void ChaseEnemy::fChaseUpdate(float elapsedTime_)
{

    if (mLengthFromPlayer > mDistance)
    {

        // �ڕW�n�_�܂ł̃x�N�g�����擾
        auto moveDirection = mPlayerPosition - mPosition;
        moveDirection = Math::Normalize(moveDirection);

        // �ʒu���X�V
        mPosition += moveDirection * mParam.mMoveSpeed * elapsedTime_;
    }
    else
    {
        // ��苗�����ɂȂ�����J�ڂ���
        fChangeState(State::Intimidation);
    }
}

void ChaseEnemy::fIntimidationInit()
{
    mTimerComponent.StartTimer(5.0f);
}

void ChaseEnemy::fIntimidationUpdate(float elapsedTime_)
{
    if(mTimerComponent.fGetOver())
    {
        fChangeState(State::Chase);
    }
}
