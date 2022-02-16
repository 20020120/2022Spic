#include <memory>

#include "scene_game.h"
#include "scene_title.h"
#include "scene_loading.h"
#include "scene_manager.h"

#include "framework.h"
#include "graphics_pipeline.h"

#include "collision.h"
#include "shader.h"
#include "texture.h"

#include "user.h"

void SceneGame::initialize(GraphicsPipeline& graphics)
{
	// カメラ
	camera = std::make_unique<Camera>(graphics);
	// shadow_map
	shadow_map = std::make_unique<ShadowMap>(graphics);
	// post effect
	post_effect = std::make_unique<PostEffect>(graphics.get_device().Get());
	// bloom
	bloom_effect = std::make_unique<Bloom>(graphics.get_device().Get(), SCREEN_WIDTH, SCREEN_HEIGHT);
	bloom_constants = std::make_unique<Constants<BloomConstants>>(graphics.get_device().Get());
	// モデルのロード
	sky_dome = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\sky\\cube.fbx", false);
	test = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\player\\player_proto.fbx", false);
	// effect
	test_effect = std::make_unique<Effect>(graphics, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\bomb_2.efk");

	//--------------------<敵の管理クラスを初期化>--------------------//
	mEnemyManager.fInitialize(graphics.get_device().Get());
}

void SceneGame::uninitialize() {}

void SceneGame::update(GraphicsPipeline& graphics, float elapsed_time)
{
	//--------------------<敵の管理クラスの更新処理>--------------------//
	mEnemyManager.fUpdate(elapsed_time);

	// camera
	camera->update_with_quaternion(elapsed_time);
	// shadow_map
	shadow_map->debug_imgui();

	effect_manager->update(elapsed_time);

	// effect demo
#ifdef USE_IMGUI
	{
		ImGui::Begin("effect");
		if (ImGui::Button("play_effect"))
		{
			test_effect->play(effect_manager->get_effekseer_manager(), { 0,1,0 }, 3);
		}
		{
			static DirectX::XMFLOAT3 pos{};
			ImGui::DragFloat3("position", &pos.x, 0.1f);
			test_effect->set_position(effect_manager->get_effekseer_manager(), pos);
			static DirectX::XMFLOAT3 scale{1,1,1};
			ImGui::DragFloat3("scale", &scale.x, 0.1f);
			test_effect->set_scale(effect_manager->get_effekseer_manager(), scale);
		}
		ImGui::End();
	}
#endif // USE_IMGUI
	// camera shake demo
#ifdef USE_IMGUI
	static bool is_shake = false;
	ImGui::Begin("camera_shake");
	if (ImGui::Checkbox("shake", &is_shake))
	{
		camera_shake->reset(graphics);
	}
	if (is_shake) camera_shake->shake(graphics, elapsed_time);
	ImGui::End();
#endif // USE_IMGUI
	// hit_stop demo
#ifdef USE_IMGUI
	{
		ImGui::Begin("hit_stop");
		if (ImGui::Button("stop")) { hit_stop->damage_hit_stop(); }
		ImGui::End();
	}
#endif // USE_IMGUI

	// gamepad
	if (game_pad->get_button_down() & GamePad::BTN_A)
	{
		// 押した瞬間
		SceneManager::scene_switching(new SceneLoading(new SceneTitle()), DISSOLVE_TYPE::TYPE2, 0.5f);
		return;
	}

	// audio デモ
	static float bgm_volume = 0.2f;
	static float se_volume = 0.2f;
	static bool is_open_button = { false };
	static bool display_audio_imgui = { false };
#ifdef USE_IMGUI
	imgui_menu_bar("contents", "audio", display_audio_imgui);
	if (display_audio_imgui)
	{
		if (ImGui::Begin("sound"))
		{
			if (ImGui::TreeNode("BGM"))
			{
				ImGui::DragFloat("bgm_volume", &bgm_volume, 0.1f, 0.0f, 3.0f);
				//if (!is_open_button)
				{
					if (ImGui::Button("play bgm")) { audio_manager->play_bgm(BGM_INDEX::ENDING); is_open_button = true; }
					if (ImGui::Button("play TITLE bgm")) { audio_manager->play_bgm(BGM_INDEX::TITLE); is_open_button = true; }
				}
				//else
				{
					if (ImGui::Button("stop bgm")) { audio_manager->stop_bgm(BGM_INDEX::ENDING); is_open_button = false; }
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SE"))
			{
				ImGui::DragFloat("se_volume", &se_volume, 0.1f, 0.0f, 3.0f);
				if (ImGui::Button("play se")) { audio_manager->play_se(SE_INDEX::DECISION); }
				if (ImGui::Button("play item_get se")) { audio_manager->play_se(SE_INDEX::GET); }
				ImGui::TreePop();
			}
			ImGui::End();
		}
	}
#endif
	audio_manager->set_volume_bgm(BGM_INDEX::ENDING, bgm_volume);
	audio_manager->set_volume_se(SE_INDEX::DECISION, se_volume);
}

#define OFF_SCREEN_RENDERING
#define SHADOW_MAP

void SceneGame::render(GraphicsPipeline& graphics, float elapsed_time)
{
#ifdef OFF_SCREEN_RENDERING
	post_effect->begin(graphics.get_dc().Get());
#endif // DEBUG

	// シャドウマップのセット
#ifdef SHADOW_MAP
	shadow_map->set_shadowmap(graphics);
#endif // SHADOW_MAP
	// カメラのビュー行列計算
	camera->calc_view_projection(graphics);

	/*-----!!!ここから上にオブジェクトの描画はしないで!!!!-----*/
	// skymap(オブジェクト描画の最初)
	{
		// 描画ステート設定
		graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::SKYMAP);
		static float dimension{ 350.0f };
		//ImGui::Begin("sky");
		//ImGui::DragFloat("dimension", &dimension, 0.1f);
		//ImGui::End();
		sky_dome->render(graphics.get_dc().Get(),
			{ dimension, 0.0f, 0.0f, 0.0f, 0.0f, dimension, 0.0f, 0.0f, 0.0f, 0.0f, dimension, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }, { 1,1,1,1 });
	}

	{
		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::PBR);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP, 0.1f)) };
		DirectX::XMMATRIX W{ DirectX::XMLoadFloat4x4(&Math::calc_world_matrix({1,1,1}, {0,0,0}, {0,0,0})) };
		DirectX::XMStoreFloat4x4(&world, C * W);
		test->render(graphics.get_dc().Get(), world, { 1,1,1,1 });
	}


	effect_manager->render(Camera::get_keep_view(), Camera::get_keep_projection());
	if (DebugFlags::get_wireframe_switching())
	{
		graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::WIREFRAME_CULL_BACK, DEPTH_STENCIL::DEON_DWON);
		debug_figure->render_all_figures(graphics.get_dc().Get());
	}

    // font demo
