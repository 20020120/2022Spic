#pragma once

#include "UI.h"
#include "BaseEnemy.h"

class Reticle : public UI
{
public:
    //--------<constructor/destructor>--------//
    Reticle(GraphicsPipeline& graphics);
    ~Reticle() override {}
    //--------< 関数 >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void update(GraphicsPipeline& graphics, float elapsed_time, float lerp_rate);
    void render(ID3D11DeviceContext* dc) override;
    //--------<getter/setter>--------//
    void focus(const BaseEnemy* target_enemy, bool lockon);
    void SetAvoidanceCharge(bool a) { recharge = a; }
private:
    static constexpr float BASE_RATE_LENGTH = 25.0f; // カメラと敵の距離に応じてUIを小さくする割合の基準
    //--------< 変数 >--------//
    std::unique_ptr<SpriteBatch> reticle{ nullptr };

    Element element;
    DirectX::XMFLOAT3 focus_position{};
    DirectX::XMFLOAT2 offset{};
    bool animation = false;
    float length_player_to_enemy = 0;
    bool recharge{ false };
};