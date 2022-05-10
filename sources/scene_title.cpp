#include <thread>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

#include "framework.h"
#include "scene_title.h"
#include "scene_game.h"
#include "scene_loading.h"
#include "scene_manager.h"
#include "ModelCashe.h"
#include"SceneTutorial.h"

bool SceneTitle::is_ready = false;

void SceneTitle::initialize(GraphicsPipeline& graphics)
{
	//--タイトル前ロード--//
	ModelCashes::Load_PreTitle(graphics.get_device().Get());

	//----<3D関連>----//
	//ステージオブジェクト
	title_stage_model = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\stage\\title_stage.fbx", false, 60.0f);
	// player
	player = std::make_unique<Player>(graphics);
	player->TransitionTitleAnimationReadyIdle();
	// cameraManager
	cameraManager = std::make_unique<CameraManager>();
	cameraManager->RegisterCamera(new TitleCamera(player.get()));
	cameraManager->ChangeCamera(graphics, static_cast<int>(CameraTypes::Title));
	// shadow_map
	shadow_map = std::make_unique<ShadowMap>(graphics);
	// post_effect
	post_effect = std::make_unique<PostEffect>(graphics.get_device().Get());
	// bloom_effect
	bloom_effect = std::make_unique<Bloom>(graphics.get_device().Get(), SCREEN_WIDTH, SCREEN_HEIGHT);
	bloom_constants = std::make_unique<Constants<BloomConstants>>(graphics.get_device().Get());
	// sky_dome
	sky_dome = std::make_unique<SkyDome>(graphics);

	// pointlight
	point_lights = std::make_unique<PointLights>(graphics);

	//-------<2Dパート>--------//
	//--slash--//
	sprite_slash   = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\title\\title_slash.png", 1);
	slash.position = { 1280,360 };
	slash.scale    = { 1,1 };
	slash.texsize  = { static_cast<float>(sprite_slash->get_texture2d_desc().Width), static_cast<float>(sprite_slash->get_texture2d_desc().Height) };
	slash.color    = { 1,1,1,1 };
	//--selecter--//
	sprite_selecter    = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\title\\selecter.png", 2);
	selecter1.texsize  = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	selecter1.position = { 990.0f, 545.0f };
	selecter1.scale    = { 0.2f, 0.1f };

	selecter2.texsize  = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	selecter2.position = { 1167.0f, 545.0f };
	selecter2.scale    = { 0.2f, 0.1f };

	arrival_pos1 = selecter1.position;
	arrival_pos2 = selecter2.position;

	//--font--//
	beginning.s = L"初めから";
	beginning.position = { 1032, 522 };
	beginning.scale = { 0.7f,0.7f };

	succession.s = L"続きから";
	succession.position = { 1035, 575 };
	succession.scale = { 0.7f,0.7f };

	exit.s = L"ゲーム終了";
	exit.position = { 1025, 630 };
	exit.scale = { 0.7f,0.7f };

	now_loading.position = { 15,675 };
	now_loading.scale = { 0.6f,0.6f };

	//--state--//
	state = 0;
	std::filesystem::path path = "./resources/Data/tutorial.json";
	if (std::filesystem::exists(path.c_str())) /*チュートリアルデータあり*/ { have_tutorial_state = 0; }
	else /*チュートリアルデータなし*/ { have_tutorial_state = -1; }

	//--tutorial tab--//
	tutorial_tab.display = false;

	tutorial_tab.selecterL.texsize = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	tutorial_tab.selecterL.position = { 587, 335 };
	tutorial_tab.selecterL.scale = { 0.2f,0.1f };

	tutorial_tab.selecterR.texsize = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	tutorial_tab.selecterR.position = { 692, 335 };
	tutorial_tab.selecterR.scale = { 0.2f,0.1f };

	tutorial_tab.arrival_posL = tutorial_tab.selecterL.position;
	tutorial_tab.arrival_posR = tutorial_tab.selecterR.position;

	tutorial_tab.headline.s = L"チュートリアルを行いますか？";
	tutorial_tab.headline.position = { 465, 235 };
	tutorial_tab.headline.scale = { 0.7f,0.7f };

	tutorial_tab.back.s = L"Aボタンで戻る";
	tutorial_tab.back.position = { 685, 435 };
	tutorial_tab.back.scale = { 0.5f,0.5f };

	tutorial_tab.yes.s = L"はい";
	tutorial_tab.yes.position = { 625, 315 };
	tutorial_tab.yes.scale = { 0.7f,0.7f };

	tutorial_tab.no.s = L"いいえ";
	tutorial_tab.no.position = { 615, 370 };
	tutorial_tab.no.scale = { 0.7f,0.7f };

	//--slashing post effect--//
	slashing_power = 0;
	slashing_wait_timer = 0;

	audio_manager->stop_all_bgm();
	audio_manager->play_bgm(BGM_INDEX::TITLE);

	//スレッド開始
	std::thread thread(loading_thread, graphics.get_device().Get());
	//スレッドの管理を放棄
	thread.detach();
}

