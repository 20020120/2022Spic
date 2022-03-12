#include "volume_icon.h"
#include "Operators.h"

VolumeIcon::VolumeIcon(ID3D11Device* device)
    : IconBase(device)
{
	//--master--//
	master.position = { 475.0f, 300.0f };
	master.scale    = { 0.6f, 0.6f };
	master.s = L"‘S‘Ì‰¹—Ê";
	//--bgm--//
	bgm.position = { 480.0f, 420.0f };
	bgm.scale    = { 0.6f, 0.6f };
	bgm.s = L"BGM‰¹—Ê";
	//--se--//
	se.position = { 480.0f, 540.0f };
	se.scale    = { 0.6f, 0.6f };
	se.s = L"SE‰¹—Ê";

	//--selecterL--//
	selecterL.position = { 360.0f, master.position.y };
	selecterL.scale    = { 0.3f, 0.3f };
	//--selecterR--//
	selecterR.position = { 590.0f, master.position.y };
	selecterR.scale    = { 0.3f, 0.3f };

	selecterL_arrival_pos = { selecterL.position.x, master.position.y };
	selecterR_arrival_pos = { selecterR.position.x, master.position.y };

	//--scales--//
	sprite_scale = std::make_unique<SpriteBatch>(device, L".\\resources\\Sprites\\option\\scale.png", MAX_SCALE_COUNT * BAR_COUNT * 2);
	float positions[BAR_COUNT] = { master.position.y, bgm.position.y, se.position.y };
	for (int i = 0; i < BAR_COUNT; ++i)
	{
		for (int o = 0; o < MAX_SCALE_COUNT; ++o)
		{
			scales[i].emplace_back();
			scales[i].at(o).texsize = { static_cast<float>(sprite_scale->get_texture2d_desc().Width), static_cast<float>(sprite_scale->get_texture2d_desc().Height) };
			scales[i].at(o).pivot = scales[i].at(o).texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
			scales[i].at(o).scale = { 0.5f, 0.6f };
			scales[i].at(o).color = { 1,1,1,1 };
			scales[i].at(o).position = { 670.0f + 20.0f * o, positions[i] };
		}
		for (int o = 0; o < MAX_SCALE_COUNT; ++o)
		{
			shell_scales[i].emplace_back();
			shell_scales[i].at(o).texsize  = { static_cast<float>(sprite_scale->get_texture2d_desc().Width), static_cast<float>(sprite_scale->get_texture2d_desc().Height) };
			shell_scales[i].at(o).pivot    = shell_scales[i].at(o).texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
			shell_scales[i].at(o).scale    = { 0.5f, 0.6f };
			shell_scales[i].at(o).color    = { 1,1,1,0.25f };
			shell_scales[i].at(o).position = { 670.0f + 20.0f * o, positions[i] };
		}
	}
	//--volume_numbers--//
	{
		Number* number = new Number(device);
		number->set_offset_pos({ 1125.0f, master.position.y });
		number->set_offset_scale({ 0.3f, 0.3f });
		volume_numbers.insert(std::make_pair(BarType::MASTER, number));

		number = new Number(device);
		number->set_offset_pos({ 1125.0f, bgm.position.y });
		number->set_offset_scale({ 0.3f, 0.3f });
		volume_numbers.insert(std::make_pair(BarType::BGM, number));

		number = new Number(device);
		number->set_offset_pos({ 1125.0f, se.position.y });
		number->set_offset_scale({ 0.3f, 0.3f });
		volume_numbers.insert(std::make_pair(BarType::SE, number));
	}
}

VolumeIcon::~VolumeIcon()
{
	for (int i = 0; i < BAR_COUNT; ++i)
	{
		scales[i].clear();
		shell_scales[i].clear();
	}
}

