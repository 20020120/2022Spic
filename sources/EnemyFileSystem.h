#pragma once
#include<cereal/cereal.hpp>
#include<vector>
#include"BaseEnemy.h"
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

namespace EnemyFileSystem
{
    void fLoadFromJson(std::vector<EnemySource>& SourceVec_, const char* FileName_);
    void fSaveToJson(std::vector<EnemySource>& SourceVec_, const char* FileName_);
    void fLoadFromJson(std::map<std::string,EnemyData>& SourceMap_, const char* FileName_);
    void fSaveToJson(std::map<std::string, EnemyData>& SourceMap_, const char* FileName_);

}
