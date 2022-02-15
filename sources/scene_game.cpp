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
	// fog_post_effect
	fog_post_effect = std::make_unique<FullScreenQuad>(graphics.get_device().Get());
	create_ps_from_cso(graphics.get_device().Get(), "shaders/fog_ps.cso", fog_ps.ReleaseAndGetAddressOf());
	// mist
	mist_constants = std::make_unique<Constants<MistConstants>>(graphics.get_device().Get());
	// bloom
	bloom_effect = std::make_unique<Bloom>(graphics.get_device().Get(), SCREEN_WIDTH, SCREEN_HEIGHT);
	bloom_constants = std::make_unique<Constants<BloomConstants>>(graphics.get_device().Get());
	// point_lights
	point_lights = std::make_unique<PointLights>(graphics);
	// モデルのロード
	skinned_meshes[0] = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\blender_gis\\akihabara.fbx", true);
	skinned_meshes[1] = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\nico\\nico.fbx");
	skinned_meshes[2] = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\FactoryMachine\\FactoryMachine.fbx", true);
	//skinned_meshes[3] = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\Player\\stage1_1.fbx", true);
	skinned_meshes[4] = resource_manager->load_model_resource(graphics.get_device().Get(), ".\\resources\\Models\\sky\\cube.000.fbx", false);
	// effect
	test_effect = std::make_unique<Effect>(graphics, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\appear\\appear_blue.efk");
	test_effect2 = std::make_unique<Effect>(graphics, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\appear\\appear_green.efk");
	// player
	skinned_meshes[1]->play_animation(0, true);
	player.position = {};
	player.scale = { 0.05f,0.05f,0.05f };
	player.angle = { 0,3.2f,0 };
	gravity = 1.0f;
	// stage
	stage.scale = { 1.0f,1.0f,1.0f };
	stage.angle = { 0,0,0 };
	stage.position = { 0,0,0 };
	stage.coordinate = Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP, 0.1f);
	// HuskParticles
	particles = std::make_unique<HuskParticles>(graphics.get_device().Get());

	//--------------------<敵の管理クラスを初期化>--------------------//
	mEnemyManager.fInitialize(graphics.get_device().Get());
}

void SceneGame::uninitialize() {}

