#pragma once

#include "icon_base.h"
#include "practical_entities.h"
#include "number.h"

class GameIcon : public IconBase, PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    GameIcon(ID3D11Device* device);
    ~GameIcon() override;
    //--------< ŠÖ” >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void render(std::string gui, ID3D11DeviceContext* dc, const DirectX::XMFLOAT2& add_pos) override;
    void vs_cursor(const DirectX::XMFLOAT2& cursor_pos) override;
private:
    //--------< ’è” >--------//
    enum ChoicesType
    {
        SHAKE,
        VIBRATION,
        FLUSH,

        SENSITIVITY, // bar‚Íˆê”Ô‰º
    };
    static const int MAX_SCALE_COUNT = 20;
    static constexpr float INTERVAL = 0.1f;
    static const int BUTTON_COUNT = 3;
    //--------< •Ï” >--------//
    FontElement shake;
    FontElement vibration;
    FontElement flush;
    FontElement sensitivity;
    //--button--//
    FontElement choices[BUTTON_COUNT][2]; // 0:ON 1:OFF
    Element selecter[BUTTON_COUNT][2]; // 0:L 1:R
    DirectX::XMFLOAT2 selecter_arrival_pos[BUTTON_COUNT][2]; // 0:L 1:R
    bool setup[BUTTON_COUNT];
    //--bar--//
    float interval_LX = 0;
    std::vector<Element> scales;
    std::vector<Element> shell_scales;
    std::unique_ptr<SpriteBatch> sprite_scale = nullptr;
    std::unique_ptr<Number> sensitivity_number;
};