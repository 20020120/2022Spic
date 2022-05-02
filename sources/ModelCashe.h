#pragma once
#include"skinned_mesh.h"
#include<vector>
#include<memory>
class ModelCashes
{
public:
    static void Load_IntoTitle(ID3D11Device* pDevice_); // タイトル画面の裏でロードする
    static void Load_PreTitle(ID3D11Device* pDevice_);  // タイトル画面の前にロードする
private:
    inline static std::vector<std::unique_ptr<SkinnedMesh>> mpModelCashVec{};
};

inline void ModelCashes::Load_IntoTitle(ID3D11Device* pDevice_)
{
  // ボスのモデルをロード
    auto model = std::make_unique<SkinnedMesh>(pDevice_,
        "./resources/Models/Enemy/boss_animation_second.fbx");
    mpModelCashVec.emplace_back(std::move(model));
}

inline void ModelCashes::Load_PreTitle(ID3D11Device* pDevice_)
{
   
}
