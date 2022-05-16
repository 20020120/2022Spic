#include "reticle.h"
#include "Operators.h"
#include "codinate_convert.h"

Reticle::Reticle(GraphicsPipeline& graphics)
{
    reticle = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\reticle.png", 1);

    element.scale   = { 0.15f, 0.15f };
    element.texsize = { static_cast<float>(reticle->get_texture2d_desc().Width), static_cast<float>(reticle->get_texture2d_desc().Height) };
    element.pivot   = element.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
    element.color.w = 0.0f;
}

void Reticle::update(GraphicsPipeline& graphics, float elapsed_time)
{
    static bool is_display_imgui = false;
    imgui_menu_bar("UI", "reticle", is_display_imgui);
#ifdef USE_IMGUI
    if (is_display_imgui)
    {
        ImGui::Begin("reticle");
        ImGui::DragFloat2("pos", &element.position.x);
        ImGui::DragFloat2("scale", &element.scale.x, 0.01f);
        ImGui::DragFloat2("offset", &offset.x, 0.1f);
        ImGui::Text("length:%f", length_player_to_enemy);
        ImGui::End();
    }
#endif // USE_IMGUI

    const float LERP_RATE = 5.0f;

    if (animation)
    {
        element.position = conversion_2D(graphics.get_dc().Get(), focus_position);

        element.color.w = Math::lerp(element.color.w, 0.7f, LERP_RATE * elapsed_time);

        element.scale = Math::lerp(element.scale, { 0.06f, 0.06f }, LERP_RATE * elapsed_time);

        if (element.color.w >= 0.6f) { element.angle += 30.0f * elapsed_time; }
        else { element.angle += 360.0f * elapsed_time; }
        if (element.angle >= 360.0f) { element.angle = 0; }
    }
    else
    {
        element.color.w = Math::lerp(element.color.w, 0.0f, LERP_RATE * elapsed_time);
    }
}

void Reticle::render(ID3D11DeviceContext* dc)
{
    //--reticle--//
    reticle->begin(dc);
    reticle->render(dc, element.position + offset, element.scale, element.pivot, element.color, element.angle, element.texpos, element.texsize);
    reticle->end(dc);
}

void Reticle::focus(const BaseEnemy* target_enemy, bool lockon)
{
    if (target_enemy != nullptr)
    {
        if (lockon && !animation)
        {
            element.scale   = { 0.15f, 0.15f };
            element.color.w = 0.0f;
        }
        animation = lockon;
        focus_position = target_enemy->fGetPosition();
        offset = { 0, -26.0f };
        length_player_to_enemy = target_enemy->fGetLengthFromPlayer();
    }
    else
    {
        animation = false;
    }
}