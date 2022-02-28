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
    void render(GraphicsPipeline& graphics, float elapsed_time) override;
    //--------<getter/setter>--------//
    void set_hp_percent(float per) { hp_percent = per; }
    void set_mp_percent(float per) { mp_percent = per; }
private:
    //--------< �ϐ� >--------//
    std::unique_ptr<SpriteBatch> hp_frame{ nullptr };
    std::unique_ptr<SpriteBatch> hp_back{ nullptr };
    std::unique_ptr<SpriteBatch> hp_body{ nullptr };
    std::unique_ptr<SpriteBatch> mp_frame{ nullptr };
    std::unique_ptr<SpriteBatch> mp_back{ nullptr };
    std::unique_ptr<SpriteBatch> mp_body{ nullptr };
    std::unique_ptr<SpriteBatch> base_sprite{ nullptr };

    Element base;
    Element hp_gauge;
    Element mp_gauge;
    float hp_percent = 1.0f;
    float mp_percent = 1.0f;
};