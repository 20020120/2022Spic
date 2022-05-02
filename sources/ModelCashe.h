#pragma once
#include"skinned_mesh.h"
#include<vector>
#include<memory>
class ModelCashes
{
public:
    static void Load_IntoTitle(ID3D11Device* pDevice_); // �^�C�g����ʂ̗��Ń��[�h����
    static void Load_PreTitle(ID3D11Device* pDevice_);  // �^�C�g����ʂ̑O�Ƀ��[�h����
private:
    inline static std::vector<std::unique_ptr<SkinnedMesh>> mpModelCashVec{};
};

inline void ModelCashes::Load_IntoTitle(ID3D11Device* pDevice_)
{
  // �{�X�̃��f�������[�h
    auto model = std::make_unique<SkinnedMesh>(pDevice_,
        "./resources/Models/Enemy/boss_animation_second.fbx");
    mpModelCashVec.emplace_back(std::move(model));
}

inline void ModelCashes::Load_PreTitle(ID3D11Device* pDevice_)
{
   
}
