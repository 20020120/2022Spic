#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>
//****************************************************************
// 
// �e�X�g�ƃT���v�������˂��G�̔h���N���X 
// 
//****************************************************************

typedef std::function<void()> InitFunc;
typedef std::function<void(float)> UpdateFunc;
typedef std::tuple<InitFunc,UpdateFunc > FunctionTuple;

class TestEnemy final:public BaseEnemy
{
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    
    TestEnemy(ID3D11Device* pDevice_, 
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/);

    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;
    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
private:
    int mState{}; // ���
    bool mIsInitialize{ false }; // �������������ǂ���

    //****************************************************************
    // 
    // �ȈՓI�ȃX�e�[�g�}�V�����������i�������x���K�v����Ȃ��Ȃ�ς��邩���j
    // 
    //****************************************************************
    std::map<int, FunctionTuple> mFunctionMap{};
    FunctionTuple mCurrentTuple{};

    void fRegisterFunctions();
    void fIdleInit();   // �ҋ@�̏�����
    void fIdleUpdate(float elapsedTime_); // �ҋ@�̍X�V����

    void fChangeState(int i);
};
