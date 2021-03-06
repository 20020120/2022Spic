#pragma once

#include "UI.h"
#include "BaseEnemy.h"

class EnemyHpGauge : public UI
{
public:
    //--------<constructor/destructor>--------//
    EnemyHpGauge(GraphicsPipeline& graphics);
    ~EnemyHpGauge() override {}
    //--------< 関数 >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void render(ID3D11DeviceContext* dc) override;
    //--------<getter/setter>--------//
    void focus(const BaseEnemy* target_enemy, bool lockon);
private:
    static constexpr float BASE_RATE_LENGTH = 25.0f; // カメラと敵の距離に応じて位置を上下する割合の基準
    //--------< 変数 >--------//
    std::unique_ptr<SpriteBatch> frame{ nullptr };
    std::unique_ptr<SpriteBatch> back{ nullptr };
    std::unique_ptr<SpriteBatch> body{ nullptr };

    Element gauge;
    DirectX::XMFLOAT3 focus_position{};
    float hp_percent = 1.0f;
    DirectX::XMFLOAT2 offset{};
    bool animation = false;
    float length_player_to_enemy = 0;
};

class BossHpGauge : public UI
{
public:
    //--------<constructor/destructor>--------//
    BossHpGauge(GraphicsPipeline& graphics);
    ~BossHpGauge() override {}
    //--------< 関数 >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void render(ID3D11DeviceContext* dc) override;
    //--------<getter/setter>--------//
    static void set_hp_percent(float arg) { hp_percent = arg; }
    void set_animation(bool arg) { animation = arg; }
private:
    //--------< 変数 >--------//
    std::unique_ptr<SpriteBatch> frame{ nullptr };
    std::unique_ptr<SpriteBatch> back{ nullptr };
    std::unique_ptr<SpriteBatch> body{ nullptr };

    Element gauge;
    static float hp_percent;
    bool animation = false;
};