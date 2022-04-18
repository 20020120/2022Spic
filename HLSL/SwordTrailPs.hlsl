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
	//float4 normalMap = gTexNormal.Sample(gSampler, PsInput_.Texcoord);
	//float2 yugamiUV = PsInput_.Texcoord + normalMap.rg;
	//float4 yugamicolor = gTexColor.Sample(gSampler, yugamiUV);
	//float4 baseColor =   gTexColor.Sample(gSampler, PsInput_.Texcoord);
	//float4 finalColor = lerp(baseColor, yugamicolor, yugamiThreshold);

	
	float4 outPutColor = { 1.0f, 0.2f, 0.2f, 1.0f };
	// UV‚Ì’l‚É‰‚¶‚ÄF‚ğ’²®‚·‚é
	//outPutColor = lerp(outPutColor, 0.0f, pow(PsInput_.Texcoord.y, 3));
	//outPutColor = lerp(outPutColor, 0.0f, pow(5, 10 *  - 10));
	float c = 1.0f - PsInput_.Texcoord.y;
	c = lerp(0.8f,c, step(0.8f, c));
	if(c>0.85f)
	{
		float a = step(0.0f, c);
	    //          [F‚Ì”Z‚³]   [”Z‚¢•”•ª‚Ì•]
		float b = (pow(5, 10 * c - 9) * a);
		outPutColor = outPutColor * b;
	}
	else
	{
		outPutColor = lerp(0.0f, 0.25f, 1.0f - PsInput_.Texcoord.y);
	}
	outPutColor.xyz *= 15.0f;
	
	return outPutColor;
}