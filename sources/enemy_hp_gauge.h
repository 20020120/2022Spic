#pragma once

#include "UI.h"
#include "BaseEnemy.h"

class EnemyHpGauge : public UI
{
public:
    //--------<constructor/destructor>--------//
    EnemyHpGauge(GraphicsPipeline& graphics);
    ~EnemyHpGauge() override {}
    //--------< ŠÖ” >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void render(GraphicsPipeline& graphics, float elapsed_time) override;
    //--------<getter/setter>--------//
    void focus(const BaseEnemy* target_enemy, bool lockon);
private:
    //--------< •Ï” >--------//
    std::unique_ptr<SpriteBatch> frame{ nullptr };
    std::unique_ptr<SpriteBatch> back{ nullptr };
    std::unique_ptr<SpriteBatch> body{ nullptr };

    Element gauge;
    DirectX::XMFLOAT3 focus_position{};
    float hp_percent = 1.0f;
    DirectX::XMFLOAT2 offset{};
    bool animation = false;
    float length_player_to_enemy = 0;
};