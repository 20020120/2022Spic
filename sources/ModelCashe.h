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
    auto model1 = resource_manager->load_model_resource(pDevice_, "./resources/Models/Enemy/boss_animation_second.fbx");
    mpModelCashVec.emplace_back(std::move(model1));
    //auto model2 = resource_manager->load_model_resource(pDevice_, "./resources/Models/Enemy/ArcherEnemy.fbx");
    //mpModelCashVec.emplace_back(std::move(model2));
    //auto model3 = resource_manager->load_model_resource(pDevice_, "./resources/Models/Enemy/SpearEnemy.fbx");
    //mpModelCashVec.emplace_back(std::move(model3));
    //auto model4 = resource_manager->load_model_resource(pDevice_, "./resources/Models/Enemy/SpearEnemyAce.fbx");
    //mpModelCashVec.emplace_back(std::move(model4));
    //auto model5 = resource_manager->load_model_resource(pDevice_, "./resources/Models/Enemy/SwordEnemy.fbx");
    //mpModelCashVec.emplace_back(std::move(model5));
    //auto model6 = resource_manager->load_model_resource(pDevice_, "./resources/Models/Enemy/SwordEnemyAce.fbx");
    //mpModelCashVec.emplace_back(std::move(model6));
}

inline void ModelCashes::Load_PreTitle(ID3D11Device* pDevice_)
{
    auto model1 = resource_manager->load_model_resource(pDevice_, ".\\resources\\Models\\Player\\player_nineteenth.fbx");
    mpModelCashVec.emplace_back(std::move(model1));
    auto model2 = resource_manager->load_model_resource(pDevice_, ".\\resources\\Models\\stage\\title_stage.fbx");
    mpModelCashVec.emplace_back(std::move(model2));

}