void SceneTitle::uninitialize()
{
	post_effect->clear_post_effect();
	audio_manager->stop_all_se();
}

void SceneTitle::update(GraphicsPipeline& graphics, float elapsed_time)
{
	static float bgm_volume = 2.0f;
	static float se_volume = 2.0f;

	audio_manager->set_volume_bgm(BGM_INDEX::TITLE, bgm_volume * VolumeFile::get_instance().get_master_volume() * VolumeFile::get_instance().get_bgm_volume());
	audio_manager->set_volume_se(SE_INDEX::SELECT, se_volume * VolumeFile::get_instance().get_master_volume() * VolumeFile::get_instance().get_se_volume());
	audio_manager->set_volume_se(SE_INDEX::DECISION, se_volume * VolumeFile::get_instance().get_master_volume() * VolumeFile::get_instance().get_se_volume());

	//----<3D関連>----//
	// cameraManager
	Camera* c = cameraManager->GetCurrentCamera();
	cameraManager->Update(elapsed_time);
	// shadow_map
	shadow_map->debug_imgui();
	// player
	player->SetCameraDirection(c->GetForward(), c->GetRight());
	player->SetCameraPosition(c->get_eye());
	player->UpdateTitle(elapsed_time);

	auto r_up = [&](int state, DirectX::XMFLOAT2 arrival_pos1, DirectX::XMFLOAT2 arrival_pos2)
	{
		if ((game_pad->get_button_down() & GamePad::BTN_UP) || game_pad->get_axis_LY() > 0.5f)
		{
			audio_manager->play_se(SE_INDEX::SELECT);
			this->state = state;
			this->arrival_pos1 = arrival_pos1;
			this->arrival_pos2 = arrival_pos2;
		}
	};
	auto r_down = [&](int state, DirectX::XMFLOAT2 arrival_pos1, DirectX::XMFLOAT2 arrival_pos2)
	{
		if ((game_pad->get_button_down() & GamePad::BTN_DOWN) || game_pad->get_axis_LY() < -0.5f)
		{
			audio_manager->play_se(SE_INDEX::SELECT);
			this->state = state;
			this->arrival_pos1 = arrival_pos1;
			this->arrival_pos2 = arrival_pos2;
		}
	};

	if (player->GetStartTitleAnimation() == false)
	{
		switch (state)
		{
		case 0: // beginning
			// tutorial_tab
			if (tutorial_tab.display)
			{
				auto r_up_tutorial = [&](int state, DirectX::XMFLOAT2 arrival_posL, DirectX::XMFLOAT2 arrival_posR)
				{
					if ((game_pad->get_button_down() & GamePad::BTN_UP) || game_pad->get_axis_LY() > 0.5f)
					{
						audio_manager->play_se(SE_INDEX::SELECT);
						have_tutorial_state = state;
						tutorial_tab.arrival_posL = arrival_posL;
						tutorial_tab.arrival_posR = arrival_posR;
					}
				};
				auto r_down_tutorial = [&](int state, DirectX::XMFLOAT2 arrival_posL, DirectX::XMFLOAT2 arrival_posR)
				{
					if ((game_pad->get_button_down() & GamePad::BTN_DOWN) || game_pad->get_axis_LY() < -0.5f)
					{
						audio_manager->play_se(SE_INDEX::SELECT);
						have_tutorial_state = state;
						tutorial_tab.arrival_posL = arrival_posL;
						tutorial_tab.arrival_posR = arrival_posR;
					}
				};
				switch (have_tutorial_state)
				{
				case 0: // はい
					r_down_tutorial(1, { 580,395 }, { 706,395 });
					break;

				case 1: // いいえ
					r_up_tutorial(0, { 587, 335 }, { 692, 335 });
					break;
				}

				tutorial_tab.selecterL.position = Math::lerp(tutorial_tab.selecterL.position, tutorial_tab.arrival_posL, 10.0f * elapsed_time);
				tutorial_tab.selecterR.position = Math::lerp(tutorial_tab.selecterR.position, tutorial_tab.arrival_posR, 10.0f * elapsed_time);

				// Aボタンで戻る
				if (is_ready && game_pad->get_button_down() & GamePad::BTN_A)
				{
					audio_manager->play_se(SE_INDEX::SELECT);
					tutorial_tab.display = false;
				}
				// 決定
				if (is_ready && game_pad->get_button_down() & GamePad::BTN_B)
				{
					audio_manager->play_se(SE_INDEX::DECISION);
					player->StartTitleAnimation();
					tutorial_tab.display = false;
					return;
				}
			}
			else
			{
				r_down(1, { 990.0f, 595.0f }, { 1167.0f, 595.0f });
				if (is_ready && game_pad->get_button_down() & GamePad::BTN_B)
				{
					if (have_tutorial_state >= 0) /* チュートリアルデータがあるのでタブ操作 */
					{
						audio_manager->play_se(SE_INDEX::SELECT);
						tutorial_tab.display = true;
					}
					else
					{
						audio_manager->play_se(SE_INDEX::DECISION);
						player->StartTitleAnimation();
						return;
					}
				}
			}
			break;

		case 1: // succession
			r_up(0, { 990.0f, 545.0f }, { 1167.0f, 545.0f });
			r_down(2, { 980.0f, 650.0f }, { 1190.0f, 650.0f });
			if (game_pad->get_button_down() & GamePad::BTN_B)
			{
				audio_manager->play_se(SE_INDEX::DECISION);
				player->StartTitleAnimation();
				return;
			}
			break;

		case 2: // exit
			r_up(1, { 990.0f, 595.0f }, { 1167.0f, 595.0f });
			if (game_pad->get_button_down() & GamePad::BTN_B)
			{
				audio_manager->play_se(SE_INDEX::DECISION);
				PostQuitMessage(0);
				return;
			}
			break;
		}
	}
	if (player->GetEndTitleAnimation())
	{
		if (slashing_power <= 0) // まずは少しずらす
		{
			slashing_power = 0.015f;
		}
		else
		{
			slashing_wait_timer += elapsed_time;
		}
		// 少しずつずらす
		if (slashing_wait_timer > 1.0f)
		{
			slashing_power += elapsed_time * 0.05f;
			slashing_power = (std::min)(slashing_power, SLASHING_MAX);
		}
	}

	// 画面遷移
	if (Math::equal_check(slashing_power, SLASHING_MAX))
	{
		if (have_tutorial_state == 1) /* チュートリアルなし */ { SceneManager::scene_switching(new SceneLoading(new SceneGame()), DISSOLVE_TYPE::DOT, 2.0f); }
		else{ SceneManager::scene_switching(new SceneLoading(new TutorialScene()), DISSOLVE_TYPE::DOT, 2.0f); }

		point_lights->finalize(graphics);
		is_ready = false;
	}

	selecter1.position = Math::lerp(selecter1.position, arrival_pos1, 10.0f * elapsed_time);
	selecter2.position = Math::lerp(selecter2.position, arrival_pos2, 10.0f * elapsed_time);

#ifdef USE_IMGUI
	ImGui::Begin("slashing_power");
	ImGui::DragFloat("slashing_power", &slashing_power, 0.01f);
	ImGui::End();
#endif // USE_IMGUI

	post_effect->title_post_effect(slashing_power);
}

