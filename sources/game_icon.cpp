#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <filesystem>
#include <fstream>

#include "game_icon.h"
#include "Operators.h"
#include "collision.h"

void GameFile::load()
{
	std::filesystem::path path = file_name;
	path.replace_extension(".json");
	if (std::filesystem::exists(path.c_str()))
	{
		std::ifstream ifs;
		ifs.open(path);
		if (ifs)
		{
			cereal::JSONInputArchive o_archive(ifs);
			o_archive(source);
		}
	}
	else
	{
		source.initialize();
	}
}

void GameFile::save()
{
	std::filesystem::path path = file_name;
	path.replace_extension(".json");
	std::ofstream ifs(path);
	if (ifs)
	{
		cereal::JSONOutputArchive o_archive(ifs);
		o_archive(source);
	}
}


GameIcon::GameIcon(ID3D11Device* device) : IconBase(device)
{
	GameFile::get_instance().load();
	//--shake--//
	shake.position = { 565.0f, 295.0f };
	shake.scale = { 0.6f, 0.6f };
	shake.s = L"カメラシェイク";
	//--vibration--//
	vibration.position = { 565.0f, 380.0f };
	vibration.scale = { 0.6f, 0.6f };
	vibration.s = L"コントローラー振動";
	//--omission--//
	omission.position = { 545.0f, 465.0f };
	omission.scale = { 0.6f, 0.6f };
	omission.s = L"UI省略";
	//--sensitivity--//
	sensitivity.position = { 550.0f, 550.0f };
	sensitivity.scale = { 0.6f, 0.6f };
	sensitivity.s = L"カメラ感度";

	//--button--//
	float on_pos_x = 900.0f; float off_pos_x = 1095.0f;
	// SHAKE
	choices[ChoicesType::SHAKE][0].position = { on_pos_x, shake.position.y };
	choices[ChoicesType::SHAKE][0].scale = { shake.scale };
	choices[ChoicesType::SHAKE][0].s = L"ON";
	choices[ChoicesType::SHAKE][1].position = { off_pos_x, shake.position.y };
	choices[ChoicesType::SHAKE][1].scale = { shake.scale };
	choices[ChoicesType::SHAKE][1].s = L"OFF";
	setup[ChoicesType::SHAKE] = GameFile::get_instance().get_shake();
	// VIBRATION
	choices[ChoicesType::VIBRATION][0].position = { on_pos_x, vibration.position.y };
	choices[ChoicesType::VIBRATION][0].scale = { vibration.scale };
	choices[ChoicesType::VIBRATION][0].s = L"ON";
	choices[ChoicesType::VIBRATION][1].position = { off_pos_x, vibration.position.y };
	choices[ChoicesType::VIBRATION][1].scale = { vibration.scale };
	choices[ChoicesType::VIBRATION][1].s = L"OFF";
	setup[ChoicesType::VIBRATION] = GameFile::get_instance().get_vibration();
	// OMISSION
	choices[ChoicesType::OMISSION][0].position = { on_pos_x, omission.position.y };
	choices[ChoicesType::OMISSION][0].scale = { omission.scale };
	choices[ChoicesType::OMISSION][0].s = L"ON";
	choices[ChoicesType::OMISSION][1].position = { off_pos_x, omission.position.y };
	choices[ChoicesType::OMISSION][1].scale = { omission.scale };
	choices[ChoicesType::OMISSION][1].s = L"OFF";
	setup[ChoicesType::OMISSION] = GameFile::get_instance().get_omission();
	// selecter
	DirectX::XMFLOAT2 selecter_texsize = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	DirectX::XMFLOAT2 selecter_pivot = { selecter_texsize * DirectX::XMFLOAT2(0.5f, 0.5f) };
	float selecter_posL[2] = { 835.0f,1005.0f }; float selecter_posR[2] = { 960.0f, 1160.0f };
	selecter[ChoicesType::SHAKE][0].position = { selecter_posL[setup[ChoicesType::SHAKE] ? 0 : 1], shake.position.y };
	selecter[ChoicesType::SHAKE][1].position = { selecter_posR[setup[ChoicesType::SHAKE] ? 0 : 1], shake.position.y };
	selecter[ChoicesType::VIBRATION][0].position = { selecter_posL[setup[ChoicesType::VIBRATION] ? 0 : 1], vibration.position.y };
	selecter[ChoicesType::VIBRATION][1].position = { selecter_posR[setup[ChoicesType::VIBRATION] ? 0 : 1], vibration.position.y };
	selecter[ChoicesType::OMISSION][0].position = { selecter_posL[setup[ChoicesType::OMISSION] ? 0 : 1], omission.position.y };
	selecter[ChoicesType::OMISSION][1].position = { selecter_posR[setup[ChoicesType::OMISSION] ? 0 : 1], omission.position.y };
	for (int i = 0; i < BUTTON_COUNT; ++i)
	{
		for (int o = 0; o < 2; ++o)
		{
			selecter[i][o].texsize = selecter_texsize;
			selecter[i][o].pivot = selecter_pivot;
			selecter[i][o].scale = { 0.3f, 0.3f };
			selecter[i][o].color = { 1,1,1,1 };
			selecter_arrival_pos[i][o] = { selecter[i][o].position };
		}
	}

	//--selecterL--//
	selecterL.position = { 395.0f, shake.position.y };
	selecterL.scale = { 0.3f, 0.3f };
	//--selecterR--//
	selecterR.position = { 705.0f, shake.position.y };
	selecterR.scale = { 0.3f, 0.3f };

	selecterL_arrival_pos = selecterL.position;
	selecterR_arrival_pos = selecterR.position;

	//--scales--//
	sprite_scale = std::make_unique<SpriteBatch>(device, L".\\resources\\Sprites\\option\\scale.png", MAX_SCALE_COUNT * 2);

	for (int o = 0; o < MAX_SCALE_COUNT * GameFile::get_instance().get_sensitivity(); ++o)
	{
		scales.emplace_back();
		scales.at(o).texsize = { static_cast<float>(sprite_scale->get_texture2d_desc().Width), static_cast<float>(sprite_scale->get_texture2d_desc().Height) };
		scales.at(o).pivot = scales.at(o).texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
		scales.at(o).scale = { 0.5f, 0.6f };
		scales.at(o).color = { 1,1,1,1 };
		scales.at(o).position = { 745.0f + 20.0f * o, sensitivity.position.y };
	}
	for (int o = 0; o < MAX_SCALE_COUNT; ++o)
	{
		shell_scales.emplace_back();
		shell_scales.at(o).texsize = { static_cast<float>(sprite_scale->get_texture2d_desc().Width), static_cast<float>(sprite_scale->get_texture2d_desc().Height) };
		shell_scales.at(o).pivot = shell_scales.at(o).texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
		shell_scales.at(o).scale = { 0.5f, 0.6f };
		shell_scales.at(o).color = { 1,1,1,0.25f };
		shell_scales.at(o).position = { 745.0f + 20.0f * o, sensitivity.position.y };
	}
	//--volume_numbers--//
	sensitivity_number = std::make_unique<Number>(device);
	sensitivity_number->set_offset_pos({ 1160.0f, sensitivity.position.y });
	sensitivity_number->set_offset_scale({ 0.25f, 0.25f });
}