void VolumeIcon::update(GraphicsPipeline& graphics, float elapsed_time)
{
	interval_LX += elapsed_time;

	auto r_bar = [&](BarType type)
	{
		if ((game_pad->get_button() & GamePad::BTN_LEFT) && interval_LX > INTERVAL)
		{
			interval_LX = 0;
			if (!scales[type].empty()) { scales[type].pop_back(); }
		}
		if ((game_pad->get_button() & GamePad::BTN_RIGHT) && interval_LX > INTERVAL)
		{
			interval_LX = 0;
			float positions[BAR_COUNT] = { master.position.y, bgm.position.y, se.position.y };
			size_t index = scales[type].size();
			if (index < MAX_SCALE_COUNT)
			{
				scales[type].emplace_back();
				scales[type].at(index).texsize  = { static_cast<float>(sprite_scale->get_texture2d_desc().Width), static_cast<float>(sprite_scale->get_texture2d_desc().Height) };
				scales[type].at(index).pivot    = scales[type].at(index).texsize * DirectX::XMFLOAT2(0.5f, 0.5f);
				scales[type].at(index).scale    = { 0.5f, 0.6f };
				scales[type].at(index).color    = { 1,1,1,1 };
				scales[type].at(index).position = { 670.0f + 20.0f * index, positions[type] };
			}
		}
	};

	switch (state)
	{
	case BarType::MASTER:
		if (game_pad->get_button_down() & GamePad::BTN_DOWN)
		{
			state = BarType::BGM;
			selecterL_arrival_pos.y = bgm.position.y;
			selecterR_arrival_pos.y = bgm.position.y;
		}
		r_bar(BarType::MASTER);

		break;
	case BarType::BGM:
		if (game_pad->get_button_down() & GamePad::BTN_UP)
		{
			state = BarType::MASTER;
			selecterL_arrival_pos.y = master.position.y;
			selecterR_arrival_pos.y = master.position.y;
		}
		if (game_pad->get_button_down() & GamePad::BTN_DOWN)
		{
			state = BarType::SE;
			selecterL_arrival_pos.y = se.position.y;
			selecterR_arrival_pos.y = se.position.y;
		}
		r_bar(BarType::BGM);

		break;
	case BarType::SE:
		if (game_pad->get_button_down() & GamePad::BTN_UP)
		{
			state = BarType::BGM;
			selecterL_arrival_pos.y = bgm.position.y;
			selecterR_arrival_pos.y = bgm.position.y;
		}
		r_bar(BarType::SE);

		break;
	}

	selecterL.position = Math::lerp(selecterL.position, selecterL_arrival_pos, 10.0f * elapsed_time);
	selecterR.position = Math::lerp(selecterR.position, selecterR_arrival_pos, 10.0f * elapsed_time);

	for (int i = 0; i < BAR_COUNT; ++i)
	{
		if (volume_numbers.count(BarType(i)))
		{
			volume_numbers.at(BarType(i))->set_value(((float)scales[i].size() / (float)MAX_SCALE_COUNT) * 100);
			volume_numbers.at(BarType(i))->update(graphics, elapsed_time);
		}
	}
}

void VolumeIcon::render(std::string gui, ID3D11DeviceContext* dc, const DirectX::XMFLOAT2& add_pos)
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
	r_font_render("master", master);
	r_font_render("bgm", bgm);
	r_font_render("se", se);
	fonts->biz_upd_gothic->End(dc);

	sprite_scale->begin(dc);
	for (int i = 0; i < BAR_COUNT; ++i)
	{
		for (int o = 0; o < shell_scales[i].size(); ++o)
		{
			std::string s = "shell_scale" + std::to_string(i) + " " + std::to_string(o);
			r_sprite_render(s, shell_scales[i].at(o), sprite_scale.get());
		}
		for (int o = 0; o < scales[i].size(); ++o)
		{
			std::string s = "scale" + std::to_string(i) + " " + std::to_string(o);
			r_sprite_render(s, scales[i].at(o), sprite_scale.get());
		}
	}
	sprite_scale->end(dc);

#ifdef USE_IMGUI
	ImGui::Begin("option");
	for (int i = 0; i < BAR_COUNT; ++i)
	{
		static DirectX::XMFLOAT2 pos[BAR_COUNT]{};
		static DirectX::XMFLOAT2 scale[BAR_COUNT]{};
		std::string s1 = "number" + std::to_string(i);
		if (ImGui::TreeNode(s1.c_str()))
		{
			ImGui::DragFloat2("pos", &pos[i].x);
			ImGui::DragFloat2("scale", &scale[i].x, 0.1f);
			ImGui::TreePop();
		}
		if (volume_numbers.count(BarType(i)))
		{
			//volume_numbers.at(BarType(i))->set_offset_pos(pos[i]);
			//volume_numbers.at(BarType(i))->set_offset_scale(scale[i]);
		}
	}
	ImGui::End();
#endif // USE_IMGUI

	for (int i = 0; i < BAR_COUNT; ++i)
	{
		if (volume_numbers.count(BarType(i))) { volume_numbers.at(BarType(i))->render(dc, add_pos); }
	}
}