void SceneTitle::render(GraphicsPipeline& graphics, float elapsed_time)
{
	// post_effect
	post_effect->begin(graphics.get_dc().Get());

	// シャドウマップのセット
	shadow_map->set_shadowmap(graphics);

	// カメラのビュー行列計算
	cameraManager->CalcViewProjection(graphics);


	/*-----!!!ここから上にオブジェクトの描画はしないで!!!!-----*/
	// sky_dome
	graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
	sky_dome->Render(graphics, elapsed_time);
	//タイトルオブジェクト
	graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
	title_stage_model->render(graphics.get_dc().Get(), Math::calc_world_matrix(title_stage_parm.scale, title_stage_parm.angle, title_stage_parm.pos), {1.0f,1.0f,1.0f,1.0f});
	// player
	player->TitleRender(graphics, elapsed_time);

	// point_lights
	point_lights->render(graphics, elapsed_time);


	//-------<2Dパート>--------//
	graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
	auto r_sprite_render = [&](std::string gui_name,SpriteBatch* batch, Element& e)
	{
		//--sprite_string--//
#ifdef USE_IMGUI
		ImGui::Begin("title");
		if (ImGui::TreeNode(gui_name.c_str()))
		{
			ImGui::DragFloat2("pos", &e.position.x, 0.1f);
			ImGui::DragFloat2("scale", &e.scale.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::End();
#endif // USE_IMGUI
		batch->begin(graphics.get_dc().Get());
		batch->render(graphics.get_dc().Get(), e.position, e.scale, e.pivot, e.color, e.angle, e.texpos, e.texsize);
		batch->end(graphics.get_dc().Get());
	};
	auto r_font_render = [&](std::string name, StepFontElement& e)
	{
#ifdef USE_IMGUI
		ImGui::Begin("title");
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::DragFloat2("pos", &e.position.x);
			ImGui::DragFloat2("scale", &e.scale.x, 0.1f);
			ImGui::ColorEdit4("color", &e.color.x);
			ImGui::TreePop();
		}
		ImGui::End();
#endif // USE_IMGUI
		fonts->yu_gothic->Draw(e.s, e.position, e.scale, e.color, e.angle, TEXT_ALIGN::UPPER_LEFT, e.length);
	};

	//--sprite_slash--//
	r_sprite_render("slash", sprite_slash.get(), slash);
	//--sprite_selecter--//
	r_sprite_render("selecter1", sprite_selecter.get(), selecter1);
	r_sprite_render("selecter2", sprite_selecter.get(), selecter2);

	//--font--//
	step_string(elapsed_time, L"ロード中...", now_loading, 2.0f, true);
	fonts->yu_gothic->Begin(graphics.get_dc().Get());
	r_font_render("beginning", beginning);
	r_font_render("succession", succession);
	r_font_render("exit", exit);
	if (!is_ready)	r_font_render("now_loading", now_loading);
	fonts->yu_gothic->End(graphics.get_dc().Get());


	if (tutorial_tab.display)
	{
		r_sprite_render("tab selecterL", sprite_selecter.get(), tutorial_tab.selecterL);
		r_sprite_render("tab selecterR", sprite_selecter.get(), tutorial_tab.selecterR);

		fonts->yu_gothic->Begin(graphics.get_dc().Get());
		r_font_render("headline", tutorial_tab.headline);
		r_font_render("back", tutorial_tab.back);
		r_font_render("yes", tutorial_tab.yes);
		r_font_render("no", tutorial_tab.no);
		fonts->yu_gothic->End(graphics.get_dc().Get());
	}


	/*-----!!!ここから下にオブジェクトの描画はしないで!!!!-----*/

	// シャドウマップの破棄
	shadow_map->clear_shadowmap(graphics);
	// post_effect
	post_effect->end(graphics.get_dc().Get());
	// 描画ステート設定
	graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEON_DWON);
	// エフェクトをかける
	post_effect->apply_an_effect(graphics.get_dc().Get(), elapsed_time);
	post_effect->blit(graphics.get_dc().Get());
	post_effect->scene_preview();
	// bloom
	{
		// 定数バッファにフェッチする
		static bool display_bloom_imgui = false;
#ifdef USE_IMGUI
		imgui_menu_bar("contents", "bloom", display_bloom_imgui);
		if (display_bloom_imgui)
		{
			ImGui::Begin("bloom");
			ImGui::DragFloat("extraction_threshold", &bloom_constants->data.bloom_extraction_threshold, 0.1f);
			ImGui::DragFloat("convolution_intensity", &bloom_constants->data.blur_convolution_intensity, 0.1f);
			ImGui::End();
		}
#endif // USE_IMGUI
		bloom_constants->bind(graphics.get_dc().Get(), 8);

		graphics.set_pipeline_preset(BLEND_STATE::NO_PROCESS, RASTERIZER_STATE::CULL_NONE, DEPTH_STENCIL::DEOFF_DWOFF);
		bloom_effect->make(graphics.get_dc().Get(), post_effect->get_color_map().Get());
		graphics.set_pipeline_preset(BLEND_STATE::ADD, RASTERIZER_STATE::CULL_NONE, DEPTH_STENCIL::DEOFF_DWOFF);
		bloom_effect->blit(graphics.get_dc().Get());
	}
}

