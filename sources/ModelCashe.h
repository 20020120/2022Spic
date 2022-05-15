#pragma once
#include"skinned_mesh.h"
#include<vector>
#include<memory>
#include "practical_entities.h"

class ModelCashes : public PracticalEntities
{
public:
    static void Load_IntoTitle(ID3D11Device* pDevice_); // タイトル画面の裏でロードする
    static void Load_PreTitle(ID3D11Device* pDevice_);  // タイトル画面の前にロードする
private:
    inline static std::vector<std::shared_ptr<SkinnedMesh>> mpModelCashVec{};
};

inline void ModelCashes::Load_IntoTitle(ID3D11Device* pDevice_)
{
    // ボスのモデルをロード
    auto model1 = resource_manager->load_model_resource(pDevice_, "./resources/Models/Enemy/boss_animation_fifth.fbx");
    mpModelCashVec.emplace_back(std::move(model1));
    auto model2 = resource_manager->load_model_resource(pDevice_, "./resources/Models/Enemy/boss_animation_fifth_dive.fbx");
    mpModelCashVec.emplace_back(std::move(model2));
}

inline void ModelCashes::Load_PreTitle(ID3D11Device* pDevice_)
{
    auto model1 = resource_manager->load_model_resource(pDevice_, ".\\resources\\Models\\Player\\player_twentyfirst.fbx");
    mpModelCashVec.emplace_back(std::move(model1));
    auto model2 = resource_manager->load_model_resource(pDevice_, ".\\resources\\Models\\stage\\title_stage.fbx");
    mpModelCashVec.emplace_back(std::move(model2));
}
