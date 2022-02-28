#include "player_config.h"
#include "Operators.h"
#include "imgui_include.h"
#include "user.h"

PlayerConfig::PlayerConfig(GraphicsPipeline& graphics)
{
    //----hp----//
    hp_frame = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_hp_flame.png", 1);
    hp_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_hp_back.png", 1);
    hp_body = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_hp_gage.png", 1);

    hp_gauge.position = { 320.0f, 645.0f };
    hp_gauge.scale    = { 0.8f, 0.8f };
    hp_gauge.texsize  = { static_cast<float>(hp_frame->get_texture2d_desc().Width), static_cast<float>(hp_frame->get_texture2d_desc().Height) };
    hp_gauge.pivot = hp_gauge.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
    //----mp----//
    mp_frame = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_mp_flame.png", 1);
    mp_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_mp_back.png", 1);
    mp_body = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_mp_gage.png", 1);

    mp_gauge.position = { 320.0f, 600.0f };
    mp_gauge.scale = { 0.8f, 0.8f };
    mp_gauge.texsize = { static_cast<float>(mp_frame->get_texture2d_desc().Width), static_cast<float>(mp_frame->get_texture2d_desc().Height) };
    mp_gauge.pivot = mp_gauge.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
    //----mp----//
    base_sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_ui_base.png", 1);

    base.position = { 270.0f, 620.0f };
    base.scale = { 0.8f, 0.8f };
    base.texsize = { static_cast<float>(base_sprite->get_texture2d_desc().Width), static_cast<float>(base_sprite->get_texture2d_desc().Height) };
    base.pivot = base.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
}

void PlayerConfig::update(GraphicsPipeline& graphics, float elapsed_time)
{
    static bool is_display_imgui = false;
    imgui_menu_bar("UI", "player config", is_display_imgui);
#ifdef USE_IMGUI
    if (is_display_imgui)
    {
        ImGui::Begin("player config");
        if (ImGui::TreeNode("base"))
        {
            ImGui::DragFloat2("pos", &base.position.x);
            ImGui::DragFloat2("scale", &base.scale.x, 0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("hp"))
        {
            ImGui::DragFloat2("pos", &hp_gauge.position.x);
            ImGui::DragFloat2("scale", &hp_gauge.scale.x, 0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("mp"))
        {
            ImGui::DragFloat2("pos", &mp_gauge.position.x);
            ImGui::DragFloat2("scale", &mp_gauge.scale.x, 0.01f);
            ImGui::TreePop();
        }
        ImGui::End();
    }
#endif // USE_IMGUI
}

void PlayerConfig::render(GraphicsPipeline& graphics, float elapsed_time)
{
    //--------<base>--------//
    base_sprite->begin(graphics.get_dc().Get());
    base_sprite->render(graphics.get_dc().Get(), base.position, base.scale, base.pivot, base.color, base.angle, base.texpos, base.texsize);
    base_sprite->end(graphics.get_dc().Get());
    //--------<hp>--------//
    //--back--//
    hp_back->begin(graphics.get_dc().Get());
    hp_back->render(graphics.get_dc().Get(), hp_gauge.position, hp_gauge.scale, hp_gauge.pivot, hp_gauge.color, hp_gauge.angle, hp_gauge.texpos, hp_gauge.texsize);
    hp_back->end(graphics.get_dc().Get());
    //--body--//
    hp_body->begin(graphics.get_dc().Get());
    hp_body->render(graphics.get_dc().Get(), hp_gauge.position, hp_gauge.scale, hp_gauge.pivot, hp_gauge.color, hp_gauge.angle, hp_gauge.texpos,
        { hp_gauge.texsize.x * hp_percent, hp_gauge.texsize.y });
    hp_body->end(graphics.get_dc().Get());
    //--frame--//
    hp_frame->begin(graphics.get_dc().Get());
    hp_frame->render(graphics.get_dc().Get(), hp_gauge.position, hp_gauge.scale, hp_gauge.pivot, hp_gauge.color, hp_gauge.angle, hp_gauge.texpos, hp_gauge.texsize);
    hp_frame->end(graphics.get_dc().Get());
    //--------<hp>--------//
    //--back--//
    mp_back->begin(graphics.get_dc().Get());
    mp_back->render(graphics.get_dc().Get(), mp_gauge.position, mp_gauge.scale, mp_gauge.pivot, mp_gauge.color, mp_gauge.angle, mp_gauge.texpos, mp_gauge.texsize);
    mp_back->end(graphics.get_dc().Get());
    //--body--//
    mp_body->begin(graphics.get_dc().Get());
    mp_body->render(graphics.get_dc().Get(), mp_gauge.position, mp_gauge.scale, mp_gauge.pivot, mp_gauge.color, mp_gauge.angle, mp_gauge.texpos,
        { mp_gauge.texsize.x * mp_percent, mp_gauge.texsize.y });
    mp_body->end(graphics.get_dc().Get());
    //--frame--//
    mp_frame->begin(graphics.get_dc().Get());
    mp_frame->render(graphics.get_dc().Get(), mp_gauge.position, mp_gauge.scale, mp_gauge.pivot, mp_gauge.color, mp_gauge.angle, mp_gauge.texpos, mp_gauge.texsize);
    mp_frame->end(graphics.get_dc().Get());
}