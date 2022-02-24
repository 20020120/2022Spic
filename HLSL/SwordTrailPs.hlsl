#include "constants.hlsli"
#include"SwordTrail.hlsli"

SamplerState gSampler : register(s0);
Texture2D<float4> gTexNormal : register(t0);
Texture2D<float4> gTexColor : register(t1);

cbuffer Data :register(b0)
{
	float yugamiThreshold;
	float3 Pad0;
}

float4 main(PsInput PsInput_) : SV_TARGET
{
	float4 normalMap = gTexNormal.Sample(gSampler, PsInput_.Texcoord);
	float2 yugamiUV = PsInput_.Texcoord + normalMap.rg;
	float4 yugamicolor = gTexColor.Sample(gSampler, yugamiUV);
	float4 baseColor =   gTexColor.Sample(gSampler, PsInput_.Texcoord);
	float4 finalColor = lerp(baseColor, yugamicolor, yugamiThreshold);

	return finalColor;
}