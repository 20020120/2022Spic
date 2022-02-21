#pragma once

#include "UI.h"
#include "BaseEnemy.h"

class Reticle : public UI
{
public:
    //--------<constructor/destructor>--------//
    Reticle(GraphicsPipeline& graphics);
    ~Reticle() override {}
    //--------< ŠÖ” >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void render(GraphicsPipeline& graphics, float elapsed_time) override;
    //--------<getter/setter>--------//
    void focus(const BaseEnemy* target_enemy, bool lockon);
private:
    //--------< •Ï” >--------//
    std::unique_ptr<SpriteBatch> reticle{ nullptr };

    Element element;
    DirectX::XMFLOAT3 focus_position{};
    DirectX::XMFLOAT2 offset{};
    bool animation = false;
    float length_player_to_enemy = 0;
};