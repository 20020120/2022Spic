#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>
#include "graphics_pipeline.h"

class Effect
{
public:
    Effect(GraphicsPipeline& graphics, Effekseer::Manager* effekseer_manager, const char* filename);
    ~Effect();
public:
    // 再生
    void play(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& position, float scale = 1.0f);
    // 停止
    void stop(Effekseer::Manager* effekseer_manager);
    // 座標設定
    void set_position(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& position);
    // スケール設定
    void set_scale(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& scale);
    // スピード設定
    void set_speed(Effekseer::Manager* effekseer_manager, float speed);
    // 行列をセット
    void set_rotationY(Effekseer::Manager* effekseer_manager, const float angle);
    // クオータニオン
    void set_quaternion(Effekseer::Manager* effekseer_manager, DirectX::XMFLOAT4 orientation);
private:
    Effekseer::Effect* effekseer_effect{ nullptr };
    Effekseer::Handle effekseer_handle{ -1 };
};