GameIcon::~GameIcon()
{
	scales.clear();
	shell_scales.clear();
}

void GameIcon::update(GraphicsPipeline& graphics, float elapsed_time)
{
	interval_LX += elapsed_time;

	auto r_bar = [&]()
	{
		if ((game_pad->get_button() & GamePad::BTN_LEFT) && interval_LX > INTERVAL)
		{
			interval_LX = 0;
			size_t index = scales.size();
			if (index > 1) { scales.pop_back(); }
			save_source();
		}
		if ((game_pad->get_button() & GamePad::BTN_RIGHT) && interval_LX > INTERVAL)
		{
			interval_LX = 0;
			size_t index = scales.size();
			if (index < MAX_SCALE_COUNT)
			{
				scales.emplace_back();
				scales.at(index).texsize = { static_cast<float>(sprite_scale->get_texture2d_desc().Width), static_cast<float>(sprite_scale->get_texture2d_desc().Height) };
				scales.at(index).pivot = scales.at(index).texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
				scales.at(index).scale = { 0.5f, 0.6f };
				scales.at(index).color = { 1,1,1,1 };
				scales.at(index).position = { 745.0f + 20.0f * index, sensitivity.position.y };
			}
			save_source();
		}
	};
	auto r_button = [&](ChoicesType type)
	{
		float selecter_posL[2] = { 835.0f,1005.0f }; float selecter_posR[2] = { 960.0f, 1160.0f };
		if (game_pad->get_button_down() & GamePad::BTN_LEFT)
		{
			if (!setup[type])
			{
				setup[type] = true;
				selecter_arrival_pos[type][0].x = selecter_posL[0];
				selecter_arrival_pos[type][1].x = selecter_posR[0];
			}
			save_source();
		}
		if (game_pad->get_button_down() & GamePad::BTN_RIGHT)
		{
			if (setup[type])
			{
				setup[type] = false;
				selecter_arrival_pos[type][0].x = selecter_posL[1];
				selecter_arrival_pos[type][1].x = selecter_posR[1];
			}
			save_source();
		}
	};

	switch (state)
	{
	case ChoicesType::SHAKE:
		if (game_pad->get_button_down() & GamePad::BTN_DOWN)
		{
			state = ChoicesType::VIBRATION;
			selecterL_arrival_pos = { 360.0f, vibration.position.y };
			selecterR_arrival_pos = { 745.0f, vibration.position.y };
		}
		r_button(ChoicesType::SHAKE);
		break;

	case ChoicesType::VIBRATION:
		if (game_pad->get_button_down() & GamePad::BTN_UP)
		{
			state = ChoicesType::SHAKE;
			selecterL_arrival_pos = { 395.0f, shake.position.y };
			selecterR_arrival_pos = { 705.0f, shake.position.y };
		}
		if (game_pad->get_button_down() & GamePad::BTN_DOWN)
		{
			state = ChoicesType::OMISSION;
			selecterL_arrival_pos = { 385.0f, omission.position.y };
			selecterR_arrival_pos = { 705.0f, omission.position.y };
		}
		r_button(ChoicesType::VIBRATION);
		break;

	case ChoicesType::OMISSION:
		if (game_pad->get_button_down() & GamePad::BTN_UP)
		{
			state = ChoicesType::VIBRATION;
			selecterL_arrival_pos = { 360.0f, vibration.position.y };
			selecterR_arrival_pos = { 745.0f, vibration.position.y };
		}
		if (game_pad->get_button_down() & GamePad::BTN_DOWN)
		{
			state = ChoicesType::SENSITIVITY;
			selecterL_arrival_pos = { 410.0f, sensitivity.position.y };
			selecterR_arrival_pos = { 680.0f, sensitivity.position.y };
		}
		r_button(ChoicesType::OMISSION);
		break;

	case ChoicesType::SENSITIVITY:
		if (game_pad->get_button_down() & GamePad::BTN_UP)
		{
			state = ChoicesType::OMISSION;
			selecterL_arrival_pos = { 385.0f, omission.position.y };
			selecterR_arrival_pos = { 705.0f, omission.position.y };
		}
		r_bar();
		break;
	}

	//--左の選択肢--//
	selecterL.position = Math::lerp(selecterL.position, selecterL_arrival_pos, 10.0f * elapsed_time);
	selecterR.position = Math::lerp(selecterR.position, selecterR_arrival_pos, 10.0f * elapsed_time);
	//--右の選択肢--//
	for (int i = 0; i < BUTTON_COUNT; ++i)
	{
		for (int o = 0; o < 2; ++o)
		{
			selecter[i][o].position = Math::lerp(selecter[i][o].position, selecter_arrival_pos[i][o], 10.0f * elapsed_time);
		}
	}
	//--bar--//
	sensitivity_number->set_value(((float)scales.size() / (float)MAX_SCALE_COUNT) * 100);
	sensitivity_number->update(graphics, elapsed_time);
}

