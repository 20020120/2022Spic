#include "sprite_dissolve.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    vout.pos = position;
    vout.color = color;
    vout.texcoord = texcoord;

    return vout;
}