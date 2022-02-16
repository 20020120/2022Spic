#pragma once
#include "json.hpp"

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
};

namespace EnemyJson
{
    using namespace  nlohmann;

    void to_json(json& j, const EnemySource& p) {
        j = json{
              {"Time", p.mSpawnTimer},
              {"Number", p.mEmitterNumber},
              {"Type", p.mType}
        };
    }

    void from_json(const json& j, EnemySource& p) {
        j.at("Time").get_to(p.mSpawnTimer);       // get_to(T& arg) は arg = get<T>() と同じ
        j.at("Number").get_to(p.mEmitterNumber);
        j.at("Type").get_to(p.mType);
    }
    
}
