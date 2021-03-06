#pragma once

#include "graphics_pipeline.h"
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

class EffectManager
{
public:
    EffectManager() {}
    ~EffectManager() {}
public:
    //初期化処理
    void initialize(GraphicsPipeline& graphics);
    //終了処理
    void finalize();
    //更新処理
    void update(float elapsed_time);
    //描画処理
    void render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
    //--------<getter/setter>--------//
    Effekseer::Manager* get_effekseer_manager() { return effekseer_manager; }
private:
    //--------< 変数 >--------//
    Effekseer::Manager* effekseer_manager{ nullptr };
    EffekseerRenderer::Renderer* effekseer_renderer{ nullptr };
};