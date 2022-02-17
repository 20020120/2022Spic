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

struct CubeData
{
    DirectX::XMFLOAT3 mHalfSize{};
};

class BaseEnemy
{
    //****************************************************************
    // 
    // �\����
    // 
    //****************************************************************
    struct Param
    {
        int mHitPoint{};      // �̗�
        int mAttackPower{};   // �U����
        float mMoveSpeed{};   // �ړ����x
        float mAttackSpeed{}; // �U���Ԋu
    };

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


    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]] bool fGetIsFrustum()const;
    [[nodiscard]] float fGetLengthFromPlayer()const;
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition()const;
    
    //--------------------<�Z�b�^�[�֐�>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_);

protected:
    void fUpdateBase(float elapsedTime_);

    void fUpdateStateMachine(float elapsedTime_);
    void fCalcFrustum();
    void fCalcLength();
    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
protected:
    DirectX::XMFLOAT3 mPosition{};    // �ʒu
    DirectX::XMFLOAT4 mOrientation{}; // ��]
    DirectX::XMFLOAT3 mScale{};       // �傫��

    DirectX::XMFLOAT3 mPlayerPosition{}; // �v���C���[�̈ʒu
    float mLengthFromPlayer{};     // �v���C���[����̋���
private:
    // ���f��
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };

    //--------------------<�X�e�[�g�}�V���Ɋ֘A����ϐ�>--------------------//
    int mState{}; // ���
    bool mIsInitialize{ false }; // �������������ǂ���

    bool mIsFrustum{}; // �J�����O����Ă��邩
    CubeData mCubeData{};
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