//==============================================================================================================================
//   https://yttm-work.jp/directx/directx_0014.html
//   https://araramistudio.jimdo.com/2017/10/18/%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0-directx-11%E3%81%A7%E3%83%86%E3%82%AF%E3%82%B9%E3%83%81%E3%83%A3%E3%83%9E%E3%83%83%E3%83%94%E3%83%B3%E3%82%B0/
//         ピクセルシェーダ
// ※ プロパティで設定することでビルド時にVSがコンパイルしてくれる
//    .cso はコンパイルされたシェーダファイル
//
// spriteクラスのrenderメンバ関数の色指定の引数が機能するように、ピクセルシェーダーに変更を加える
// 色調の変更や、 アルファ値を変化させフェードアウト等の効果を表現できるようにする
//
//==============================================================================================================================
#include "sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
    float alpha = color.a;
#if 0
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif
    return float4(color.rgb, alpha) * pin.color;
}