#include "BaseEnemy.h"
#include"user.h"
#include"collision.h"
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

bool BaseEnemy::fGetIsFrustum() const
{
    return mIsFrustum;
}

float BaseEnemy::fGetLengthFromPlayer() const
{
    return mLengthFromPlayer;
}

DirectX::XMFLOAT3 BaseEnemy::fGetPosition() const
{
    return mPosition;
}

bool BaseEnemy::fGetIsAlive() const
{
    return true;
}

void BaseEnemy::fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_)
{
    mPlayerPosition = PlayerPosition_;
}

void BaseEnemy::fUpdateBase(float elapsedTime_)
{
    //--------------------<������J�����O>--------------------//
    fCalcFrustum();

    //--------------------<�X�e�[�g�}�V��>--------------------//
    fUpdateStateMachine(elapsedTime_);

    //--------------------<�v���C���[�Ƃ̋������v�Z>--------------------//
    fCalcLength();
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

void BaseEnemy::fCalcFrustum()
{
    const DirectX::XMFLOAT3 minPoint{
        mPosition.x - mCubeData.mHalfSize.x,
        mPosition.y - mCubeData.mHalfSize.y,
        mPosition.z - mCubeData.mHalfSize.z
    };
    const DirectX::XMFLOAT3 maxPoint{
        mPosition.x + mCubeData.mHalfSize.x,
        mPosition.y + mCubeData.mHalfSize.y,
        mPosition.z + mCubeData.mHalfSize.z
    };

    mIsFrustum=Collision::frustum_vs_cuboid(minPoint, maxPoint);
}

void BaseEnemy::fCalcLength()
{
    mLengthFromPlayer=Math::calc_vector_AtoB_length(mPlayerPosition, mPosition);
}


void BaseEnemy::fChangeState(int i)
{
    mCurrentTuple = mFunctionMap.at(i);
    mIsInitialize = false;
}
