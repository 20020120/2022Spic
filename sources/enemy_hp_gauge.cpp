#include "enemy_hp_gauge.h"
#include "Operators.h"
#include "codinate_convert.h"

EnemyHpGauge::EnemyHpGauge(GraphicsPipeline& graphics)
{
    frame = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\enemy_hp_flame.png", 1);
    back  = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\enemy_hp_back.png", 1);
    body  = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\enemy_hp_gauge.png", 1);

    gauge.scale = { 0.2f, 0.2f };
    gauge.texsize = { static_cast<float>(frame->get_texture2d_desc().Width), static_cast<float>(frame->get_texture2d_desc().Height) };
    gauge.pivot = gauge.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
    gauge.color.w = 0.0f;
}

void EnemyHpGauge::update(GraphicsPipeline& graphics, float elapsed_time)
{
    static bool is_display_imgui = false;
    imgui_menu_bar("UI", "enemygauge", is_display_imgui);
#ifdef USE_IMGUI
    if (is_display_imgui)
    {
        ImGui::Begin("enemy gauge");
        ImGui::DragFloat2("pos", &gauge.position.x);
        ImGui::DragFloat2("scale", &gauge.scale.x, 0.01f);
        ImGui::DragFloat2("offset", &offset.x, 0.1f);
        ImGui::DragFloat("hp_percent", &hp_percent, 0.1f, 0.0f, 1.0f);
        ImGui::End();
    }
#endif // USE_IMGUI

    if (animation)
    {
        gauge.position = conversion_2D(graphics.get_dc().Get(), focus_position);
        gauge.color.w = Math::lerp(gauge.color.w, 1.0f, 10.0f * elapsed_time);
    }
    else
    {
        gauge.color.w = Math::lerp(gauge.color.w, 0.0f, 10.0f * elapsed_time);
    }
}

void EnemyHpGauge::render(GraphicsPipeline& graphics, float elapsed_time)
{
    //--back--//
    back->begin(graphics.get_dc().Get());
    back->render(graphics.get_dc().Get(), gauge.position + offset, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos, gauge.texsize);
    back->end(graphics.get_dc().Get());
    //--body--//
    body->begin(graphics.get_dc().Get());
    body->render(graphics.get_dc().Get(), gauge.position + offset, gauge.scale, gauge.pivot, gauge.color,
        gauge.angle, gauge.texpos, { gauge.texsize.x * hp_percent, gauge.texsize.y });
    body->end(graphics.get_dc().Get());
    //--frame--//
    frame->begin(graphics.get_dc().Get());
    frame->render(graphics.get_dc().Get(), gauge.position + offset, gauge.scale, gauge.pivot, gauge.color, gauge.angle, gauge.texpos, gauge.texsize);
    frame->end(graphics.get_dc().Get());
}

void EnemyHpGauge::focus(const BaseEnemy* target_enemy, bool lockon)
{
    if (target_enemy != nullptr)
    {
        animation = lockon;
        focus_position = target_enemy->fGetPosition();
        offset = { 0,-70.0f };
    }
    else
    {
        animation = false;
        hp_percent = 1.0f;
    }
}