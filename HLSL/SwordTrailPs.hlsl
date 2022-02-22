#include "constants.hlsli"
#include"SwordTrail.hlsli"

SamplerState gSampler : register(s0);
Texture2D<float4> gTexNormal : register(t0);

float4 main(PsInput PsInput_) : SV_TARGET
{
	float4 normalMap = gTexNormal.Sample(gSampler, PsInput_.Texcoord);
	float4 color = { 1.0f, 0.0f, 0.0f, 1.0f };

	float p = dot(normalMap.xyz, -light_direction.xyz);
	p = p * 0.5f + 0.5f;
	float ambient = 0.0f; // ŠÂ‹«Œõ(ƒ‚ƒfƒ‹‘S‘Ì‚ð–¾‚é‚­‚·‚é)
    p = p * p + ambient;


	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}