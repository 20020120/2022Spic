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
    float InvincibleTime{}; // �U����������̖��G����
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



// �G�̎��
enum class EnemyType
{
    // �������������������������牺�ɑ��₷��������������������������
    Archer, //�������U��
    Shield, //������
    Sword,  // ���̓G 
    Spear,
    Archer_Ace, //�������U��
    Shield_Ace, //������
    Sword_Ace,  // ���̓G 
    Spear_Ace,
    Boss,
    // ���������������������������ɑ��₷��������������������������
    Count, // ��ނ̑���
};


struct EnemySource
{
    float mSpawnTimer{};  // �o���^�C�~���O���L�^
    DirectX::XMFLOAT3 mEmitterPoint{}; // �o���ʒu�̔ԍ�
    EnemyType mType{};          // �G�̎��

    // �V���A���C�Y
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Timer", mSpawnTimer),
            cereal::make_nvp("Emitter", mEmitterPoint),
            cereal::make_nvp("Type", mType));
    }
};
