#pragma once
#include<cereal/cereal.hpp>


#include<vector>

//****************************************************************
// 
// �G�̃t�@�C���Ǘ��V�X�e�� 
// 
//****************************************************************

struct EnemySource
{
    float mSpawnTimer{};  // �o���^�C�~���O���L�^
    int mEmitterNumber{}; // �o���ʒu�̔ԍ�
    int mType{};          // �G�̎��

    // �V���A���C�Y
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Timer", mSpawnTimer),
            cereal::make_nvp("Emitter", mEmitterNumber),
            cereal::make_nvp("Type", mType));
    }
};

struct EnemyCommonParam
{
    int mHitPoint{};      // �̗�
    int mAttackPower{};   // �U����
    float mMoveSpeed{};   // �ړ����x
    float mAttackSpeed{}; // �U�����x

     // �V���A���C�Y
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("HP", mHitPoint),
            cereal::make_nvp("Power", mAttackPower),
            cereal::make_nvp("MoveSpeed", mMoveSpeed),
            cereal::make_nvp("AttackSpeed", mAttackSpeed)
        );
    }
};

namespace EnemyFileSystem
{
    void fLoadFromJson(std::vector<EnemySource>& SourceVec_, const char* FileName_);
    void fSaveToJson(std::vector<EnemySource>& SourceVec_, const char* FileName_);

    namespace Private
    {

    }

}

namespace EnemyFileParam
{
    EnemyCommonParam fLoad(const char* FileName_);
    void fCreate(const char* FileName_);

}
