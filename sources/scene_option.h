#pragma once

#include <DirectXMath.h>
#include "graphics_pipeline.h"
#include "sprite_batch.h"
#include "practical_entities.h"
#include "transition_icon.h"

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
    static bool get_validity() { return validity; }
    static void set_validity(bool v) { validity = v; }
private:
    //--------< íËêî >--------//
    enum class IconType
    {
        VOLUME,
        GAME,
        TRANSITION,
    };
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
    Element tab;
    std::unique_ptr<SpriteBatch> sprite_tab{ nullptr };
    Element frame;
    std::unique_ptr<SpriteBatch> sprite_frame{ nullptr };

    std::map<IconType, std::unique_ptr<IconBase>> icon_map;

    IconType state = IconType::TRANSITION;
    static bool validity;
};