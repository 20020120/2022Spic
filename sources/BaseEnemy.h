#pragma once
#include"skinned_mesh.h"
#include "MoveBehavior.h"
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

class BaseEnemy :public MoveBehavior
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
public:
    struct CapsuleCollider
    {
        DirectX::XMFLOAT3 mPointA{};
        DirectX::XMFLOAT3 mPointB{};
        float mRadius{};
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

    //--------------------<�v���C���[����_���[�W���󂯂�>--------------------//
    virtual void fDamaged(int Damage_);

    // �v���C���[�G�Ƃ̋������v�Z����
    void fCalcNearestEnemy(DirectX::XMFLOAT3 NearPosition_);

    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]] bool fGetIsFrustum()const;
    [[nodiscard]] float fGetLengthFromPlayer()const;
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition()const;
    [[nodiscard]] bool fGetIsAlive()const;
    [[nodiscard]] CapsuleCollider fGetCapsuleData()const;
    //--------------------<�Z�b�^�[�֐�>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_);

protected:
    void fUpdateBase(float elapsedTime_);

    void fUpdateStateMachine(float elapsedTime_);
    void fCalcFrustum();
    void fCalcLength();
    virtual  void fSetCapsulePoint() = 0;
    virtual void fTurnToThePlayer(){}
    //--------------------<�ړ������֘A>--------------------//
    //�v���C���[�̂ق�����������
    bool fTurnToPlayer(float elapsedTime_, float end_turn_angle);
    void fUpdateVelocity(float elapsed_time, DirectX::XMFLOAT3& position, DirectX::XMFLOAT4& orientation);
    //�������͍X�V����
    void fUpdateVerticalVelocity(float elapsedFrame);
    //�����ړ��X�V����
    void fUpdateVerticalMove(float elapsedTime, DirectX::XMFLOAT3& position);
    //�������͍X�V����
    void fUpdateHrizontalVelocity(float elasedFrame);
    //�����ړ��X�V����
    void fUpdateHorizontalMove(float elapsedTime, DirectX::XMFLOAT3& position);

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
    float mLengthFromTargetEnemy{}; // �v���C���[�����b�N�I�����Ă���G�Ƃ̋���
    Param mParam{};
    CapsuleCollider mCapsuleCollider{};
    
    //�G�l�~�[�̊e����
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up = {0.0f, 1.0f, 0.1f};
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
    void fGetEnemyDirections();

};