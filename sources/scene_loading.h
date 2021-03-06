#pragma once

#include "Scene.h"
#include "graphics_pipeline.h"
#include "SpriteBatch.h"
#include "practical_entities.h"

class SpriteBatch;

class SceneLoading : public Scene, PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    SceneLoading(Scene* nextscene) : next_scene(nextscene) {}
    ~SceneLoading() override {}
    //--------< 関数 >--------//
    //初期化
    void initialize(GraphicsPipeline& graphics) override;
    //終了化
    void uninitialize() override;
    //更新処理
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    //描画処理
    void render(GraphicsPipeline& graphics, float elapsed_time) override;
private:
    //--------< 関数 >--------//
    static void loading_thread(GraphicsPipeline* graphics, SceneLoading* scene);
private:
    //--------< 変数 >--------//
    Scene* next_scene{};
    std::unique_ptr<SpriteBatch> load_back;
    std::unique_ptr<SpriteBatch> load_text;
    std::unique_ptr<SpriteBatch> load_icon;

    float loadTextWidth{};
    DirectX::XMFLOAT2 loadIconPosition{};
};