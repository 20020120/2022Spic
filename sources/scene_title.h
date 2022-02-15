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
    //--------< ŠÖ” >--------//
    //‰Šú‰»
    void initialize(GraphicsPipeline& graphics) override;
    //I—¹‰»
    void uninitialize() override;
    //XVˆ—
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    //•`‰æˆ—
    void render(GraphicsPipeline& graphics, float elapsed_time) override;
private:
    //--------< •Ï” >--------//
    std::unique_ptr<SpriteBatch> sprite_batch;
};