#pragma once
#include "json.hpp"

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
        j.at("Time").get_to(p.mSpawnTimer);       // get_to(T& arg) �� arg = get<T>() �Ɠ���
        j.at("Number").get_to(p.mEmitterNumber);
        j.at("Type").get_to(p.mType);
    }
    
}
