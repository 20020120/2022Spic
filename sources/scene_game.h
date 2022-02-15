#pragma once

#include "Scene.h"
#include "camera.h"
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

class SceneGame : public Scene, public PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    SceneGame() {}
    ~SceneGame() override {}
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
    //--------< 変数 >--------//
    float gravity = 0;
    // camera
    std::unique_ptr<Camera> camera;
    // shadowmap
    std::unique_ptr<ShadowMap> shadow_map;
    // post effect
    std::unique_ptr<PostEffect> post_effect;
    // fog
    std::unique_ptr<FullScreenQuad> fog_post_effect;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> fog_ps;
    struct MistConstants
    {
        DirectX::XMFLOAT4 fog_color{ 1,1,1,1 };
        DirectX::XMFLOAT4 fog_range{ 0.1f, 30.0f, 0,0 }; // x:near y:far
    };
    std::unique_ptr<Constants<MistConstants>> mist_constants;
    // bloom
    std::unique_ptr<Bloom> bloom_effect;
    struct BloomConstants
    {
        float bloom_extraction_threshold = 0.800f;
        float blur_convolution_intensity = 0.500f;

        float bloom_options[2];
    };
    std::unique_ptr<Constants<BloomConstants>> bloom_constants;
    // PointLights
    std::unique_ptr<PointLights> point_lights;
    // スキンメッシュの実体
    std::shared_ptr<SkinnedMesh> skinned_meshes[5];

    std::unique_ptr<Effect> test_effect;
    std::unique_ptr<Effect> test_effect2;

    struct Player
    {
        DirectX::XMFLOAT3 position{};
        DirectX::XMFLOAT3 scale{};
        DirectX::XMFLOAT3 angle{};
        int anim_index{ 0 };
    } player;
    bool display_player_imgui = false;

    struct Stage
    {
        DirectX::XMFLOAT3 position{};
        DirectX::XMFLOAT3 scale{};
        DirectX::XMFLOAT3 angle{};
        DirectX::XMFLOAT4X4 coordinate{};
    } stage;

    bool accumulate_husk_particles{ true };
    bool integrate_particles{ false };
    std::unique_ptr<HuskParticles> particles;
};