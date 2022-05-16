#pragma once

#include "UI.h"

class PlayerConfig : public UI
{
public:
    //--------<constructor/destructor>--------//
    PlayerConfig(GraphicsPipeline& graphics);
    ~PlayerConfig() override {}
    //--------< �֐� >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void render(ID3D11DeviceContext* dc) override;
    //--------<getter/setter>--------//
    void set_hp_percent(float per) { hp_percent = per; }
    void set_mp_percent(float per) { mp_percent = per; }
private:
    //--------< �ϐ� >--------//
    std::unique_ptr<SpriteBatch> hp_frame{ nullptr };
    std::unique_ptr<SpriteBatch> hp_back{ nullptr };
    std::unique_ptr<SpriteBatch> hp_body_sprite{ nullptr };
    std::unique_ptr<SpriteBatch> mp_frame{ nullptr };
    std::unique_ptr<SpriteBatch> mp_back{ nullptr };
    std::unique_ptr<SpriteBatch> mp_body_sprite{ nullptr };
    std::unique_ptr<SpriteBatch> base_sprite{ nullptr };
    std::unique_ptr<SpriteBatch> base2_sprite{ nullptr };

    Element base;
    Element hp_gauge;
    Element hp_body;
    Element mp_gauge;
    Element mp_body;
    float hp_percent = 1.0f;
    float mp_percent = 1.0f;

    float glow_vertical = {};
};