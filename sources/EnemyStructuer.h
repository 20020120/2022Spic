#pragma once
#include<DirectXMath.h>
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
    float BodyCapsuleRad{};
    float AttackCapsuleRad{};

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("MaxHp", MaxHp),
            cereal::make_nvp("AttackPower", AttackPower),
            cereal::make_nvp("BodyCapsuleRad", BodyCapsuleRad));
            cereal::make_nvp("AttackCapsuleRad", AttackCapsuleRad);
    }
};