#if 0
	{
		graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
		fonts->gothic->Begin(graphics.get_dc().Get());
		fonts->gothic->Draw(L"テスト", { 640, 360 }, { 1,1 }, { 0,0,0,1 }, 0, TEXT_ALIGN::MIDDLE);
		fonts->gothic->End(graphics.get_dc().Get());
	}
#endif // 0

	//--------------------<敵の管理クラスの描画処理>--------------------//
	mEnemyManager.fRender(graphics.get_dc().Get());

	/*-----!!!ここから下にオブジェクトの描画はしないで!!!!-----*/

	// シャドウマップの破棄
#ifdef SHADOW_MAP
	shadow_map->clear_shadowmap(graphics);
#endif // SHADOW_MAP

#ifdef OFF_SCREEN_RENDERING
	post_effect->end(graphics.get_dc().Get());
	// 描画ステート設定
	graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEON_DWON);
	// エフェクトをかける
	post_effect->apply_an_effect(graphics.get_dc().Get(), elapsed_time);
	post_effect->scene_preview();
#endif // DEBUG
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

void SceneGame::register_shadowmap(GraphicsPipeline& graphics, float elapsed_time)
{
#ifdef SHADOW_MAP
	//--シャドウマップの生成--//
	shadow_map->activate_shadowmap(graphics, camera->get_light_direction());







	//--元のビューポートに戻す--//
	shadow_map->deactivate_shadowmap(graphics);
#endif // SHADOW_MAP
}