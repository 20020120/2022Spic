//==============================================================================================================================
//   https://yttm-work.jp/directx/directx_0014.html
//   https://araramistudio.jimdo.com/2017/10/18/%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9F%E3%83%B3%E3%82%B0-directx-11%E3%81%A7%E3%83%86%E3%82%AF%E3%82%B9%E3%83%81%E3%83%A3%E3%83%9E%E3%83%83%E3%83%94%E3%83%B3%E3%82%B0/
//         �s�N�Z���V�F�[�_
// �� �v���p�e�B�Őݒ肷�邱�ƂŃr���h����VS���R���p�C�����Ă����
//    .cso �̓R���p�C�����ꂽ�V�F�[�_�t�@�C��
//
// sprite�N���X��render�����o�֐��̐F�w��̈������@�\����悤�ɁA�s�N�Z���V�F�[�_�[�ɕύX��������
// �F���̕ύX��A �A���t�@�l��ω������t�F�[�h�A�E�g���̌��ʂ�\���ł���悤�ɂ���
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