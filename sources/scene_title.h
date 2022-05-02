#pragma once

#include <memory>

#include "scene.h"
#include "sprite_batch.h"
#include "practical_entities.h"

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
    struct Element
    {
        DirectX::XMFLOAT2 position{};
        DirectX::XMFLOAT2 scale{ 1, 1 };
        DirectX::XMFLOAT2 pivot{};
        DirectX::XMFLOAT4 color{ 1,1,1,1 };
        float angle{};
        DirectX::XMFLOAT2 texpos{};
        DirectX::XMFLOAT2 texsize{};
    };
    Element back;
    std::unique_ptr<SpriteBatch> sprite_back{ nullptr };
    Element start;
    Element exit;
    std::unique_ptr<SpriteBatch> sprite_string{ nullptr };
    Element selecter1;
    DirectX::XMFLOAT2 arrival_pos1{};
    Element selecter2;
    DirectX::XMFLOAT2 arrival_pos2{};
    std::unique_ptr<SpriteBatch> sprite_selecter{ nullptr };
    int state = 0;
    //--------< �}���`�X���b�h >--------//
    static void loading_thread(ID3D11Device* device);
    static bool is_ready;
};