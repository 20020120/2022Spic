#include "fullscreen_quad.hlsli"
#include "constants.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    //--------<RGB‚¸‚ç‚µ>--------//
    float4 tex = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = tex.a;
    // r’l‚ð‰¡•ûŒü‚É‚¸‚ç‚·
    float2 sample_point = pin.texcoord;
    sample_point.x += 0.005;
    tex.r = texture_maps[0].Sample(sampler_states[ANISOTROPIC], sample_point).r;

    return float4(tex.rgb, alpha);
}