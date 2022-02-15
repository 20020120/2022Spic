#pragma once

#include <memory>

#include "Scene.h"
#include "SpriteBatch.h"
#include "practical_entities.h"

class SpriteBatch;

class SceneTitle : public Scene , public PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    SceneTitle() {}
    ~SceneTitle() override {}
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
    //--------< 変数 >--------//
    std::unique_ptr<SpriteBatch> sprite_batch;
};