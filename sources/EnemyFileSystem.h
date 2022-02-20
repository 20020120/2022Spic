#pragma once
#include<cereal/cereal.hpp>
#include<vector>
#include"BaseEnemy.h"
//****************************************************************
// 
// 敵のファイル管理システム 
// 
//****************************************************************

struct EnemySource
{
    float mSpawnTimer{};  // 出現タイミングを記録
    int mEmitterNumber{}; // 出現位置の番号
    int mType{};          // 敵の種類

    // シリアライズ
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
