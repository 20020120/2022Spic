#include "skinned_mesh.hlsli"
#include "constants.hlsli"
#include "shading_functions.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

#define BASE_COLOR 0
#define NORMAL     1
#define METAL      2
#define ROUGHNESS  3
#define AO         4
#define Emissive   5

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[6] : register(t0);

#define S(r,v,a) smoothstep(a/iResolution.y,0.,abs(v-(r)))

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color_map = texture_maps[BASE_COLOR].Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // マテリアルのアルファ値が0.1未満ならそのピクセルを破棄する
    clip(color_map.a < 0.1f ? -1 : 1);

    return color_map;
}