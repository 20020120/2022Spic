#pragma once
#include"skinned_mesh.h"
#include<d3d11.h>
#include<memory>

//****************************************************************
// 
// �G�̊��N���X 
// 
//****************************************************************

typedef std::function<void()> InitFunc;
typedef std::function<void(float)> UpdateFunc;
typedef std::tuple<InitFunc, UpdateFunc > FunctionTuple;


class BaseEnemy
{
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    BaseEnemy(ID3D11Device *pDevice_,const char* ModelName_);
    virtual ~BaseEnemy();

    virtual void fInitialize() = 0;
    virtual void fUpdate(float elapsedTime_) = 0;
    void fRender(ID3D11DeviceContext* pDeviceContext_) const;

protected:
    void fUpdateStateMachine(float elapsedTime_);

    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
protected:
    DirectX::XMFLOAT3 mPosition{};    // �ʒu
    DirectX::XMFLOAT4 mOrientation{}; // ��]
    DirectX::XMFLOAT3 mScale{};       // �傫��
private:
    // ���f��
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };

    //--------------------<�X�e�[�g�}�V���Ɋ֘A����ϐ�>--------------------//

    int mState{}; // ���
    bool mIsInitialize{ false }; // �������������ǂ���

protected:
    //****************************************************************
    // 
    // �ȈՓI�ȃX�e�[�g�}�V�����������i�������x���K�v����Ȃ��Ȃ�ς��邩���j
    // 
    //****************************************************************
    std::map<int, FunctionTuple> mFunctionMap{};
    FunctionTuple mCurrentTuple{};

    virtual void fRegisterFunctions() {}; 
    void fChangeState(int i);
};