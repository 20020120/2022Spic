#include "SceneTutorial.h"
#include <cereal/archives/json.hpp>

#include <memory>
#include "scene_title.h"
#include "scene_loading.h"
#include "scene_manager.h"

#include "framework.h"
#include "graphics_pipeline.h"

#include "collision.h"
#include "shader.h"
#include "texture.h"

#include "user.h"


void TutorialScene::initialize(GraphicsPipeline& graphics)
{

	// Jsonにかきだし
	std::filesystem::path path = "./resources/Data/tutorial.json";
	path.replace_extension(".json");
	int a = 1;
	std::ofstream ifs(path);
	if (ifs)
	{
		cereal::JSONOutputArchive o_archive(ifs);
		o_archive(a);
	}

	// shadow_map
	shadow_map = std::make_unique<ShadowMap>(graphics);
	// post effect
	post_effect = std::make_unique<PostEffect>(graphics.get_device().Get());
	// bloom
	bloom_effect = std::make_unique<Bloom>(graphics.get_device().Get(), SCREEN_WIDTH, SCREEN_HEIGHT);
	bloom_constants = std::make_unique<Constants<BloomConstants>>(graphics.get_device().Get());
	// モデルのロード
	sky_dome = std::make_unique<SkyDome>(graphics);
	// effect
	test_effect = std::make_unique<Effect>(graphics, effect_manager->get_effekseer_manager(), ".\\resources\\Effect\\enemy_vernier.efk");

	//--------------------<弾の管理クラスを初期化>--------------------//
	BulletManager& mBulletManager = BulletManager::Instance();
	mBulletManager.fInitialize();
	//--------------------<敵の管理クラスを初期化>--------------------//
	mWaveManager.fInitialize(graphics, mBulletManager.fGetAddFunction());

	player = std::make_unique<Player>(graphics);
	// カメラ
	cameraManager = std::make_unique<CameraManager>();

	cameraManager->RegisterCamera(new GameCamera(player.get()));
	cameraManager->RegisterCamera(new ClearCamera(player.get()));
	//cameraManager->SetCamera(static_cast<int>(CameraTypes::Game));
	//cameraManager->Initialize(graphics);
	cameraManager->ChangeCamera(graphics, static_cast<int>(CameraTypes::Game));

	// enemy_hp_gauge
	enemy_hp_gauge = std::make_unique<EnemyHpGauge>(graphics);
	// reticle
	reticle = std::make_unique<Reticle>(graphics);
	// wave
	wave = std::make_unique<Counter>(graphics, L".\\resources\\Sprites\\ui\\wave.png");
	// option
	option = std::make_unique<Option>(graphics);
	// tunnel
	tunnel = std::make_unique<Tunnel>(graphics.get_device().Get());
	//mini_map
	minimap = std::make_unique<MiniMap>(graphics);

	check_mark = std::make_unique<SpriteDissolve>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\CheckMark.png",
		L".\\resources\\Sprites\\mask\\dissolve_mask1.png", 1, true);
	check_box = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\CheckBox.png", 1);

	//チュートリアル文の初期化
	tutorial_text_element[0].tutorial_text = L"Lスティックでプレイヤーを動かすことができ\nRスティックでカメラを動かすことができます";
	tutorial_text_element[1].tutorial_text = L"RBボタンかRTボタンを押すと回避することができます";
	tutorial_text_element[2].tutorial_text = L"敵が視界に入っている時にLTボタンを押している間\nロックオンすることができます";
	tutorial_text_element[3].tutorial_text = L"Bボタンを押すと攻撃でき,ロックオン中は自動で敵を攻撃します\n敵に当たったらもう一度Bボタンを押して連続攻撃できます";
	tutorial_text_element[4].tutorial_text = L"ロックオン中に一定の近さの時に回避をすると回り込み,スタンさせます";
	tutorial_text_element[5].tutorial_text = L"LBボタンを押し続けるとスタンしている敵をロックオンして\n一度に攻撃することができます";
	tutorial_text_element[6].tutorial_text = L"敵に攻撃するとゲージがたまっていき\n満タンの状態でAボタンを押すと覚醒状態になります。";

	tutorial_text_element[0].position = { 304.0f,556.0f };
	tutorial_text_element[1].position = { 307.0f,595.0f };
	tutorial_text_element[2].position = { 309.0f,573.0f };
	tutorial_text_element[3].position = { 295.0f,516.0f };
	tutorial_text_element[4].position = { 277.0f,566.0f };

}

