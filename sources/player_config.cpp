#include "player_config.h"
#include "Operators.h"
#include "imgui_include.h"
#include "user.h"

PlayerConfig::PlayerConfig(GraphicsPipeline& graphics)
{
    //----hp----//
    hp_frame = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_hp_flame.png", 1);
    hp_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_hp_back.png", 1);
    hp_body_sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_hp_gage.png", 1);

    hp_body.position = { 120.0f, 60.0f };
    hp_body.scale    = { 0.8f, 0.8f };
    hp_body.texsize  = { static_cast<float>(hp_body_sprite->get_texture2d_desc().Width), static_cast<float>(hp_body_sprite->get_texture2d_desc().Height) };
    hp_body.pivot    = hp_body.texsize * DirectX::XMFLOAT2(0.5f, 1.0f);
    hp_body.texpos   = hp_body.texsize * DirectX::XMFLOAT2(0, 1.0f);
    hp_body.angle = 180.0f;

    hp_gauge.position = { 120.0f, 285.0f };
    hp_gauge.scale    = { 0.8f, 0.8f };
    hp_gauge.texsize  = { static_cast<float>(hp_frame->get_texture2d_desc().Width), static_cast<float>(hp_frame->get_texture2d_desc().Height) };
    hp_gauge.pivot    = hp_gauge.texsize * DirectX::XMFLOAT2(0.5f, 1.0f);
    //----mp----//
    mp_frame = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_mp_flame.png", 1);
    mp_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_mp_back.png", 1);
    mp_body_sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_mp_gage.png", 1);

    mp_body.position = { 190.0f, 60.0f };
    mp_body.scale = { 0.8f, 0.8f };
    mp_body.texsize = { static_cast<float>(mp_body_sprite->get_texture2d_desc().Width), static_cast<float>(mp_body_sprite->get_texture2d_desc().Height) };
    mp_body.pivot  = mp_body.texsize * DirectX::XMFLOAT2(0.5f, 1.0f);
    mp_body.texpos = mp_body.texsize * DirectX::XMFLOAT2(0, 1.0f);
    mp_body.angle = 180.0f;

    mp_gauge.position = { 190.0f, 160.0f };
    mp_gauge.scale = { 0.8f, 0.8f };
    mp_gauge.texsize = { static_cast<float>(mp_frame->get_texture2d_desc().Width), static_cast<float>(mp_frame->get_texture2d_desc().Height) };
    mp_gauge.pivot = mp_gauge.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
    //----mp----//
    base_sprite  = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_gage_base_1.png", 1);
    base2_sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\player\\player_gage_base_2.png", 1);

    base.position = { 125.0f, 160.0f };
    base.scale = { 0.9f, 0.9f };
    base.texsize = { static_cast<float>(base_sprite->get_texture2d_desc().Width), static_cast<float>(base_sprite->get_texture2d_desc().Height) };
    base.pivot = base.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
}

void PlayerConfig::update(GraphicsPipeline& graphics, float elapsed_time)
{
    glow_vertical -= elapsed_time * 0.2f;

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
        if (ImGui::TreeNode("hp_body"))
        {
            ImGui::DragFloat2("pos", &hp_body.position.x);
            ImGui::DragFloat2("scale", &hp_body.scale.x, 0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("mp"))
        {
            ImGui::DragFloat2("pos", &mp_gauge.position.x);
            ImGui::DragFloat2("scale", &mp_gauge.scale.x, 0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("mp_body"))
        {
            ImGui::DragFloat2("pos", &mp_body.position.x);
            ImGui::DragFloat2("scale", &mp_body.scale.x, 0.01f);
            ImGui::TreePop();
        }
        ImGui::End();
    }
#endif // USE_IMGUI
}

void PlayerConfig::render(ID3D11DeviceContext* dc)
{
    //--------<base>--------//
    base2_sprite->begin(dc);
    base2_sprite->render(dc, base.position, base.scale, base.pivot, base.color, base.angle, base.texpos, base.texsize, 0, glow_vertical);
    base2_sprite->end(dc);
    base_sprite->begin(dc);
    base_sprite->render(dc, base.position, base.scale, base.pivot, base.color, base.angle, base.texpos, base.texsize);
    base_sprite->end(dc);
    //--------<hp>--------//
    //--back--//
    hp_back->begin(dc);
    hp_back->render(dc, hp_gauge.position, hp_gauge.scale, hp_gauge.pivot, hp_gauge.color, hp_gauge.angle, hp_gauge.texpos, hp_gauge.texsize);
    hp_back->end(dc);
    //--body--//
    hp_body_sprite->begin(dc);
    hp_body_sprite->render(dc, hp_body.position, hp_body.scale, hp_body.pivot, hp_body.color, hp_body.angle, hp_body.texpos,
        { hp_body.texsize.x, -hp_body.texsize.y * hp_percent });
    hp_body_sprite->end(dc);
    //--frame--//
    hp_frame->begin(dc);
    hp_frame->render(dc, hp_gauge.position, hp_gauge.scale, hp_gauge.pivot, hp_gauge.color, hp_gauge.angle, hp_gauge.texpos, hp_gauge.texsize);
    hp_frame->end(dc);
    //--------<hp>--------//
    //--back--//
    mp_back->begin(dc);
    mp_back->render(dc, mp_gauge.position, mp_gauge.scale, mp_gauge.pivot, mp_gauge.color, mp_gauge.angle, mp_gauge.texpos, mp_gauge.texsize);
    mp_back->end(dc);
    //--body--//
    mp_body_sprite->begin(dc);
    mp_body_sprite->render(dc, mp_body.position, mp_body.scale, mp_body.pivot, mp_body.color, mp_body.angle, mp_body.texpos,
        { mp_body.texsize.x, -mp_body.texsize.y * mp_percent });
    mp_body_sprite->end(dc);
    //--frame--//
    mp_frame->begin(dc);
    mp_frame->render(dc, mp_gauge.position, mp_gauge.scale, mp_gauge.pivot, mp_gauge.color, mp_gauge.angle, mp_gauge.texpos, mp_gauge.texsize);
    mp_frame->end(dc);
}