void GameIcon::render(std::string gui, ID3D11DeviceContext* dc, const DirectX::XMFLOAT2& add_pos)
{
	IconBase::render(gui, dc, add_pos);

	auto r_sprite_render = [&](std::string name, Element& e, SpriteBatch* s)
	{
#ifdef USE_IMGUI
		ImGui::Begin("option");
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::DragFloat2("pos", &e.position.x);
			ImGui::DragFloat2("scale", &e.scale.x, 0.1f);
			ImGui::ColorEdit4("color", &e.color.x);
			ImGui::TreePop();
		}
		ImGui::End();
#endif // USE_IMGUI
		s->render(dc, e.position + add_pos, e.scale, e.pivot, e.color, e.angle, e.texpos, e.texsize);
	};
	auto r_font_render = [&](std::string name, FontElement& e)
	{
#ifdef USE_IMGUI
		ImGui::Begin("option");
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::DragFloat2("pos", &e.position.x);
			ImGui::DragFloat2("scale", &e.scale.x, 0.1f);
			ImGui::ColorEdit4("color", &e.color.x);
			ImGui::TreePop();
		}
		ImGui::End();
#endif // USE_IMGUI
		fonts->biz_upd_gothic->Draw(e.s, e.position + add_pos, e.scale, e.color, e.angle, TEXT_ALIGN::MIDDLE, e.length);
	};

	fonts->biz_upd_gothic->Begin(dc);
	r_font_render("shake", shake);
	r_font_render("vibration", vibration);
	//r_font_render("omission", omission);
	//r_font_render("sensitivity", sensitivity);
	//--button--//
	//for (int i = 0; i < BUTTON_COUNT; ++i)
	for (int i = 0; i < 2; ++i)
	{
		for (int o = 0; o < 2; ++o)
		{
			std::string s = "choices" + std::to_string(i) + " " + std::to_string(o);
			r_font_render(s, choices[i][o]);
		}
	}
	fonts->biz_upd_gothic->End(dc);

	sprite_selecter->begin(dc);
	//for (int i = 0; i < BUTTON_COUNT; ++i)
	for (int i = 0; i < 2; ++i)
	{
		for (int o = 0; o < 2; ++o)
		{
			std::string s = "selecter" + std::to_string(i) + " " + std::to_string(o);
			r_sprite_render(s, selecter[i][o], sprite_selecter.get());
		}
	}
	sprite_selecter->end(dc);

	//--bar--//
	//sprite_scale->begin(dc);
	//for (int o = 0; o < shell_scales.size(); ++o)
	//{
	//	std::string s = "shell_scale " + std::to_string(o);
	//	r_sprite_render(s, shell_scales.at(o), sprite_scale.get());
	//}
	//for (int o = 0; o < scales.size(); ++o)
	//{
	//	std::string s = "scale " + std::to_string(o);
	//	r_sprite_render(s, scales.at(o), sprite_scale.get());
	//}
	//sprite_scale->end(dc);