void TutorialScene::uninitialize()
{
	BulletManager& mBulletManager = BulletManager::Instance();

	mWaveManager.fFinalize();
	mBulletManager.fFinalize();
}

void TutorialScene::update(GraphicsPipeline& graphics, float elapsed_time)
{
	TutorialUpdate(graphics, elapsed_time);

	// option
	if (option->get_validity())
	{
		option->update(graphics, elapsed_time);
		return;
	}
	else
	{
		if (game_pad->get_button_down() & GamePad::BTN_START)
		{
			option->initialize();
			option->set_validity(true);
			return;
		}
	}
	BulletManager& mBulletManager = BulletManager::Instance();

	// クリア演出
	mWaveManager.fUpdate(graphics, elapsed_time, mBulletManager.fGetAddFunction());
	if (mWaveManager.during_clear_performance())
	{
		tunnel_alpha += elapsed_time * 0.5f;
		tunnel_alpha = (std::min)(tunnel_alpha, 1.0f);

		if (!during_clear)
		{
			cameraManager->ChangeCamera(graphics, static_cast<int>(CameraTypes::Tunnel));
			player->TransitionStageMove();
			during_clear = true;
		}

		//return;
	}
	else
	{
		if (during_clear)
		{
			tunnel_alpha -= elapsed_time;
			tunnel_alpha = (std::max)(tunnel_alpha, 0.0f);
			if (Math::equal_check(tunnel_alpha, 0.0f, 0.01f))
			{
				cameraManager->ChangeCamera(graphics, static_cast<int>(CameraTypes::Game));
				player->TransitionIdle();
				tunnel_alpha = 0.0f;
				during_clear = false;
			}
		}
	}

	//--------------------<敵の管理クラスの更新処理>--------------------//

	const auto enemyManager = mWaveManager.fGetEnemyManager();
	enemyManager->fSetPlayerPosition(player->GetPosition());
	mBulletManager.fUpdate(elapsed_time);

	// ↓↓↓↓↓↓↓↓↓プレイヤーの更新はこのした↓↓↓↓↓
	BaseEnemy* enemy = enemyManager->fGetNearestEnemyPosition();

	Camera* c = cameraManager->GetCurrentCamera();

	if (player->GetIsAlive() == false)	SceneManager::scene_switching(new SceneLoading(new SceneTitle()), DISSOLVE_TYPE::DOT, 2.0f);


	// 敵とのあたり判定(当たったらコンボ加算)
	if (player->GetIsPlayerAttack())
	{
		if (player->GetIsAwakening())
		{
			player->AddCombo(enemyManager->fCalcPlayerAttackVsEnemies(
				player->GetSwordCapsuleParam(0).start,
				player->GetSwordCapsuleParam(0).end,
				player->GetSwordCapsuleParam(0).rasius,
				player->GetPlayerPower()));

			player->AddCombo(enemyManager->fCalcPlayerAttackVsEnemies(
				player->GetSwordCapsuleParam(1).start,
				player->GetSwordCapsuleParam(1).end,
				player->GetSwordCapsuleParam(1).rasius,
				player->GetPlayerPower()));
		}
		else
		{
			player->AddCombo(enemyManager->fCalcPlayerAttackVsEnemies(
				player->GetSwordCapsuleParam(0).start,
				player->GetSwordCapsuleParam(0).end,
				player->GetSwordCapsuleParam(0).rasius,
				player->GetPlayerPower()));
		}
	}

	enemyManager->fCalcEnemiesAttackVsPlayer(player->GetBodyCapsuleParam().start,
		player->GetBodyCapsuleParam().end,
		player->GetBodyCapsuleParam().rasius, player->GetDamagedFunc());

	//プレイヤーがジャスト回避した時の範囲スタンの当たり判定
	enemyManager->fCalcPlayerStunVsEnemyBody(player->GetPosition(), player->GetStunRadius());

	//弾とプレイヤーの当たり判定
	mBulletManager.fCalcBulletsVsPlayer(player->GetBodyCapsuleParam().start,
		player->GetBodyCapsuleParam().end,
		player->GetBodyCapsuleParam().rasius, player->GetDamagedFunc());

	// camera
	//camera->Update(elapsed_time,player.get());
	cameraManager->Update(elapsed_time);

	player->SetCameraDirection(c->GetForward(), c->GetRight());
	player->UpdateTutorial(elapsed_time, graphics, sky_dome.get(), enemyManager->fGetEnemies());
	//player->UpdateTutorial(elapsed_time, graphics, sky_dome.get(), enemyManager->fGetEnemies());
	player->lockon_post_effect(elapsed_time, [=](float scope, float alpha) { post_effect->lockon_post_effect(scope, alpha); },
		[=]() { post_effect->clear_post_effect(); });
	player->SetCameraPosition(c->get_eye());
	player->SetTarget(enemy);
	player->SetCameraTarget(c->get_target());
	if (player->GetStartDashEffect()) post_effect->dash_post_effect(graphics.get_dc().Get(), player->GetPosition());
	if (player->GetEndDashEffect())
	{
		post_effect->clear_post_effect();
		player->SetEndDashEffect(false);
	}


	enemy_hp_gauge->update(graphics, elapsed_time);
	enemy_hp_gauge->focus(player->GetPlayerTargetEnemy(), player->GetEnemyLockOn());

	reticle->update(graphics, elapsed_time);
	reticle->focus(player->GetPlayerTargetEnemy(), player->GetEnemyLockOn());
	{
		static DirectX::XMFLOAT2 pos{ 950.0f, 90.0f };
		static DirectX::XMFLOAT2 offset{ 50.0f, 0 };
		static DirectX::XMFLOAT2 scale{ 0.5f,0.5f };
		static DirectX::XMFLOAT4 color{ 1,1,1,1 };
		static int value{};
		static bool is_display_imgui = false;
		imgui_menu_bar("UI", "wave", is_display_imgui);
#ifdef USE_IMGUI
		if (is_display_imgui)
		{
			ImGui::Begin("wave");
			ImGui::DragFloat2("pos", &pos.x);
			ImGui::DragFloat2("offset", &offset.x);
			ImGui::DragFloat2("scale", &scale.x, 0.01f);
			ImGui::DragInt("value", &value);
			ImGui::ColorEdit4("color", &color.x);
			ImGui::End();
		}
#endif
		wave->set_positoin(pos); wave->set_offset(offset); wave->set_scale(scale); wave->set_value(value); wave->set_color(color);
		wave->update(graphics, elapsed_time);
	}


	//camera->update_with_quaternion(elapsed_time);
	// shadow_map
	shadow_map->debug_imgui();

	//\effect_manager->update(elapsed_time);

	// effect demo
#ifdef USE_IMGUI
	{
		ImGui::Begin("effect");
		if (ImGui::Button("play_effect"))
		{
			test_effect->play(effect_manager->get_effekseer_manager(), { 0,1,0 }, 3);
		}
		if (ImGui::Button("stop_effect"))
		{
			test_effect->stop(effect_manager->get_effekseer_manager());
		}
		{
			static DirectX::XMFLOAT3 pos{};
			ImGui::DragFloat3("position", &pos.x, 0.1f);
			test_effect->set_position(effect_manager->get_effekseer_manager(), pos);
			static DirectX::XMFLOAT3 scale{ 1,1,1 };
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
					//if (ImGui::Button("play bgm")) { audio_manager->play_bgm(BGM_INDEX::ENDING); is_open_button = true; }
					if (ImGui::Button("play TITLE bgm")) { audio_manager->play_bgm(BGM_INDEX::TITLE); is_open_button = true; }
				}
				//else
				{
					//if (ImGui::Button("stop bgm")) { audio_manager->stop_bgm(BGM_INDEX::ENDING); is_open_button = false; }
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
	//audio_manager->set_volume_bgm(BGM_INDEX::ENDING, bgm_volume);
	audio_manager->set_volume_bgm(BGM_INDEX::TITLE, bgm_volume);
	audio_manager->set_volume_se(SE_INDEX::DECISION, se_volume);


	//****************************************************************
	//
	// オブジェクトの削除処理はこの下でやるルール
	//
	//****************************************************************
	enemyManager->fDeleteEnemies();

}
#define OFF_SCREEN_RENDERING
#define SHADOW_MAP

void TutorialScene::render(GraphicsPipeline& graphics, float elapsed_time)
{
#ifdef OFF_SCREEN_RENDERING
	post_effect->begin(graphics.get_dc().Get());
#endif // DEBUG

	// シャドウマップのセット
#ifdef SHADOW_MAP
	shadow_map->set_shadowmap(graphics);
#endif // SHADOW_MAP
	// カメラのビュー行列計算
	//camera->calc_view_projection(graphics);
	cameraManager->CalcViewProjection(graphics);

	/*-----!!!ここから上にオブジェクトの描画はしないで!!!!-----*/
	{
		graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::DEFAULT);
#ifdef USE_IMGUI
		ImGui::Begin("sky");
		ImGui::DragFloat("dimension", &dimension, 0.01f);
		ImGui::End();
#endif
		sky_dome->Render(graphics, elapsed_time);


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
	BulletManager& mBulletManager = BulletManager::Instance();

	//--------------------<敵の管理クラスの描画処理>--------------------//
	mWaveManager.fGetEnemyManager()->fRender(graphics);
	player->Render(graphics, elapsed_time);
	mBulletManager.fRender(graphics);

	//--------<ui>--------//
	graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
	// enemy_hp_gauge
	enemy_hp_gauge->render(graphics.get_dc().Get());
	// reticle
	reticle->render(graphics.get_dc().Get());
	// wave
	//wave->render(graphics.get_dc().Get());
	Camera* c = cameraManager->GetCurrentCamera();
	const DirectX::XMFLOAT2 p_pos = { player->GetPosition().x,player->GetPosition().z };
	const DirectX::XMFLOAT2 c_forward = { c->GetForward().x,c->GetForward().z };
	minimap->render(graphics, p_pos, c_forward, mWaveManager.fGetEnemyManager()->fGetEnemies());

	effect_manager->render(Camera::get_keep_view(), Camera::get_keep_projection());
	graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::WIREFRAME_CULL_BACK, DEPTH_STENCIL::DEON_DWON);
	debug_figure->render_all_figures(graphics.get_dc().Get());


	// クリア中のトンネル
	if (during_clear)
	{
		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
		tunnel->render(graphics.get_dc().Get(), elapsed_time, tunnel_alpha, [&]() {
			player->Render(graphics, elapsed_time);
			});
	}

	TutorialRender(graphics, elapsed_time);

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
	post_effect->blit(graphics.get_dc().Get());
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

	graphics.set_pipeline_preset(BLEND_STATE::ALPHA, RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
	mWaveManager.render(graphics.get_dc().Get(), elapsed_time);
	if (option->get_validity()) { option->render(graphics, elapsed_time); }

}

void TutorialScene::register_shadowmap(GraphicsPipeline& graphics, float elapsed_time)
{
#ifdef SHADOW_MAP
	Camera* c = cameraManager->GetCurrentCamera();
	//--シャドウマップの生成--//
	shadow_map->activate_shadowmap(graphics, c->get_light_direction());







	//--元のビューポートに戻す--//
	shadow_map->deactivate_shadowmap(graphics);
#endif // SHADOW_MAP

}

void TutorialScene::TutorialUpdate(GraphicsPipeline& graphics, float elapsed_time)
{
#ifdef USE_IMGUI
	{
		if (is_end_text)
		{
			//ImGui::Begin("tutorial_state");
			//static int state = 1;
			//ImGui::SliderInt("tutorial_state", &state, 1, 7);
			//tutorial_state = static_cast<TutorialState>(state);
			//ImGui::End();
		}
	}
#endif
	//プレイヤー側の今のチュートリアルが終わってるときかつis_nextがtrueの時
	if (player->GetNextTutorial() && is_end_text)
	{
		//チェックボックスのディゾルブを開始
		check_mark_parm.is_threshold = true;
	}
	if (check_mark_parm.is_threshold)
	{
		//0よりも大きかったら引いていく
		if (check_mark_parm.threshold > 0)check_mark_parm.threshold -= 1.0f * elapsed_time;
		else
		{
			is_next = true;
		}
	}

	switch (tutorial_state)
	{
	case TutorialScene::TutorialState::MoveTutorial:
		player->ChangeTutorialState(static_cast<int>(tutorial_state));
		tutorial_check_text = L"Lスティックで移動させる";
		if (is_next)
		{
			Judea_timer += 1.0f * elapsed_time;
			if (Judea_timer > 1.0f)
			{
				//次のステートに設定
				tutorial_state = TutorialState::AvoidanceTutorial;
				//次に進むフラグの初期化
				is_next = false;
				//プレイヤーの次に進むフラグを初期化
				player->FalseNextTutorial();
				//猶予時間を初期化
				Judea_timer = 0;
				//ディゾルブ時間を初期化
				check_mark_parm.threshold = 1.0f;
				//ディゾルブしていいかどうかのフラグを初期化
				check_mark_parm.is_threshold = false;
			}
		}
		break;
	case TutorialScene::TutorialState::AvoidanceTutorial:
		player->ChangeTutorialState(static_cast<int>(tutorial_state));
		tutorial_check_text = L"RB,RT,ボタンを押して回避する";
		if (is_next)
		{
			Judea_timer += 1.0f * elapsed_time;
			if (Judea_timer > 1.0f)
			{
				//次のステートに設定
				tutorial_state = TutorialState::LockOnTutorial;
				//次に進むフラグの初期化
				is_next = false;
				//プレイヤーの次に進むフラグを初期化
				player->FalseNextTutorial();
				//猶予時間を初期化
				Judea_timer = 0;
				//ディゾルブ時間を初期化
				check_mark_parm.threshold = 1.0f;
				//ディゾルブしていいかどうかのフラグを初期化
				check_mark_parm.is_threshold = false;
			}
		}

		break;
	case TutorialScene::TutorialState::LockOnTutorial:
		player->ChangeTutorialState(static_cast<int>(tutorial_state));
		tutorial_check_text = L"LTボタンでロックオンする";
		if (is_next)
		{
			Judea_timer += 1.0f * elapsed_time;
			if (Judea_timer > 1.0f)
			{
				//次のステートに設定
				tutorial_state = TutorialState::AttackTutorial;
				//次に進むフラグの初期化
				is_next = false;
				//プレイヤーの次に進むフラグを初期化
				player->FalseNextTutorial();
				//猶予時間を初期化
				Judea_timer = 0;
				//ディゾルブ時間を初期化
				check_mark_parm.threshold = 1.0f;
				//ディゾルブしていいかどうかのフラグを初期化
				check_mark_parm.is_threshold = false;
			}
		}

		break;
	case TutorialScene::TutorialState::AttackTutorial:
		player->ChangeTutorialState(static_cast<int>(tutorial_state));
		tutorial_check_text = L"Bボタンを押して攻撃";
		if (is_next)
		{
			Judea_timer += 1.0f * elapsed_time;
			if (Judea_timer > 1.0f)
			{
				//次のステートに設定
				tutorial_state = TutorialState::BehindAvoidanceTutorial;
				//次に進むフラグの初期化
				is_next = false;
				//プレイヤーの次に進むフラグを初期化
				player->FalseNextTutorial();
				//猶予時間を初期化
				Judea_timer = 0;
				//ディゾルブ時間を初期化
				check_mark_parm.threshold = 1.0f;
				//ディゾルブしていいかどうかのフラグを初期化
				check_mark_parm.is_threshold = false;
			}
		}

		break;
	case TutorialScene::TutorialState::BehindAvoidanceTutorial:
		player->ChangeTutorialState(static_cast<int>(tutorial_state));
		tutorial_check_text = L"回り込み回避をする";
		if (is_next)
		{
			Judea_timer += 1.0f * elapsed_time;
			if (Judea_timer > 1.0f)
			{
				//次のステートに設定
				tutorial_state = TutorialState::ChainAttackTutorial;
				//次に進むフラグの初期化
				is_next = false;
				//プレイヤーの次に進むフラグを初期化
				player->FalseNextTutorial();
				//猶予時間を初期化
				Judea_timer = 0;
				//ディゾルブ時間を初期化
				check_mark_parm.threshold = 1.0f;
				//ディゾルブしていいかどうかのフラグを初期化
				check_mark_parm.is_threshold = false;
			}
		}

		break;
	case TutorialScene::TutorialState::ChainAttackTutorial:
		player->ChangeTutorialState(static_cast<int>(tutorial_state));
		tutorial_check_text = L"LBボタンを長押ししてスタンしている敵をロックオン";
		if (is_next)
		{
			Judea_timer += 1.0f * elapsed_time;
			if (Judea_timer > 1.0f)
			{
				//次のステートに設定
				tutorial_state = TutorialState::AwaikingTutorial;
				//次に進むフラグの初期化
				is_next = false;
				//プレイヤーの次に進むフラグを初期化
				player->FalseNextTutorial();
				//猶予時間を初期化
				Judea_timer = 0;
				//ディゾルブ時間を初期化
				check_mark_parm.threshold = 1.0f;
				//ディゾルブしていいかどうかのフラグを初期化
				check_mark_parm.is_threshold = false;
				for (int i = 0; i < 50; ++i)
				{
					player->AddCombo(2);
				}
			}
		}

		break;
	case TutorialScene::TutorialState::AwaikingTutorial:
		player->ChangeTutorialState(static_cast<int>(tutorial_state));
		tutorial_check_text = L"Aボタンを押して覚醒";
		if (is_next)
		{
			Judea_timer += 1.0f * elapsed_time;
			if (Judea_timer > 1.0f)
			{
				//次のステートに設定
				tutorial_state = TutorialState::FreePractice;
				//次に進むフラグの初期化
				is_next = false;
				//プレイヤーの次に進むフラグを初期化
				player->FalseNextTutorial();
				//猶予時間を初期化
				Judea_timer = 0;
				//ディゾルブ時間を初期化
				check_mark_parm.threshold = 1.0f;
				//ディゾルブしていいかどうかのフラグを初期化
				check_mark_parm.is_threshold = false;
			}
		}
		break;
	case TutorialScene::TutorialState::FreePractice:
		tutorial_check_text = L"自由に練習する";

		break;
	default:
		break;
	}

}

void TutorialScene::TutorialRender(GraphicsPipeline& graphics, float elapsed_time)
{
	graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
	if (tutorial_state != TutorialState::FreePractice)
	{
		//ここで-1してるのは1から始まっているから
		if (StepString(elapsed_time, tutorial_text_element[static_cast<int>(tutorial_state) - 1])) is_end_text = true;
		else is_end_text = false;
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
		r_font_render("text", tutorial_text_element[static_cast<int>(tutorial_state) - 1]);
		fonts->yu_gothic->End(graphics.get_dc().Get());


	}
#ifdef USE_IMGUI
	ImGui::Begin("check_mark_parm");
	if (ImGui::TreeNode("check_mark_parm"))
	{
		ImGui::DragFloat2("pos", &check_mark_parm.pos.x,0.1f);
		ImGui::DragFloat2("scale", &check_mark_parm.scale.x, 0.1f);
		ImGui::DragFloat("threshold", &check_mark_parm.threshold, 0.01f);
		ImGui::Checkbox("is_threshold", &check_mark_parm.is_threshold);
		ImGui::TreePop();
	}
	ImGui::End();
	ImGui::Begin("tutorial_check_text_parm");
	if (ImGui::TreeNode("tutorial_check_text_parm"))
	{
		ImGui::DragFloat2("pos", &tutorial_check_text_parm.position.x,0.1f);
		ImGui::DragFloat2("scale", &tutorial_check_text_parm.scale.x, 0.1f);
		ImGui::TreePop();
	}
	ImGui::End();
#endif // USE_IMGUI



	check_box->begin(graphics.get_dc().Get());
	check_box->render(graphics.get_dc().Get(), check_mark_parm.pos, check_mark_parm.scale);
	check_box->end(graphics.get_dc().Get());

	fonts->yu_gothic->Begin(graphics.get_dc().Get());
	fonts->yu_gothic->Draw(tutorial_check_text, tutorial_check_text_parm.position, tutorial_check_text_parm.scale, tutorial_check_text_parm.color, tutorial_check_text_parm.angle, TEXT_ALIGN::UPPER_LEFT);
	fonts->yu_gothic->End(graphics.get_dc().Get());

	check_mark->begin(graphics.get_dc().Get());
	check_mark->render(graphics.get_dc().Get(), check_mark_parm.pos, check_mark_parm.scale, check_mark_parm.threshold);
	check_mark->end(graphics.get_dc().Get());


}

bool TutorialScene::StepString(float elapsed_time, StepFontElement& step_font_element, bool loop)
{
	step_font_element.timer += elapsed_time * step_font_element.speed;
	step_font_element.step = static_cast<int>(step_font_element.timer);
	size_t size = step_font_element.tutorial_text.size();
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
			step_font_element.s += step_font_element.tutorial_text[step_font_element.index];
			step_font_element.step = 1;
			step_font_element.timer = 1.0f;
		}
		++step_font_element.index;
	}

	return false;
}