// Convert a position in screen space to a position on near plane in world space.
DirectX::XMFLOAT4 convert_pixel_to_world(ID3D11DeviceContext* device_context,
	const DirectX::XMFLOAT2& pixel, UINT viewport_index, const DirectX::XMFLOAT4X4& view_projection)
{
	D3D11_VIEWPORT viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	UINT viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	device_context->RSGetViewports(&viewport_count, viewports);
	_ASSERT_EXPR(viewport_count > viewport_index, L"");

	DirectX::XMFLOAT4 p{};
	DirectX::XMStoreFloat4(&p,
		DirectX::XMVector3TransformCoord(
			DirectX::XMVector3TransformCoord(
				DirectX::XMVectorSet(pixel.x, pixel.y, 0/*near panel*/, 1),
				DirectX::XMMatrixInverse(NULL,
					DirectX::XMMatrixSet(
						viewports[viewport_index].Width * 0.5f, 0.0f, 0.0f, 0.0f,
						0.0f, -viewports[viewport_index].Height * 0.5f, 0.0f, 0.0f,
						0.0f, 0.0f, viewports[viewport_index].MaxDepth - viewports[viewport_index].MinDepth, 0.0f,
						viewports[viewport_index].TopLeftX + viewports[viewport_index].Width * 0.5f, viewports[viewport_index].Height * 0.5f + viewports[viewport_index].TopLeftY, viewports[viewport_index].MinDepth, 1.0f))
			), DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&view_projection))
		)
	);
	return p;
}

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
		if (ImGui::Button("play_effect2"))
		{
			test_effect2->play(effect_manager->get_effekseer_manager(), { 0,1,0 }, 3);
		}
		{
			static DirectX::XMFLOAT3 pos{};
			ImGui::DragFloat3("position2", &pos.x, 0.1f);
			test_effect2->set_position(effect_manager->get_effekseer_manager(), pos);
			static DirectX::XMFLOAT3 scale{1,1,1};
			ImGui::DragFloat3("scale2", &scale.x, 0.1f);
			test_effect2->set_scale(effect_manager->get_effekseer_manager(), scale);
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

	skinned_meshes[1]->update_animation(elapsed_time);
	// レイキャストによる地面補完
	static bool is_stop = false;
	float velocity_y = gravity * elapsed_time;
	if (!is_stop) player.position.y -= velocity_y;
	if (velocity_y > 0.0f)
	{
		DirectX::XMFLOAT3 start = { player.position.x, player.position.y + 0.5f, player.position.z };
		DirectX::XMFLOAT3 end = { player.position.x, player.position.y - (velocity_y), player.position.z };
		// レイキャスト
		HitResult hit;
		if (Collision::ray_vs_model(start, end,
			skinned_meshes[0].get(), stage.position, stage.scale, stage.angle, stage.coordinate, hit))
		{
			is_stop = true;
			player.position = hit.position;
		}
		else
		{
			is_stop = false;
		}
	}

#ifdef USE_IMGUI
	if (display_player_imgui)
	{
		ImGui::Begin("player_para");
		ImGui::DragFloat("velocity_y", &velocity_y);
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

	// AABB同士の当たり判定
#if 0
	{
		static DirectX::XMFLOAT3 aabb_pos_a{};
		static DirectX::XMFLOAT3 aabb_radius_a{ 1,1,1 };

		static DirectX::XMFLOAT3 aabb_pos_b{};
		static DirectX::XMFLOAT3 aabb_radius_b{ 1,1,1 };
		static DirectX::XMFLOAT3 aabb_velocity_b{};
		static float speed = 3.0f;

		DirectX::XMFLOAT3 ax{};
		if (mouse->game_button() & Mouse::BTN_D_RIGHT) { ax.x = 1; }
		if (mouse->game_button() & Mouse::BTN_A_LEFT) { ax.x = -1; }
		if (mouse->game_button() & Mouse::BTN_Q) { ax.y = 1; }
		if (mouse->game_button() & Mouse::BTN_E) { ax.y = -1; }
		if (mouse->game_button() & Mouse::BTN_W_UP) { ax.z = 1; }
		if (mouse->game_button() & Mouse::BTN_S_DOWN) { ax.z = -1; }
		aabb_velocity_b.x = ax.x * speed * elapsed_time;
		aabb_velocity_b.y = ax.y * speed * elapsed_time;
		aabb_velocity_b.z = ax.z * speed * elapsed_time;
		bool hit = Collision::cuboid_vs_cuboid(aabb_pos_a, aabb_radius_a, aabb_pos_b, aabb_radius_b, aabb_velocity_b);
		// 移動
		aabb_pos_b.x += aabb_velocity_b.x;
		aabb_pos_b.y += aabb_velocity_b.y;
		aabb_pos_b.z += aabb_velocity_b.z;

		DirectX::XMFLOAT3 aabb_max_pos_b = { aabb_pos_b.x + aabb_radius_b.x, aabb_pos_b.y + aabb_radius_b.y, aabb_pos_b.z + aabb_radius_b.z };
		DirectX::XMFLOAT3 aabb_min_pos_b = { aabb_pos_b.x - aabb_radius_b.x, aabb_pos_b.y - aabb_radius_b.y, aabb_pos_b.z - aabb_radius_b.z };
		bool frustom_hit = Collision::frustum_vs_cuboid(aabb_min_pos_b, aabb_max_pos_b);

#ifdef USE_IMGUI
		ImGui::Begin("aabb intersect");
		ImGui::DragFloat3("pos a", &aabb_pos_a.x, 0.1f);
		ImGui::DragFloat3("radius a", &aabb_radius_a.x, 0.1f);
		ImGui::DragFloat3("pos b", &aabb_pos_b.x, 0.1f);
		ImGui::DragFloat3("radius b", &aabb_radius_b.x, 0.1f);
		ImGui::DragFloat3("velocity b", &aabb_velocity_b.x, 0.1f);
		ImGui::DragFloat("speed b", &speed, 0.1f);
		ImGui::Checkbox("hit", &hit);
		ImGui::Checkbox("frustom_hit", &frustom_hit);
		ImGui::End();
#endif
	}
#endif // 0
	// カプセル同士の当たり判定
	{
		static DirectX::XMFLOAT3 capsule_start_a{ 0,0,0 };
		static DirectX::XMFLOAT3 capsule_end_a{ 0,1,0 };
		static float capsule_radius_a{ 1 };

		static DirectX::XMFLOAT3 capsule_start_b{ 1,0,0 };
		static DirectX::XMFLOAT3 capsule_end_b{ 1,1,0 };
		static float capsule_radius_b{ 1 };
		static DirectX::XMFLOAT3 capsule_velocity_b{};
		static float speed = 3.0f;

		DirectX::XMFLOAT3 ax{};
		if (mouse->game_button() & Mouse::BTN_D_RIGHT) { ax.x = 1; }
		if (mouse->game_button() & Mouse::BTN_A_LEFT)  { ax.x = -1; }
		if (mouse->game_button() & Mouse::BTN_Q)       { ax.y = 1; }
		if (mouse->game_button() & Mouse::BTN_E)       { ax.y = -1; }
		if (mouse->game_button() & Mouse::BTN_W_UP)    { ax.z = 1; }
		if (mouse->game_button() & Mouse::BTN_S_DOWN)  { ax.z = -1; }
		capsule_velocity_b.x = ax.x * speed * elapsed_time;
		capsule_velocity_b.y = ax.y * speed * elapsed_time;
		capsule_velocity_b.z = ax.z * speed * elapsed_time;
		// 移動
		capsule_start_b.x += capsule_velocity_b.x;
		capsule_start_b.y += capsule_velocity_b.y;
		capsule_start_b.z += capsule_velocity_b.z;

#if 1
		bool hit = Collision::capsule_vs_capsule(capsule_start_a, capsule_end_a, capsule_radius_a,
			capsule_start_b, capsule_end_b, capsule_radius_b);
#else
		bool hit = Collision::sphere_vs_capsule(capsule_start_b, capsule_radius_b,
			capsule_start_a, capsule_end_a, capsule_radius_a);
#endif // 0

#ifdef USE_IMGUI
		ImGui::Begin("capsule intersect");
		ImGui::DragFloat3("start a", &capsule_start_a.x, 0.1f);
		ImGui::DragFloat3("end a", &capsule_end_a.x, 0.1f);
		ImGui::DragFloat("radius a", &capsule_radius_a, 0.01f);

		ImGui::DragFloat3("start b", &capsule_start_b.x, 0.1f);
		ImGui::DragFloat3("end b", &capsule_end_b.x, 0.1f);
		ImGui::DragFloat("radius b", &capsule_radius_b, 0.01f);

		ImGui::DragFloat("speed b", &speed, 0.1f);
		ImGui::Checkbox("hit", &hit);
		ImGui::End();
#endif
	}

	// Convert a position in screen space to a position on near plane in world space.
	if (mouse->game_button_down() & Mouse::BTN_LEFT_CLICK)
	{
		DirectX::XMFLOAT4X4 view_pro = camera->get_view_projection();
		particles->particle_constants->data.position_on_near_plane = convert_pixel_to_world(graphics.get_dc().Get(),
			{ static_cast<float>(mouse->get_cursor_pos().x), static_cast<float>(mouse->get_cursor_pos().y) }, 0.0f, view_pro);
	}
	particles->particle_constants->data.eye_position = { camera->get_eye().x, camera->get_eye().y, camera->get_eye().z, 1.0f };

	// HUSK
	if (integrate_particles)
	{
		particles->integrate(graphics.get_dc().Get(), elapsed_time);
	}
#ifdef USE_IMGUI
	// HUSK
	ImGui::Begin("husk particles");
	ImGui::Text("accumulated husk particle count %d", particles->particle_constants->data.particle_count);
	if (ImGui::Checkbox("integrate_particles", &integrate_particles))
	{
		accumulate_husk_particles = false;
	}
	if (ImGui::Checkbox("accumulate_husk_particles", &accumulate_husk_particles))
	{
		integrate_particles = false;
	}
	ImGui::DragFloat("particle_data.size", &particles->particle_constants->data.particle_size, 0.001f, 0, 100);
	ImGui::End();
#endif

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
		const float dimension{ 350.0f };
		skinned_meshes[4]->render(graphics.get_dc().Get(),
			{ dimension, 0.0f, 0.0f, 0.0f, 0.0f, dimension, 0.0f, 0.0f, 0.0f, 0.0f, dimension, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }, { 1,1,1,1 });
	}

	// ステージ
	{
		// 拡大縮小（S）・回転（R）・平行移動（T）行列を計算する
		static DirectX::XMFLOAT4 material_color = { 1,1,1,1 };
		static bool display_stage_imgui = false;
#ifdef USE_IMGUI
		imgui_menu_bar("objects", "stage", display_stage_imgui);
		if (display_stage_imgui)
		{
			ImGui::Begin("stage");
			if (ImGui::TreeNode("transform"))
			{
				ImGui::DragFloat3("scale", &stage.scale.x, 0.1f, 0, 30);
				ImGui::DragFloat3("angle", &stage.angle.x, 0.1f, -10, 10);
				ImGui::DragFloat3("position", &stage.position.x, 0.1f, -10, 10);
				ImGui::ColorPicker4("material_color", &material_color.x);
				ImGui::TreePop();
			}
			ImGui::End();
		}
#endif
		// 描画
		// 描画ステート設定
		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::HALF_LAMBERT);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&stage.coordinate) };
		DirectX::XMMATRIX W{ DirectX::XMLoadFloat4x4(&Math::calc_world_matrix(stage.scale, stage.angle, stage.position)) };
		DirectX::XMStoreFloat4x4(&world, C * W);
		skinned_meshes[0]->render(graphics.get_dc().Get(), world, material_color);
	}
	// nico
	{
		// 拡大縮小（S）・回転（R）・平行移動（T）行列を計算する
		static DirectX::XMFLOAT4 material_color = { 1,1,1,1 };
		static bool loop_anim = true;
		const char* shaders[] = { "SPECULAR", "TOON", "HALF_LAMBERT" };
		static int shader_type = 0;
#ifdef USE_IMGUI
		imgui_menu_bar("objects", "player", display_player_imgui);
		if (display_player_imgui)
		{
			ImGui::Begin("player_para");
			ImGui::Combo("shader type", &shader_type, shaders, IM_ARRAYSIZE(shaders));
			ImGui::InputInt("anime_index", &player.anim_index);
			ImGui::Checkbox("loop_anim", &loop_anim);
			bool end_of_animation = skinned_meshes[1]->end_of_animation();
			ImGui::Checkbox("end_of_animation", &end_of_animation);
			if (ImGui::Button("play_animation"))
			{
				skinned_meshes[1]->play_animation(player.anim_index, loop_anim);
			}
			if (ImGui::Button("pause_animation"))
			{
				skinned_meshes[1]->pause_animation();
			}
			if (ImGui::Button("progress_animation"))
			{
				skinned_meshes[1]->progress_animation();
			}
			if (ImGui::TreeNode("transform"))
			{
				ImGui::DragFloat3("scale", &player.scale.x, 0.1f, 0, 5);
				ImGui::DragFloat3("angle", &player.angle.x, 0.1f, -10, 10);
				ImGui::DragFloat3("position", &player.position.x, 0.1f, -100, 100);
				ImGui::ColorPicker4("material_color", &material_color.x);
				ImGui::TreePop();
			}
			ImGui::End();
		}
#endif
		// 描画ステート設定
		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, (SHADER_TYPES)shader_type);
		// スタティックメッシュ
		DirectX::XMFLOAT4X4 world;
		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP, 0.1f)) };
		DirectX::XMMATRIX W{ DirectX::XMLoadFloat4x4(&Math::calc_world_matrix(player.scale, player.angle, player.position)) };
		DirectX::XMStoreFloat4x4(&world, C * W);
		skinned_meshes[1]->render(graphics.get_dc().Get(), world, material_color);

		// HUSK
		if (accumulate_husk_particles)
		{
			graphics.set_pipeline_preset(BLEND_STATE::NO_PROCESS, RASTERIZER_STATE::CULL_NONE, DEPTH_STENCIL::DEOFF_DWOFF);

			DirectX::XMStoreFloat4x4(&world, DirectX::XMLoadFloat4x4(&world) * DirectX::XMMatrixTranslation(+1, 0, 0));
			particles->accumulate_husk_particles(graphics.get_dc().Get(), [&]() {
				skinned_meshes[1]->render(graphics.get_dc().Get(), world, material_color);
				});
		}
	}

	// HUSK
	graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::CULL_NONE, DEPTH_STENCIL::DEON_DWON);
	particles->render(graphics.get_dc().Get());

	// PBR test
	{
		// 拡大縮小（S）・回転（R）・平行移動（T）行列を計算する
		static DirectX::XMFLOAT3  position = { 0,3,0 };
		static DirectX::XMFLOAT3  angle = { 0,3.1f,0 };
		static DirectX::XMFLOAT3  scale = { 0.1f,0.1f,0.1f };
		static DirectX::XMFLOAT4 material_color = { 1,1,1,1 };
		static bool loop_anim = true;
		const char* shaders[] = { "SPECULAR", "TOON", "HALF_LAMBERT", "PBR" };
		static int shader_type = 3;
#ifdef USE_IMGUI
		ImGui::Begin("PBR test");
		if (ImGui::Button("regeneration"))
		{
			skinned_meshes[2]->regeneration(graphics.get_device().Get(), ".\\resources\\Models\\FactoryMachine\\FactoryMachine.fbx");
		}
		ImGui::Combo("shader type", &shader_type, shaders, IM_ARRAYSIZE(shaders));
		if (ImGui::TreeNode("transform"))
		{
			ImGui::DragFloat3("angle", &angle.x, 0.1f, -10, 10);
			ImGui::DragFloat3("position", &position.x, 0.1f, -100, 100);
			ImGui::DragFloat3("scale", &scale.x, 0.1f, -10, 10);
			ImGui::ColorPicker4("material_color", &material_color.x);
			ImGui::TreePop();
		}
		ImGui::End();
#endif
		// 描画ステート設定
		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, (SHADER_TYPES)shader_type);
		// スタティックメッシュ
		DirectX::XMFLOAT4X4 world;
		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP, 0.1f)) };
		DirectX::XMMATRIX W{ DirectX::XMLoadFloat4x4(&Math::calc_world_matrix(scale, angle, position)) };
		DirectX::XMStoreFloat4x4(&world, C * W);
		skinned_meshes[2]->render(graphics.get_dc().Get(), world, material_color);
	}
	// player
	{
//		// 拡大縮小（S）・回転（R）・平行移動（T）行列を計算する
//		static DirectX::XMFLOAT3  position = { 0,3,0 };
//		static DirectX::XMFLOAT3  angle = { 0,0,0 };
//		static DirectX::XMFLOAT3  scale = { 0.5f,0.5f,0.5f };
//		static DirectX::XMFLOAT4 material_color = { 1,1,1,1 };
//		static bool loop_anim = true;
//		const char* shaders[] = { "SPECULAR", "TOON", "HALF_LAMBERT", "PBR" };
//		static int shader_type = 3;
//#ifdef USE_IMGUI
//		ImGui::Begin("player");
//		ImGui::Combo("shader type", &shader_type, shaders, IM_ARRAYSIZE(shaders));
//		if (ImGui::TreeNode("transform"))
//		{
//			ImGui::DragFloat3("angle", &angle.x, 0.1f, -10, 10);
//			ImGui::DragFloat3("position", &position.x, 0.1f, -100, 100);
//			ImGui::DragFloat3("scale", &scale.x, 0.1f, -10, 10);
//			ImGui::ColorPicker4("material_color", &material_color.x);
//			ImGui::TreePop();
//		}
//		ImGui::End();
//#endif
//		// 描画ステート設定
//		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, (SHADER_TYPES)shader_type);
//		// スタティックメッシュ
//		DirectX::XMFLOAT4X4 world;
//		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP, 0.1f)) };
//		DirectX::XMMATRIX W{ DirectX::XMLoadFloat4x4(&Math::calc_world_matrix(scale, angle, position)) };
//		DirectX::XMStoreFloat4x4(&world, C * W);
//		skinned_meshes[3]->render(graphics.get_dc().Get(), world, material_color);
	}
	// point_light
	{
		point_lights->render(graphics, elapsed_time);
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
	// fog
	{
		static bool display_fog_imgui = false;
#ifdef USE_IMGUI
		imgui_menu_bar("contents", "fog", display_fog_imgui);
		if (display_fog_imgui)
		{
			ImGui::Begin("fog");
			ImGui::ColorEdit4("fog_color", &mist_constants->data.fog_color.x);
			ImGui::DragFloat2("fog_range", &mist_constants->data.fog_range.x, 0.1f);
			ImGui::End();
		}
#endif // USE_IMGUI
		mist_constants->bind(graphics.get_dc().Get(), 7);
		// 深度マップのセット
		graphics.get_dc()->PSSetShaderResources(5, 1, post_effect->get_depth_map().GetAddressOf());
		static bool is_mist = false;
#ifdef USE_IMGUI
		if (display_fog_imgui)
		{
			ImGui::Begin("fog");
			ImGui::Checkbox("enable", &is_mist);
			ImGui::End();
		}
#endif // USE_IMGUI
		if (is_mist) fog_post_effect->blit(graphics.get_dc().Get(), post_effect->get_color_map().GetAddressOf(), 0, 1, fog_ps.Get());
	}
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

	// シャドウマップの生成
	shadow_map->activate_shadowmap(graphics, camera->get_light_direction());

	// ステージ
	{
		// 描画ステート設定
		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::NONE);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&stage.coordinate) };
		DirectX::XMMATRIX W{ DirectX::XMLoadFloat4x4(&Math::calc_world_matrix(stage.scale, stage.angle, stage.position)) };
		DirectX::XMStoreFloat4x4(&world, C * W);
		skinned_meshes[0]->render(graphics.get_dc().Get(), world, { 1,1,1,1 });
	}
	// player
	{
		// 描画ステート設定
		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::NONE);
		// 登録
		DirectX::XMFLOAT4X4 world;
		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&Math::conversion_coordinate_system(Math::COORDINATE_SYSTEM::RHS_YUP, 0.1f)) };
		DirectX::XMMATRIX W{ DirectX::XMLoadFloat4x4(&Math::calc_world_matrix(player.scale, player.angle, player.position)) };
		DirectX::XMStoreFloat4x4(&world, C * W);
		skinned_meshes[1]->render(graphics.get_dc().Get(), world, { 1,1,1,1 });
	}

	// 元のビューポートに戻す
	shadow_map->deactivate_shadowmap(graphics);

#endif // SHADOW_MAP
}