#ifdef USE_IMGUI
	ImGui::Begin("option");
	static DirectX::XMFLOAT2 pos{};
	static DirectX::XMFLOAT2 scale{};
	if (ImGui::TreeNode("number"))
	{
		ImGui::DragFloat2("pos", &pos.x);
		ImGui::DragFloat2("scale", &scale.x, 0.1f);
		ImGui::TreePop();
	}
	//sensitivity_number->set_offset_pos(pos);
	//sensitivity_number->set_offset_scale(scale);
	ImGui::End();
#endif // USE_IMGUI

	//sensitivity_number->render(dc, add_pos);
}

void GameIcon::vs_cursor(const DirectX::XMFLOAT2& cursor_pos)
{
	for (int i = 0; i < BUTTON_COUNT; ++i)
	{
		float selecter_posL[2] = { 835.0f,1005.0f }; float selecter_posR[2] = { 960.0f, 1160.0f };
		for (int o = 0; o < 2; ++o)
		{
			DirectX::XMFLOAT2 length = choices[i][o].length * DirectX::XMFLOAT2(0.7f, 0.7f);
			if (Collision::hit_check_rect(cursor_pos, { 5,5 }, choices[i][o].position, length))
			{
				if (game_pad->get_button_down() & GamePad::BTN_B)
				{
					float selecter_arrival_pos_y[BUTTON_COUNT] = { shake.position.y, vibration.position.y, omission.position.y };
					float selecterL_arrival_pos_x[BUTTON_COUNT] = { 395.0f, 360.0f, 385.0f };
					float selecterR_arrival_pos_x[BUTTON_COUNT] = { 705.0f, 745.0f, 705.0f };
					state = ChoicesType(i);
					selecterL_arrival_pos = { selecterL_arrival_pos_x[i], selecter_arrival_pos_y[i] };
					selecterR_arrival_pos = { selecterR_arrival_pos_x[i], selecter_arrival_pos_y[i] };

					if (o == 0 && !setup[i]) // on
					{
						setup[i] = true;
						selecter_arrival_pos[i][0].x = selecter_posL[0];
						selecter_arrival_pos[i][1].x = selecter_posR[0];
					}
					if (o == 1 && setup[i]) // off
					{
						setup[i] = false;
						selecter_arrival_pos[i][0].x = selecter_posL[1];
						selecter_arrival_pos[i][1].x = selecter_posR[1];
					}
					save_source();
				}
			}
		}
	}
	//--bar--//
	static DirectX::XMFLOAT2 value = { 20,0 };
#ifdef USE_IMGUI
	ImGui::Begin("test");
	ImGui::DragFloat2("value", &value.x, 0.1f);
	ImGui::End();
#endif

	DirectX::XMFLOAT2 bar_radius = { (shell_scales.at(shell_scales.size() - 1).position.x - shell_scales.begin()->position.x) / 2,
		shell_scales.begin()->texsize.y * shell_scales.begin()->scale.y };
	DirectX::XMFLOAT2 bar_position = { shell_scales.begin()->position.x + bar_radius.x, shell_scales.begin()->position.y };
	if (Collision::hit_check_rect(cursor_pos, { 5,5 }, bar_position, bar_radius + value))
	{
		if (game_pad->get_button_down() & GamePad::BTN_B)
		{
			state = ChoicesType::SENSITIVITY;
			selecterL_arrival_pos = { 410.0f, sensitivity.position.y };
			selecterR_arrival_pos = { 680.0f, sensitivity.position.y };

			int index = 0;
			float distance = 100;
			for (int i = 0; i < shell_scales.size(); ++i)
			{
				if (distance > fabsf(shell_scales.at(i).position.x - cursor_pos.x + FLT_EPSILON))
				{
					distance = fabsf(shell_scales.at(i).position.x - cursor_pos.x + FLT_EPSILON);
					index = i;
				}
			}
			scales.clear();
			for (int i = 0; i < index + 1; ++i)
			{
				scales.emplace_back();
				scales.at(i).texsize = { static_cast<float>(sprite_scale->get_texture2d_desc().Width), static_cast<float>(sprite_scale->get_texture2d_desc().Height) };
				scales.at(i).pivot = scales.at(i).texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
				scales.at(i).scale = { 0.5f, 0.6f };
				scales.at(i).color = { 1,1,1,1 };
				scales.at(i).position = { 745.0f + 20.0f * i, sensitivity.position.y };
			}
			save_source();
		}
	}
}

void GameIcon::save_source()
{
	GameFile::get_instance().set_shake(setup[ChoicesType::SHAKE]);
	GameFile::get_instance().set_vibration(setup[ChoicesType::VIBRATION]);
	GameFile::get_instance().set_omission(setup[ChoicesType::OMISSION]);
	GameFile::get_instance().set_sensitivity((float)scales.size() / (float)MAX_SCALE_COUNT);

	GameFile::get_instance().save();
}