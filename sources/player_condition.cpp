#include "player_condition.h"

PlayerCondition::PlayerCondition(GraphicsPipeline& graphics)
{
    {
        normal_condition = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\scene\\damage_blue.png", 1);
        damage_condition = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\scene\\damage_red.png", 1);

        condition.position = { 0.0f, 0.0f };
        condition.scale    = { 1.0f, 1.0f };
        condition.texsize = { static_cast<float>(normal_condition->get_texture2d_desc().Width),
                              static_cast<float>(normal_condition->get_texture2d_desc().Height) };
    }
    is_damage = false;
    reset_timer = 0;
}

void PlayerCondition::update(GraphicsPipeline& graphics, float elapsed_time)
{
    if (is_damage)
    {
        reset_timer += elapsed_time;
        const float RESET_TIME = 0.5f;
        if (reset_timer > RESET_TIME) { is_damage = false; }
    }
    else
    {
        reset_timer = 0;
    }
}

void PlayerCondition::render(ID3D11DeviceContext* dc)
{
    if (is_damage)
    {
        damage_condition->begin(dc);
        damage_condition->render(dc, condition.position, condition.scale, condition.pivot,
            condition.color, condition.angle, condition.texpos, condition.texsize);
        damage_condition->end(dc);
    }
    else
    {
        normal_condition->begin(dc);
        normal_condition->render(dc, condition.position, condition.scale, condition.pivot,
            condition.color, condition.angle, condition.texpos, condition.texsize);
        normal_condition->end(dc);
    }
}