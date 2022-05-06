#pragma once

#include <memory>

#include "scene.h"
#include "sprite_batch.h"
#include "practical_entities.h"
#include "camera.h"
#include "CameraManager.h"
#include "post_effect.h"
#include "bloom.h"
#include "practical_entities.h"
#include "skinned_mesh.h"
#include "effect.h"
#include "point_light.h"
#include "constants.h"
#include "Player.h"
#include "SkyDome.h"
#include "shadow_map.h"
#include "point_light.h"

class SceneTitle : public Scene , public PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    SceneTitle() {}
    ~SceneTitle() override {}
    //--------< 関数 >--------//
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
    //----<3D関連>----//
    std::unique_ptr<CameraManager> cameraManager;
public:
    enum class CameraTypes
    {
        Title,
    };
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
    std::unique_ptr<Player> player{ nullptr };
    std::unique_ptr<SkyDome> sky_dome{ nullptr };
    std::shared_ptr<SkinnedMesh> title_stage_model{ nullptr };

    struct TitleStageParm
    {
        DirectX::XMFLOAT3 pos{};
        DirectX::XMFLOAT3 angle{0,-3.14f,0};
        DirectX::XMFLOAT3 scale{0.06f,0.06f,0.06f};
    };
    TitleStageParm title_stage_parm;

    //--------< 変数 >--------//
    struct Element
    {
        DirectX::XMFLOAT2 position{};
        DirectX::XMFLOAT2 scale{ 1, 1 };
        DirectX::XMFLOAT2 pivot{};
        DirectX::XMFLOAT4 color{ 1,1,1,1 };
        float angle{};
        DirectX::XMFLOAT2 texpos{};
        DirectX::XMFLOAT2 texsize{};
    };
    Element back;
    std::unique_ptr<SpriteBatch> sprite_back{ nullptr };
    Element start;
    Element exit;
    std::unique_ptr<SpriteBatch> sprite_string{ nullptr };
    Element selecter1;
    DirectX::XMFLOAT2 arrival_pos1{};
    Element selecter2;
    DirectX::XMFLOAT2 arrival_pos2{};
    std::unique_ptr<SpriteBatch> sprite_selecter{ nullptr };
    int state = 0;

    static constexpr float SLASHING_MAX = 0.06f;
    float slashing_power = 0.0f;
    float slashing_wait_timer = 0.0f;

    std::unique_ptr<PointLights> point_lights;
    //--------< マルチスレッド >--------//
    static void loading_thread(ID3D11Device* device);
    static bool is_ready;
};