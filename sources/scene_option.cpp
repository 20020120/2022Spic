#include "scene_option.h"
#include "scene_title.h"
#include "scene_loading.h"
#include "scene_manager.h"
#include "Operators.h"

bool Option::validity = false;

Option::Option(GraphicsPipeline& graphics)
{
	//----icon_sprites----//
	{
		SpriteBatch* sprite = new SpriteBatch(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\volume_icon.png", 1);
		icon_sprites.insert(std::make_pair(IconType::VOLUME, sprite));
		sprite = new SpriteBatch(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\game_icon.png", 1);
		icon_sprites.insert(std::make_pair(IconType::GAME, sprite));
		sprite = new SpriteBatch(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\move_icon.png", 1);
		icon_sprites.insert(std::make_pair(IconType::TRANSITION, sprite));
	}
	//----icon_elements----//
	{
		Element element;
		element.texsize = { static_cast<float>(icon_sprites.at(IconType::VOLUME)->get_texture2d_desc().Width),
			                static_cast<float>(icon_sprites.at(IconType::VOLUME)->get_texture2d_desc().Height) };
		element.pivot = element.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
		element.scale = { 0.25f, 0.25f };
		element.color = { 1,1,1,1 };

		element.position = { 480.0f, 160.0f };
		icon_elements.insert(std::make_pair(IconType::VOLUME, element));
		element.position.x = 610.0f;
		icon_elements.insert(std::make_pair(IconType::GAME, element));
		element.position.x = 740.0f;
		icon_elements.insert(std::make_pair(IconType::TRANSITION, element));
	}
	//----icon_map----//
	{
		IconBase* icon = new VolumeIcon(graphics.get_device().Get());
		icon_map.insert(std::make_pair(IconType::VOLUME, icon));
		icon = new TransitionIcon(graphics.get_device().Get());
		icon_map.insert(std::make_pair(IconType::TRANSITION, icon));
	}
	//--back--//
	sprite_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\back.png", 1);
	back.position = { 0, 70 };
	back.texsize = { static_cast<float>(sprite_back->get_texture2d_desc().Width), static_cast<float>(sprite_back->get_texture2d_desc().Height) };
	back.color = { 1.0f,1.0f,1.0f,1.0f };
	//--tab--//
	sprite_tab = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\tab.png", 1);
	tab.position = { 0, 70 };
	tab.texsize = { static_cast<float>(sprite_tab->get_texture2d_desc().Width), static_cast<float>(sprite_tab->get_texture2d_desc().Height) };
	tab.color = { 1.0f,1.0f,1.0f,1.0f };
	//--frame--//
	sprite_frame = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\icon_frame.png", 1);
	if (icon_elements.count(state)) { frame.position = icon_elements.at(state).position; }
	frame.texsize = { static_cast<float>(sprite_frame->get_texture2d_desc().Width), static_cast<float>(sprite_frame->get_texture2d_desc().Height) };
	frame.pivot = frame.texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
	frame.color = { 1.0f,1.0f,1.0f,1.0f };
	frame.scale = { 0.25f, 0.25f };
}

void Option::update(GraphicsPipeline& graphics, float elapsed_time)
{
	if (game_pad->get_button_down() & GamePad::BTN_START)
	{
		validity = false;
		return;
	}
	//--icon--//
	if (icon_map.count(state)) { icon_map.at(state)->update(graphics, elapsed_time); }
}

void Option::render(GraphicsPipeline& graphics, float elapsed_time)
{
	graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
	auto r_render = [&](std::string gui, Element& e, SpriteBatch* s)
	{
#ifdef USE_IMGUI
		ImGui::Begin("option");
		if (ImGui::TreeNode(gui.c_str()))
		{
			ImGui::DragFloat2("pos", &e.position.x);
			ImGui::DragFloat2("scale", &e.scale.x, 0.1f);
			ImGui::ColorEdit4("color", &e.color.x);
			ImGui::TreePop();
		}
		ImGui::End();
#endif // USE_IMGUI
		s->begin(graphics.get_dc().Get());
		s->render(graphics.get_dc().Get(), e.position, e.scale, e.pivot, e.color, e.angle, e.texpos, e.texsize);
		s->end(graphics.get_dc().Get());
	};
	//--back--//
	r_render("back", back, sprite_back.get());
	//--tab--//
	r_render("tab", tab, sprite_tab.get());
	//--frame--//
	r_render("frame", frame, sprite_frame.get());
	//--icon--//
	r_render("volume",     icon_elements.at(IconType::VOLUME),     icon_sprites.at(IconType::VOLUME).get());
	r_render("game",       icon_elements.at(IconType::GAME),       icon_sprites.at(IconType::GAME).get());
	r_render("transition", icon_elements.at(IconType::TRANSITION), icon_sprites.at(IconType::TRANSITION).get());
	//--icons--//
	std::string gui_names[3] = { "volume", "game", "transition" };
	if (icon_map.count(state)) { icon_map.at(state)->render(gui_names[static_cast<int>(state)], graphics.get_dc().Get()); };
}