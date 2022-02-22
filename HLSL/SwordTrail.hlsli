struct VsInput
{
	float3 mWorldPosition : POSITION;
	float2 mTexCoord : TEXCOORD;
};

struct VsOutput
{
	float4 Position : SV_POSITION;
	float2 Texcoord : TEXCOORD;
};
#define PsInput VsOutput