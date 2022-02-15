#include "skinned_mesh.hlsli"
#include "constants.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
texture2D texture_maps[4] : register(t0);

static const float PI = 3.1415926f; // ��

float3 get_normal(float3 normal, float3 tangent, float3 binormal, float2 uv)
{
    float3 bin_space_normal = texture_maps[1].SampleLevel(sampler_states[ANISOTROPIC], uv, 0.0f).xyz;
    bin_space_normal = (bin_space_normal * 2.0f) - 1.0f;

    float3 new_normal = tangent * bin_space_normal.x + binormal * bin_space_normal.y + normal * bin_space_normal.z;

    return new_normal;
}
// �x�b�N�}�����z���v�Z����
float beckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
    return D;
}
// �t���l�����v�Z�BSchlick�ߎ����g�p
float spc_fresnel(float f0, float u)
{
    // from Schlick
    return f0 + (1 - f0) * pow(1 - u, 5);
}
// Cook-Torrance���f���̋��ʔ��˂��v�Z
// <param name="L">�����Ɍ������x�N�g��</param>
// <param name="V">���_�Ɍ������x�N�g��</param>
// <param name="N">�@���x�N�g��</param>
// <param name="metallic">�����x</param>
float cook_torrance_specular(float3 L, float3 V, float3 N, float metallic)
{
    float microfacet = 0.76f;
    // �����x�𐂒����˂̎��̃t���l�����˗��Ƃ��Ĉ���
    // �����x�������قǃt���l�����˂͑傫���Ȃ�
    float f0 = metallic;
    // ���C�g�Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂�
    float3 H = normalize(L + V);
    // �e��x�N�g�����ǂꂭ�炢���Ă��邩����ς𗘗p���ċ��߂�
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    // D�����x�b�N�}�����z��p���Čv�Z����
    float D = beckmann(microfacet, NdotH);
    // F����Schlick�ߎ���p���Čv�Z����
    float F = spc_fresnel(f0, VdotH);
    // G�������߂�
    float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));
    // m�������߂�
    float m = PI * NdotV * NdotH;
    // �����܂ŋ��߂��A�l�𗘗p���āACook-Torrance���f���̋��ʔ��˂����߂�
    return max(F * D * G / m, 0.0);
}
// �t���l�����˂��l�������g�U���˂��v�Z
// </summary>
// <remark>
// ���̊֐��̓t���l�����˂��l�������g�U���˗����v�Z���܂�
// �t���l�����˂́A�������̂̕\�ʂŔ��˂��錻�ۂ̂Ƃ��ŁA���ʔ��˂̋����ɂȂ�܂�
// ����g�U���˂́A�������̂̓����ɓ����āA�����������N�����āA�g�U���Ĕ��˂��Ă������̂��Ƃł�
// �܂�t���l�����˂��ア�Ƃ��ɂ́A�g�U���˂��傫���Ȃ�A�t���l�����˂������Ƃ��́A�g�U���˂��������Ȃ�܂�

// </remark>
// <param name="N">�@��</param>
// <param name="L">�����Ɍ������x�N�g���B���̕����Ƌt�����̃x�N�g���B</param>
// <param name="V">�����Ɍ������x�N�g���B</param>
float calc_diffuse_from_fresnel(float3 N, float3 L, float3 V)
{
    // step-4 �t���l�����˂��l�������g�U���ˌ������߂�
    // �@���ƌ����Ɍ������x�N�g�����ǂꂾ�����Ă��邩����ςŋ��߂�
    float dotNL = saturate(dot(N, L));

    // �@���Ǝ����Ɍ������x�N�g�����ǂꂾ�����Ă��邩����ςŋ��߂�
    float dotNV = saturate(dot(N, V));

    // �@���ƌ����ւ̕����Ɉˑ�����g�U���˗��ƁA�@���Ǝ��_�x�N�g���Ɉˑ�����g�U���˗���
    // ��Z���čŏI�I�Ȋg�U���˗������߂Ă���BPI�ŏ��Z���Ă���̂͐��K�����s������
    return (dotNL * dotNV);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color_map     = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 normal_map    = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    float4 metal_map     = texture_maps[2].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 roughness_map = texture_maps[3].Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // �@�����v�Z
    float3 N = normalize(pin.world_normal.xyz);
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
    float3 normal = get_normal(N, T, B, pin.texcoord);
    // �����x
    float metallic = metal_map.r;
    // ���炩��
    float smooth = roughness_map.r;
    // �����Ɍ������ĐL�т�x�N�g�����v�Z����
    float3 toEye = normalize(camera_position - pin.world_position);
    // �V���v���ȃf�B�Y�j�[�x�[�X�̊g�U���˂���������
    float diffuseFromFresnel = calc_diffuse_from_fresnel(normal, -light_direction.xyz, toEye);
    float NdotL = saturate(dot(normal, -light_direction.xyz));
    float3 lambertDiffuse = float4(1, 1, 1, 1) * NdotL / PI;
    float3 diffuse = color_map.rgb * diffuseFromFresnel * lambertDiffuse;
    // Cook-Torrance���f���𗘗p�������ʔ��˗����v�Z����
    float3 spec = cook_torrance_specular(-light_direction.xyz, toEye, normal, smooth) * float4(1, 1, 1, 1);
    spec *= lerp(float3(1, 1, 1), color_map.rgb, metallic);
    // ���炩�����g���āA�g�U���ˌ��Ƌ��ʔ��ˌ�����������
    float3 lig = diffuse * (1.0f - smooth) + spec * 2;
    // �����ɂ���グ
    lig += float3(0.4, 0.4, 0.4) * color_map.rgb;

    float4 finalColor = 1.0f;
    finalColor.xyz = float4(lig, color_map.a);
    // �u���[���Ŗ\�����Ȃ��悤�ɋ���
    finalColor.xyz = min(finalColor.xyz, 6.0);

    return finalColor * pin.color;
}