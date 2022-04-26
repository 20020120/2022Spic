#pragma once
#include<DirectXMath.h>
#include <cereal/cereal.hpp>

struct Capsule
{
    DirectX::XMFLOAT3 mTop{};
    DirectX::XMFLOAT3 mBottom{};
    float mRadius{};
};


struct EnemyParamPack
{
    int MaxHp{};
    int AttackPower{};
    float InvincibleTime{}; // 攻撃した相手の無敵時間
    float BodyCapsuleRad{};
    float AttackCapsuleRad{};

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("MaxHp", MaxHp),
            cereal::make_nvp("AttackPower", AttackPower),
            cereal::make_nvp("InvincibleTime", InvincibleTime),
            cereal::make_nvp("BodyCapsuleRad", BodyCapsuleRad));
            cereal::make_nvp("AttackCapsuleRad", AttackCapsuleRad);
    }
};



// 敵の種類
enum class EnemyType
{
    // ↓↓↓↓↓↓↓↓↓ここから下に増やす↓↓↓↓↓↓↓↓↓↓↓↓↓
    Archer, //遠距離攻撃
    Shield, //盾持ち
    Sword,  // 刀の敵 
    Spear,
    Archer_Ace, //遠距離攻撃
    Shield_Ace, //盾持ち
    Sword_Ace,  // 刀の敵 
    Spear_Ace,
    Boss,
    // ↑↑↑↑↑↑↑↑↑ここから上に増やす↑↑↑↑↑↑↑↑↑↑↑↑↑
    Count, // 種類の総量
};


struct EnemySource
{
    float mSpawnTimer{};  // 出現タイミングを記録
    DirectX::XMFLOAT3 mEmitterPoint{}; // 出現位置の番号
    EnemyType mType{};          // 敵の種類

    // シリアライズ
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Timer", mSpawnTimer),
            cereal::make_nvp("Emitter", mEmitterPoint),
            cereal::make_nvp("Type", mType));
    }
};
