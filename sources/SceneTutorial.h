#pragma once
#include "Scene.h"
#include "camera.h"
#include "CameraManager.h"
#include "shadow_map.h"
#include "post_effect.h"
#include "bloom.h"
#include "practical_entities.h"
#include "sprite_batch.h"
#include "skinned_mesh.h"
#include "effect.h"
#include "point_light.h"
#include "husk_particles.h"
#include "constants.h"
#include "imgui_include.h"
#include"WaveManager.h"
#include "mini_map.h"
#include"EnemyManager.h"
#include"Player.h"
#include"SkyDome.h"
#include "enemy_hp_gauge.h"
#include "reticle.h"
#include "counter.h"
#include "scene_option.h"
#include "BulletManager.h"
#include "tunnel.h"
#include "sprite_dissolve.h"

class TutorialScene : public Scene, public PracticalEntities
{
public:
    TutorialScene() {}
    ~TutorialScene() override {}
    //初期化
    void initialize(GraphicsPipeline& graphics) override;
    //終了化
    void uninitialize() override;
    //更新処理
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    //描画処理
    void render(GraphicsPipeline& graphics, float elapsed_time) override;
    //シャドウマップ登録
    void register_shadowmap(GraphicsPipeline& graphics, float elapsed_time) override;
private:
    void TutorialUpdate(GraphicsPipeline& graphics, float elapsed_time);
    //チュートリアルの文字とかを描画する
    void TutorialRender(GraphicsPipeline& graphics, float elapsed_time);
private:
    //--------< 変数 >--------//
    // camera
    //std::unique_ptr<Camera> camera;
    std::unique_ptr<CameraManager> cameraManager;
public:
    enum class CameraTypes
    {
        Game,
        Tunnel,
    };

    //1が最初で大きくなっていくようにする
    enum class TutorialState
    {
        //移動
        MoveTutorial = 1,
        //回避(通常)
        AvoidanceTutorial,
        //ロックオン
        LockOnTutorial,
        //攻撃
        AttackTutorial,
        //回り込み回避
        BehindAvoidanceTutorial,
        //チェイン攻撃
        ChainAttackTutorial,
        //覚醒
        AwaikingTutorial
    };

    TutorialState tutorial_state{ TutorialState::MoveTutorial };
private:
    // shadowmap
    std::unique_ptr<ShadowMap> shadow_map;
    // post effect
    std::unique_ptr<PostEffect> post_effect;
    // bloom
    std::unique_ptr<Bloom> bloom_effect;
    struct BloomConstants
    {
        float bloom_extraction_threshold = 0.800f;
        float blur_convolution_intensity = 0.500f;

        float bloom_options[2];
    };
    std::unique_ptr<Constants<BloomConstants>> bloom_constants;
    //// スキンメッシュの実体
    //std::shared_ptr<SkinnedMesh> sky_dome;
    std::unique_ptr<Effect> test_effect;
    //****************************************************************
    //
    // スピック開始後追加した変数
    //
    //****************************************************************
    //EnemyManager mEnemyManager{};
    WaveManager mWaveManager{};
    std::unique_ptr<Player> player{ nullptr };
    std::unique_ptr<SkyDome> sky_dome{ nullptr };
    //skydome
    float dimension{ 0.1f };
    // enemy_hp_gauge
    std::unique_ptr<EnemyHpGauge> enemy_hp_gauge{ nullptr };
    // reticle
    std::unique_ptr<Reticle> reticle{ nullptr };
    // counter
    std::unique_ptr<Counter> wave{ nullptr };
    // option
    std::unique_ptr<Option> option{ nullptr };
    std::unique_ptr<MiniMap> minimap{ nullptr };

    bool is_next{ false };
    //次のチュートリアルに行くまでの時間
    float Judea_timer{ 0 };
    // クリア演出
    bool during_clear = false;
    float tunnel_alpha = 0.0f;
    std::unique_ptr<Tunnel> tunnel{ nullptr };
private:
    //チュートリアルのテキスト
    //説明文用の構造体
    struct StepFontElement
    {
        std::wstring s = L"";
        DirectX::XMFLOAT2 position{};
        DirectX::XMFLOAT2 scale{ 0.7f, 0.7f };
        DirectX::XMFLOAT4 color{ 1.0f,1.0f,1.0f,1.0f };
        float angle{};
        DirectX::XMFLOAT2 length{};
        //説明文のテキスト
        std::wstring tutorial_text=L"UI";

        // step string
        float timer = 0;
        int step = 0;
        int index = 0;
        //スピード
        float speed{ 25.0f };
    };
    StepFontElement tutorial_text_element[7];

    bool StepString(float elapsed_time, StepFontElement& step_font_element, bool loop = false);
    bool is_end_text{ false };

    struct TutorialCheckText
    {
        DirectX::XMFLOAT2 position{ 103.0f,79.8f };
        DirectX::XMFLOAT2 scale{ 0.7f,0.7f };
        float angle;
        DirectX::XMFLOAT4 color{ 1.0f,1.0f,1.0f,1.0f };
    };
    TutorialCheckText tutorial_check_text_parm;
    //ｔチェックボックスのテキスト
    std::wstring tutorial_check_text = L"UI";
private:
    //チェックボックスの画像
    struct CheckMarkParm
    {
        DirectX::XMFLOAT2 pos{ 32.0f,76.0f };
        DirectX::XMFLOAT2 scale{ 0.2f,0.2f };
        float threshold{1.0f};
        bool is_threshold{ false };
    };
    CheckMarkParm check_mark_parm;
    std::unique_ptr<SpriteDissolve> check_mark{ nullptr };
    std::unique_ptr<SpriteBatch> check_box{ nullptr };


};