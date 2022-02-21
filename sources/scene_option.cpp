#include "scene_option.h"
#include "scene_title.h"
#include "scene_loading.h"
#include "scene_manager.h"

Option::Option(GraphicsPipeline& graphics)
{
	//--back--//
	sprite_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\back.png", 1);
	back.texsize = { static_cast<float>(sprite_back->get_texture2d_desc().Width), static_cast<float>(sprite_back->get_texture2d_desc().Height) };
	back.color = { 0.5f,0.5f,0.5f,0.5f };
	//--string--//
	sprite_string = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\string.png", 2);
	game.texsize  = { static_cast<float>(sprite_string->get_texture2d_desc().Width), static_cast<float>(sprite_string->get_texture2d_desc().Height) / 2.0f };
	game.pivot    = { game.texsize.x / 2.0f, 0 };
	game.texpos   = { 0, 0 };
	game.position = { 690.0f, 210.0f };
	game.color = { 0,0,0,1 };

	title.texsize = { static_cast<float>(sprite_string->get_texture2d_desc().Width), static_cast<float>(sprite_string->get_texture2d_desc().Height) / 2.0f };
	title.pivot   = { title.texsize.x / 2.0f, 0 };
	title.texpos  = { 0, title.texsize.y };
	title.position = { 690.0f, 415.0f };
	title.color    = { 0,0,0,1 };
	//--selecter--//
	sprite_selecter    = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\option\\selecter.png", 2);
	selecter1.texsize  = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	selecter1.position = { 265.0f, 260.0f };
	selecter1.scale    = { 0.7f, 0.3f };
	selecter1.color = { 0,0,0,1 };

	selecter2.texsize  = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	selecter2.position = { 945.0f, 260.0f };
	selecter2.scale    = { 0.7f, 0.3f };
	selecter2.color = { 0,0,0,1 };

	arrival_pos1       = { 265.0f, 260.0f };
	arrival_pos2       = { 945.0f, 260.0f };
}

void Option::update(GraphicsPipeline& graphics, float elapsed_time)
{
	if (game_pad->get_button_down() & GamePad::BTN_START)
	{
		validity = false;
		return;
	}

	switch (state)
	{
	case 0: // game
		if ((game_pad->get_button_down() & GamePad::BTN_DOWN) || game_pad->get_axis_LY() < -0.5f)
		{
			state = 1;
			arrival_pos1 = { 265.0f, 465.0f };
			arrival_pos2 = { 945.0f, 465.0f };
		}
		if (game_pad->get_button_down() & GamePad::BTN_B)
		{
			validity = false;
			return;
		}
		break;
	case 1: // title
		if ((game_pad->get_button_down() & GamePad::BTN_UP) || game_pad->get_axis_LY() > 0.5f)
		{
			state = 0;
			arrival_pos1 = { 265.0f, 260.0f };
			arrival_pos2 = { 945.0f, 260.0f };
		}
		if (game_pad->get_button_down() & GamePad::BTN_B)
		{
			SceneManager::scene_switching(new SceneLoading(new SceneTitle()));
			return;
		}
		break;
	}

	selecter1.position = Math::lerp(selecter1.position, arrival_pos1, 10.0f * elapsed_time);
	selecter2.position = Math::lerp(selecter2.position, arrival_pos2, 10.0f * elapsed_time);
}

void Option::render(GraphicsPipeline& graphics, float elapsed_time)
{
	graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
	//--sprite_back--//
	sprite_back->begin(graphics.get_dc().Get());
	sprite_back->render(graphics.get_dc().Get(), back.position, back.scale, back.pivot, back.color, back.angle, back.texpos, back.texsize);
	sprite_back->end(graphics.get_dc().Get());
	//--sprite_string--//
#ifdef USE_IMGUI
	ImGui::Begin("game");
	ImGui::DragFloat2("pos", &game.position.x);
	ImGui::DragFloat2("scale", &game.scale.x, 0.01f);
	ImGui::End();
	ImGui::Begin("title");
	ImGui::DragFloat2("pos", &title.position.x);
	ImGui::DragFloat2("scale", &title.scale.x, 0.01f);
	ImGui::End();
#endif // USE_IMGUI
	sprite_string->begin(graphics.get_dc().Get());
	sprite_string->render(graphics.get_dc().Get(), game.position, game.scale, game.pivot, game.color, game.angle, game.texpos, game.texsize);
	sprite_string->render(graphics.get_dc().Get(), title.position, title.scale, title.pivot, title.color, title.angle, title.texpos, title.texsize);
	sprite_string->end(graphics.get_dc().Get());
	//--sprite_selecter--//
#ifdef USE_IMGUI
	ImGui::Begin("selecter1");
	ImGui::DragFloat2("pos", &selecter1.position.x);
	ImGui::DragFloat2("scale", &selecter1.scale.x, 0.01f);
	ImGui::ColorEdit4("color", &selecter1.color.x);
	ImGui::End();
	ImGui::Begin("selecter2");
	ImGui::DragFloat2("pos", &selecter2.position.x);
	ImGui::DragFloat2("scale", &selecter2.scale.x, 0.01f);
	ImGui::ColorEdit4("color", &selecter2.color.x);
	ImGui::End();
#endif // USE_IMGUI
	sprite_selecter->begin(graphics.get_dc().Get());
	sprite_selecter->render(graphics.get_dc().Get(), selecter1.position, selecter1.scale,
		selecter1.pivot, selecter1.color, selecter1.angle, selecter1.texpos, selecter1.texsize);
	sprite_selecter->render(graphics.get_dc().Get(), selecter2.position, selecter2.scale,
		selecter2.pivot, selecter2.color, selecter2.angle, selecter2.texpos, selecter2.texsize);
	sprite_selecter->end(graphics.get_dc().Get());
}