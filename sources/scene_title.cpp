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
//#define Tutorial
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


	//--back--//
	sprite_back  = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\title\\back.png", 1);
	back.texsize = { static_cast<float>(sprite_back->get_texture2d_desc().Width), static_cast<float>(sprite_back->get_texture2d_desc().Height) };
	back.color   = { 0.5f,0.5f,0.5f,1 };
	//--string--//
	sprite_string = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\title\\string.png", 2);
	start.texsize = { static_cast<float>(sprite_string->get_texture2d_desc().Width), static_cast<float>(sprite_string->get_texture2d_desc().Height) / 2.0f };
	start.pivot   = { start.texsize.x / 2.0f, 0 };
	start.texpos  = { 0, 0 };
	start.position = { 650.0f, 460.0f };

	exit.texsize  = { static_cast<float>(sprite_string->get_texture2d_desc().Width), static_cast<float>(sprite_string->get_texture2d_desc().Height) / 2.0f };
	exit.pivot    = { exit.texsize.x / 2.0f, 0 };
	exit.texpos   = { 0, exit.texsize.y };
	exit.position = { 730.0f, 575.0f };
	//--selecter--//
	sprite_selecter   = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\title\\selecter.png", 2);
	selecter1.texsize = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	selecter1.position = { 250.0f, 515.0f };
	selecter1.scale = { 0.7f, 0.3f };

	selecter2.texsize = { static_cast<float>(sprite_selecter->get_texture2d_desc().Width), static_cast<float>(sprite_selecter->get_texture2d_desc().Height) };
	selecter2.position = { 950.0f, 515.0f };
	selecter2.scale = { 0.7f, 0.3f };

	arrival_pos1 = { 250.0f, 515.0f };
	arrival_pos2 = { 950.0f, 515.0f };

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
}

void SceneTitle::update(GraphicsPipeline& graphics, float elapsed_time)
{
	static float bgm_volume = 2.0f;

	audio_manager->set_volume_bgm(BGM_INDEX::TITLE, bgm_volume * VolumeFile::get_instance().get_master_volume() * VolumeFile::get_instance().get_bgm_volume());

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

	if (player->GetStartTitleAnimation() == false)
	{
		switch (state)
		{
		case 0: // start
			if ((game_pad->get_button_down() & GamePad::BTN_DOWN) || game_pad->get_axis_LY() < -0.5f)
			{
				state = 1;
				arrival_pos1 = { 315.0f, 630.0f };
				arrival_pos2 = { 870.0f, 630.0f };
			}
			if (is_ready && game_pad->get_button_down() & GamePad::BTN_B)
			{
				player->StartTitleAnimation();
				return;
			}
			break;
		case 1: // exit
			if ((game_pad->get_button_down() & GamePad::BTN_UP) || game_pad->get_axis_LY() > 0.5f)
			{
				state = 0;
				arrival_pos1 = { 250.0f, 515.0f };
				arrival_pos2 = { 950.0f, 515.0f };
			}
			if (game_pad->get_button_down() & GamePad::BTN_B)
			{
				PostQuitMessage(0);
				return;
			}
			break;
		}
	}
	if (player->GetEndTitleAnimation())
	{
		if (slashing_power <= 0)
		{
			slashing_power = 0.015f;
		}
		else
		{
			slashing_wait_timer += elapsed_time;
		}
		if (slashing_wait_timer > 1.0f)
		{
			slashing_power += elapsed_time * 0.05f;
			slashing_power = (std::min)(slashing_power, SLASHING_MAX);
		}
	}

	if (Math::equal_check(slashing_power, SLASHING_MAX))
	{
#ifdef Tutorial
		SceneManager::scene_switching(new SceneLoading(new TutorialScene()), DISSOLVE_TYPE::DOT, 2.0f);
#else
		SceneManager::scene_switching(new SceneLoading(new SceneGame()), DISSOLVE_TYPE::DOT, 2.0f);
#endif // DEBUG

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


	graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
	//--sprite_back--//
	//sprite_back->begin(graphics.get_dc().Get());
	//sprite_back->render(graphics.get_dc().Get(), back.position, back.scale, back.pivot, back.color, back.angle, back.texpos, back.texsize);
	//sprite_back->end(graphics.get_dc().Get());
	//--sprite_string--//
#ifdef USE_IMGUI
	ImGui::Begin("start");
	ImGui::DragFloat2("pos", &start.position.x);
	ImGui::DragFloat2("scale", &start.scale.x, 0.01f);
	ImGui::End();
	ImGui::Begin("exit");
	ImGui::DragFloat2("pos", &exit.position.x);
	ImGui::DragFloat2("scale", &exit.scale.x, 0.01f);
	ImGui::End();
#endif // USE_IMGUI
	sprite_string->begin(graphics.get_dc().Get());
	sprite_string->render(graphics.get_dc().Get(), start.position, start.scale, start.pivot, start.color, start.angle, start.texpos, start.texsize);
	sprite_string->render(graphics.get_dc().Get(), exit.position, exit.scale, exit.pivot, exit.color, exit.angle, exit.texpos, exit.texsize);
	sprite_string->end(graphics.get_dc().Get());
	//--sprite_selecter--//
#ifdef USE_IMGUI
	ImGui::Begin("selecter1");
	ImGui::DragFloat2("pos", &selecter1.position.x);
	ImGui::DragFloat2("scale", &selecter1.scale.x, 0.01f);
	ImGui::End();
	ImGui::Begin("selecter2");
	ImGui::DragFloat2("pos", &selecter2.position.x);
	ImGui::DragFloat2("scale", &selecter2.scale.x, 0.01f);
	ImGui::End();
#endif // USE_IMGUI
	sprite_selecter->begin(graphics.get_dc().Get());
	sprite_selecter->render(graphics.get_dc().Get(), selecter1.position, selecter1.scale,
		selecter1.pivot, selecter1.color, selecter1.angle, selecter1.texpos, selecter1.texsize);
	sprite_selecter->render(graphics.get_dc().Get(), selecter2.position, selecter2.scale,
		selecter2.pivot, selecter2.color, selecter2.angle, selecter2.texpos, selecter2.texsize);
	sprite_selecter->end(graphics.get_dc().Get());


	static float speed = 2.0f;
#ifdef USE_IMGUI
	ImGui::Begin("step string");
	ImGui::DragFloat("speed", &speed, 0.1f);
	ImGui::End();
#endif // USE_IMGUI


	step_string(elapsed_time, L"UI省略コントローラー振動\nカメラシェイク", test, speed, true);
	auto r_font_render = [&](std::string name, StepFontElement& e)
	{
#ifdef USE_IMGUI
		ImGui::Begin(name.c_str());
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
	fonts->yu_gothic->Begin(graphics.get_dc().Get());
	r_font_render("test", test);
	fonts->yu_gothic->End(graphics.get_dc().Get());

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