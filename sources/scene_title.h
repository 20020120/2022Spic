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
    //--------< �֐� >--------//
    //������
    void initialize(GraphicsPipeline& graphics) override;
    //�I����
    void uninitialize() override;
    //�X�V����
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    //�`�揈��
    void render(GraphicsPipeline& graphics, float elapsed_time) override;
private:
    //--------< �ϐ� >--------//
    std::unique_ptr<SpriteBatch> sprite_batch;
};