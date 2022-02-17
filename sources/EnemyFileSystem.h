#pragma once
#include<cereal/cereal.hpp>


#include<vector>

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

struct EnemyCommonParam
{
    int mHitPoint{};      // 体力
    int mAttackPower{};   // 攻撃力
    float mMoveSpeed{};   // 移動速度
    float mAttackSpeed{}; // 攻撃速度

     // シリアライズ
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
