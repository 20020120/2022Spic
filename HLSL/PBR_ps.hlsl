#include "skinned_mesh.hlsli"
#include "constants.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
texture2D texture_maps[4] : register(t0);

static const float PI = 3.1415926f; // π

float3 get_normal(float3 normal, float3 tangent, float3 binormal, float2 uv)
{
    float3 bin_space_normal = texture_maps[1].SampleLevel(sampler_states[ANISOTROPIC], uv, 0.0f).xyz;
    bin_space_normal = (bin_space_normal * 2.0f) - 1.0f;

    float3 new_normal = tangent * bin_space_normal.x + binormal * bin_space_normal.y + normal * bin_space_normal.z;

    return new_normal;
}
// ベックマン分布を計算する
float beckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
    return D;
}
// フレネルを計算。Schlick近似を使用
float spc_fresnel(float f0, float u)
{
    // from Schlick
    return f0 + (1 - f0) * pow(1 - u, 5);
}
// Cook-Torranceモデルの鏡面反射を計算
// <param name="L">光源に向かうベクトル</param>
// <param name="V">視点に向かうベクトル</param>
// <param name="N">法線ベクトル</param>
// <param name="metallic">金属度</param>
float cook_torrance_specular(float3 L, float3 V, float3 N, float metallic)
{
    float microfacet = 0.76f;
    // 金属度を垂直入射の時のフレネル反射率として扱う
    // 金属度が高いほどフレネル反射は大きくなる
    float f0 = metallic;
    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);
    // 各種ベクトルがどれくらい似ているかを内積を利用して求める
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    // D項をベックマン分布を用いて計算する
    float D = beckmann(microfacet, NdotH);
    // F項をSchlick近似を用いて計算する
    float F = spc_fresnel(f0, VdotH);
    // G項を求める
    float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));
    // m項を求める
    float m = PI * NdotV * NdotH;
    // ここまで求めた、値を利用して、Cook-Torranceモデルの鏡面反射を求める
    return max(F * D * G / m, 0.0);
}
// フレネル反射を考慮した拡散反射を計算
// </summary>
// <remark>
// この関数はフレネル反射を考慮した拡散反射率を計算します
// フレネル反射は、光が物体の表面で反射する現象のとこで、鏡面反射の強さになります
// 一方拡散反射は、光が物体の内部に入って、内部錯乱を起こして、拡散して反射してきた光のことです
// つまりフレネル反射が弱いときには、拡散反射が大きくなり、フレネル反射が強いときは、拡散反射が小さくなります

// </remark>
// <param name="N">法線</param>
// <param name="L">光源に向かうベクトル。光の方向と逆向きのベクトル。</param>
// <param name="V">視線に向かうベクトル。</param>
float calc_diffuse_from_fresnel(float3 N, float3 L, float3 V)
{
    // step-4 フレネル反射を考慮した拡散反射光を求める
    // 法線と光源に向かうベクトルがどれだけ似ているかを内積で求める
    float dotNL = saturate(dot(N, L));

    // 法線と視線に向かうベクトルがどれだけ似ているかを内積で求める
    float dotNV = saturate(dot(N, V));

    // 法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
    // 乗算して最終的な拡散反射率を求めている。PIで除算しているのは正規化を行うため
    return (dotNL * dotNV);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color_map     = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 normal_map    = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    float4 metal_map     = texture_maps[2].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float4 roughness_map = texture_maps[3].Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    // 法線を計算
    float3 N = normalize(pin.world_normal.xyz);
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
    float3 normal = get_normal(N, T, B, pin.texcoord);
    // 金属度
    float metallic = metal_map.r;
    // 滑らかさ
    float smooth = roughness_map.r;
    // 視線に向かって伸びるベクトルを計算する
    float3 toEye = normalize(camera_position - pin.world_position);
    // シンプルなディズニーベースの拡散反射を実装する
    float diffuseFromFresnel = calc_diffuse_from_fresnel(normal, -light_direction.xyz, toEye);
    float NdotL = saturate(dot(normal, -light_direction.xyz));
    float3 lambertDiffuse = float4(1, 1, 1, 1) * NdotL / PI;
    float3 diffuse = color_map.rgb * diffuseFromFresnel * lambertDiffuse;
    // Cook-Torranceモデルを利用した鏡面反射率を計算する
    float3 spec = cook_torrance_specular(-light_direction.xyz, toEye, normal, smooth) * float4(1, 1, 1, 1);
    spec *= lerp(float3(1, 1, 1), color_map.rgb, metallic);
    // 滑らかさを使って、拡散反射光と鏡面反射光を合成する
    float3 lig = diffuse * (1.0f - smooth) + spec * 2;
    // 環境光による底上げ
    lig += float3(0.4, 0.4, 0.4) * color_map.rgb;

    float4 finalColor = 1.0f;
    finalColor.xyz = float4(lig, color_map.a);
    // ブルームで暴走しないように強制
    finalColor.xyz = min(finalColor.xyz, 6.0);

    return finalColor * pin.color;
}