void SceneTitle::register_shadowmap(GraphicsPipeline& graphics, float elapsed_time)
{
	Camera* c = cameraManager->GetCurrentCamera();
	//--シャドウマップの生成--//
	shadow_map->activate_shadowmap(graphics, c->get_light_direction());


	//--元のビューポートに戻す--//
	shadow_map->deactivate_shadowmap(graphics);
}

void SceneTitle::loading_thread(ID3D11Device* device)
{
	//--タイトル裏ロード--//
	ModelCashes::Load_IntoTitle(device);

	is_ready = true;
}

bool SceneTitle::step_string(float elapsed_time, std::wstring full_text,
	StepFontElement& step_font_element, float speed, bool loop)
{
	step_font_element.timer += elapsed_time * speed;
	step_font_element.step = static_cast<int>(step_font_element.timer);
	size_t size = full_text.size();
	if (step_font_element.index >= size + 1) // 一文字分時間を置く
	{
		if (!loop)
		{
			return true;
		}
		else
		{
			step_font_element.timer = 0.0f;
			step_font_element.step = 0;
			step_font_element.index = 0;
			step_font_element.s = L"";
		}
	}

	if (step_font_element.step % 2 == 0)
	{
		if (step_font_element.index < size)
		{
			step_font_element.s += full_text[step_font_element.index];
			step_font_element.step = 1;
			step_font_element.timer = 1.0f;
		}
		++step_font_element.index;
	}

	return false;
}