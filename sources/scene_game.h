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

#include"EnemyManager.h"
#include"Player.h"

class SceneGame : public Scene, public PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    SceneGame() {}
    ~SceneGame() override {}
    //--------< �֐� >--------//
    //������
    void initialize(GraphicsPipeline& graphics) override;
    //�I����
    void uninitialize() override;
    //�X�V����
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    //�`�揈��
    void render(GraphicsPipeline& graphics, float elapsed_time) override;
    //�V���h�E�}�b�v�o�^
    void register_shadowmap(GraphicsPipeline& graphics, float elapsed_time) override;
private:
    //--------< �ϐ� >--------//
    // camera
    std::unique_ptr<Camera> camera;
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
    // �X�L�����b�V���̎���
    std::shared_ptr<SkinnedMesh> sky_dome;
    std::shared_ptr<SkinnedMesh> test;
    std::unique_ptr<Effect> test_effect;
    //****************************************************************
    //
    // �X�s�b�N�J�n��ǉ������ϐ�
    //
    //****************************************************************
    EnemyManager mEnemyManager{};
    std::unique_ptr<Player> player{ nullptr };
};