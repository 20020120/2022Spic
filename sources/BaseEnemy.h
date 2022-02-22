#pragma once
#include"skinned_mesh.h"
#include "EnemyMove.h"

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

struct CapsuleSaveData
{
    float mLengthFromPositionA{};
    float mLengthFromPositionB{};
    float mRadius{};
    // �V���A���C�Y
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("LengthA", mLengthFromPositionA),
            cereal::make_nvp("LengthB", mLengthFromPositionB),
            cereal::make_nvp("Radius", mRadius));
    }

};
struct EnemyData
{
    std::string mDivideClass; // �h���N���X�̖��O���f�[�^�̃L�[�l�i�ꉞ�j
    int mMaxHitPoint{};      // �̗�
    int mAttackPower{};   // �U����
    float mMoveSpeed{};   // �ړ����x
    float mAttackSpeed{}; // �U���Ԋu
    CapsuleSaveData mCapsule{}; //�J�v�Z���̃f�[�^
    template<class Archive>
        void serialize(Archive& archive)
    {
            archive(
                cereal::make_nvp("Name", mDivideClass),
                cereal::make_nvp("MaxHp", mMaxHitPoint),
                cereal::make_nvp("AttackPower", mAttackPower),
                cereal::make_nvp("MoveSpeed", mMoveSpeed),
                cereal::make_nvp("AttackSpeed", mAttackSpeed),
                cereal::make_nvp("Capsule", mCapsule));
    }
};


class BaseEnemy :public EnemyMove
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
        // �V���A���C�Y
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("PointA", mPointA),
                cereal::make_nvp("PointB", mPointB),
                cereal::make_nvp("Radius", mRadius));
        }

    };

    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    BaseEnemy(ID3D11Device *pDevice_,int UniqueId_,const char* ModelName_);
    virtual ~BaseEnemy();

    virtual void fInitialize() = 0;
    virtual void fUpdate(float elapsedTime_) = 0;
    void fRender(ID3D11DeviceContext* pDeviceContext_) const;

    void fGetParam(BaseEnemy* This_,std::function<EnemyData(std::string)> Function_);
    //--------------------<ImGui>--------------------//
    virtual void fGuiMenu(){}
    
    //--------------------<�v���C���[����_���[�W���󂯂�>--------------------//
    virtual void fDamaged(int Damage_, float InvinsibleTime_);

    // �v���C���[�G�Ƃ̋������v�Z����
    void fCalcNearestEnemy(DirectX::XMFLOAT3 NearPosition_);

    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]] bool fGetIsFrustum()const;
    [[nodiscard]] float fGetLengthFromPlayer()const;
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition()const;
    [[nodiscard]] bool fGetIsAlive()const;
    [[nodiscard]] CapsuleCollider fGetCapsuleData()const;
    [[nodiscard]] int fGetUniqueId()const;
    [[nodiscard]] float fGetLengthFromNearEstEnemy()const;
    [[nodiscard]] const float fGetPercentHitPoint()const;
    [[nodiscard]] std::string fGetType()const;
    //--------------------<�Z�b�^�[�֐�>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_);
    void fSetPosition(DirectX::XMFLOAT3 Position);
protected:
    void fUpdateBase(float elapsedTime_);

    void fUpdateStateMachine(float elapsedTime_);
    void fCalcFrustum();
    void fCalcLength();
    virtual  void fSetCapsulePoint();
    void fUpdateInvicibleTimer(float elapsedTime_);
    //--------------------<�ړ������֘A>--------------------//
    //�v���C���[�̂ق�����������
    bool fTurnToPlayer(float elapsedTime_);
    //�����擾
    void fGetDirections(DirectX::XMFLOAT4& o);

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
    EnemyData mData{}; 
    Param mParam{};
    CapsuleCollider mCapsuleCollider{};
    bool IsAttacked{};
    float mInvinsibleTimer = 0.0f;//���G����
    //�G�l�~�[�̊e����
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.1f };

    int mUniqueId{};
    // ���f��
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };
private:

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