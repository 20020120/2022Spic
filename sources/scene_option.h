#pragma once

#include <DirectXMath.h>
#include "graphics_pipeline.h"
#include "sprite_batch.h"
#include "practical_entities.h"

class Option : public PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    Option(GraphicsPipeline& graphics);
    ~Option() override {}
    //--------< ä÷êî >--------//
    //çXêVèàóù
    void update(GraphicsPipeline& graphics, float elapsed_time);
    //ï`âÊèàóù
    void render(GraphicsPipeline& graphics, float elapsed_time);
    //--------<getter/setter>--------//
    bool get_validity() const { return validity; }
    void set_validity(bool v) { validity = v; }
private:
    //--------< ïœêî >--------//
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
    Element game;
    Element title;
    std::unique_ptr<SpriteBatch> sprite_string{ nullptr };
    Element selecter1;
    DirectX::XMFLOAT2 arrival_pos1{};
    Element selecter2;
    DirectX::XMFLOAT2 arrival_pos2{};
    std::unique_ptr<SpriteBatch> sprite_selecter{ nullptr };

    int state = 0;
    bool validity = false;
};