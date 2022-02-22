#include"SwordTrail.hlsli"
#include "Constants.hlsli"

VsOutput main( VsInput VsInput_ )
{
	VsOutput vs_output;
	vs_output.Texcoord = VsInput_.mTexCoord;
	//float4x4 world =
	//{
	//	1.0f, 0.0f, 0.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f, 0.0f,
	//	0.0, 0.0, 1.0f, 0.0f,
	//	VsInput_.mWorldPosition.xyz, 1.0f
	//};
	vs_output.Position = mul(float4(VsInput_.mWorldPosition,1.0f), view_projection);
	return vs_output;
}