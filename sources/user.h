#pragma once


//******************************************************************************
//
//
//      ユーザー（ユーティリティー）
//
//
//******************************************************************************

//------< インクルード >----------------------------------------------------------
#include <algorithm>
#include <sstream>
#include <bitset>
#include <assert.h>
#include <DirectXMath.h>
#include "imgui_include.h"
//------< inline function >-----------------------------------------------------
namespace Math
{
    //--------------------------------------------------------------
    //  実数値のイコール判定
    //--------------------------------------------------------------
    inline float equal_check(float value1, float value2, float ep = FLT_EPSILON)
    {
        return (value1 >= value2 - ep) && (value1 < value2 + ep);
    }
    //--------------------------------------------------------------
    //  値を範囲内に収める関数
    //--------------------------------------------------------------
    //    引数：const float& v  入力する数値
    //        ：const float& lo 最小値
    //        ：const float& hi 最大値
    //  戻り値：const float&    範囲内に収まった数値
    //--------------------------------------------------------------
    inline const float& clamp(const float& v, const float& lo, const float& hi)
    {
        assert(hi >= lo);
        return (std::max)((std::min)(v, hi), lo);
    }
    inline const int& clamp(const int& v, const int& lo, const int& hi)
    {
        assert(hi >= lo);
        return (std::max)((std::min)(v, hi), lo);
    }
    //--------------------------------------------------------------
    //  値をラップアラウンド（範囲を繰り返す）させる
    //--------------------------------------------------------------
    //  const int v  範囲を繰り返させたい値
    //  const int lo 下限値
    //  const int hi 上限値（loより大きい値でなければならない）
    //--------------------------------------------------------------
    //  戻り値：int    vをloからhiまでの範囲でラップアラウンドさせた数値
    //--------------------------------------------------------------
    inline int wrap(const int v, const int lo, const int hi)
    {
        assert(hi > lo);
        const int n = (v - lo) % (hi - lo); // 負数の剰余はc++11から使用可になった
        return n >= 0 ? (n + lo) : (n + hi);
    }
    // float版
    inline float wrap(const float v, const float lo, const float hi)
    {
        assert(hi > lo);
        const float n = std::fmod(v - lo, hi - lo);
        return n >= 0 ? (n + lo) : (n + hi);
    }
    //--------------------------------------------------------------
    //  任意のフレーム後にtrueを返す
    //--------------------------------------------------------------
    inline bool stop_watch(const int frame, int& timer)
    {
        assert(frame > 0);
        assert(frame >= timer);

        if (timer < frame) ++timer;
        return timer >= frame;
    }
    //--------------------------------------------------------------
    //  3Dモデルの座標系を変更する
    //--------------------------------------------------------------
    enum COORDINATE_SYSTEM
    {
        RHS_YUP, LHS_YUP, RHS_ZUP, LHS_ZUP,
    };
    inline const DirectX::XMFLOAT4X4& conversion_coordinate_system(
        COORDINATE_SYSTEM coordinate_system, float scale_factor = 1.0f)
    {
        const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
            {-1,0,0,0,   0,1,0,0,   0,0,1,0,   0,0,0,1},    // 0:RHS Y-UP
            { 1,0,0,0,   0,1,0,0,   0,0,1,0,   0,0,0,1},	// 1:LHS Y-UP
            {-1,0,0,0,   0,0,-1,0,  0,1,0,0,   0,0,0,1},	// 2:RHS Z-UP
            { 1,0,0,0,   0,0,1,0,   0,1,0,0,   0,0,0,1},	// 3:LHS Z-UP
        };
        // To change the units from centimeters to meters, set 'scale_factor' to 0.01.
        DirectX::XMFLOAT4X4 mat;
        DirectX::XMStoreFloat4x4(&mat, DirectX::XMMATRIX{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[coordinate_system])
            * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) });
        return mat;
    }
    //--------------------------------------------------------------
    //  ワールド行列を計算する(オイラー)
    //--------------------------------------------------------------
    // 戻り値：ワールド行列
    //--------------------------------------------------------------
    inline auto calc_world_matrix(const DirectX::XMFLOAT3& scale,
        const DirectX::XMFLOAT3& rotate, const DirectX::XMFLOAT3& trans)
    {
        DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };
        DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z) };
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(trans.x, trans.y, trans.z) };
        DirectX::XMFLOAT4X4 world;
        DirectX::XMStoreFloat4x4(&world, S * R * T);
        return world;
    }
    //--------------------------------------------------------------
    // ワールド行列を計算する(オイラーの親子関係)
    //--------------------------------------------------------------
    // 戻り値：親子関係を考慮したワールド行列
    //--------------------------------------------------------------
    inline auto calc_world_matrix(const DirectX::XMFLOAT3& parent_scale,
        const DirectX::XMFLOAT3& parent_rotate, const DirectX::XMFLOAT3& parent_trans,
        const DirectX::XMFLOAT3& child_scale, const DirectX::XMFLOAT3& child_rotate,
        const DirectX::XMFLOAT3& child_trans)
    {
        DirectX::XMMATRIX P_S{ DirectX::XMMatrixScaling(parent_scale.x, parent_scale.y, parent_scale.z) };
        DirectX::XMMATRIX P_R{ DirectX::XMMatrixRotationRollPitchYaw(parent_rotate.x, parent_rotate.y, parent_rotate.z) };
        DirectX::XMMATRIX P_T{ DirectX::XMMatrixTranslation(parent_trans.x, parent_trans.y, parent_trans.z) };

        DirectX::XMMATRIX C_S{ DirectX::XMMatrixScaling(child_scale.x, child_scale.y, child_scale.z) };
        DirectX::XMMATRIX C_R{ DirectX::XMMatrixRotationRollPitchYaw(child_rotate.x, child_rotate.y, child_rotate.z) };
        DirectX::XMMATRIX C_T{ DirectX::XMMatrixTranslation(child_trans.x, child_trans.y, child_trans.z) };

        DirectX::XMMATRIX P_W = P_S * P_R * P_T;
        DirectX::XMMATRIX C_W = C_S * C_R * C_T;
        DirectX::XMFLOAT4X4 world;
        DirectX::XMStoreFloat4x4(&world, C_W * P_W);

        return world;
    }
    //--------------------------------------------------------------
    //  ワールド行列を計算する(クォータニオン)
    //--------------------------------------------------------------
    // 戻り値：ワールド行列
    //--------------------------------------------------------------
    inline auto calc_world_matrix(const DirectX::XMFLOAT3& scale,
        const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& trans)
    {
        DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };
        DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orien)) };
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(trans.x, trans.y, trans.z) };
        DirectX::XMFLOAT4X4 world;
        DirectX::XMStoreFloat4x4(&world, S * R * T);

        return world;
    }
    //--------------------------------------------------------------
    //  ワールド行列を計算する(クォータニオンの親子関係)
    //--------------------------------------------------------------
    // 戻り値：親子関係を考慮したワールド行列
    //--------------------------------------------------------------
    inline auto calc_world_matrix(const DirectX::XMFLOAT3& parent_scale,
        const DirectX::XMFLOAT4& parent_orien, const DirectX::XMFLOAT3& parent_trans,
        const DirectX::XMFLOAT3& child_scale, const DirectX::XMFLOAT4& child_orien,
        const DirectX::XMFLOAT3& child_trans)
    {
        DirectX::XMMATRIX P_S{ DirectX::XMMatrixScaling(parent_scale.x, parent_scale.y, parent_scale.z) };
        DirectX::XMMATRIX P_R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&parent_orien)) };
        DirectX::XMMATRIX P_T{ DirectX::XMMatrixTranslation(parent_trans.x, parent_trans.y, parent_trans.z) };

        DirectX::XMMATRIX C_S{ DirectX::XMMatrixScaling(child_scale.x, child_scale.y, child_scale.z) };
        DirectX::XMMATRIX C_R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&child_orien)) };
        DirectX::XMMATRIX C_T{ DirectX::XMMatrixTranslation(child_trans.x, child_trans.y, child_trans.z) };

        DirectX::XMMATRIX P_W = P_S * P_R * P_T;
        DirectX::XMMATRIX C_W = C_S * C_R * C_T;
        DirectX::XMFLOAT4X4 world;
        DirectX::XMStoreFloat4x4(&world, C_W * P_W);

        return world;
    }
    //--------------------------------------------------------------
    //  親子関係を考慮したpositionを算出
    //--------------------------------------------------------------
    inline auto calc_world_position(const DirectX::XMFLOAT3& parent_pos, const DirectX::XMFLOAT3& child_pos)
    {
        using namespace DirectX;
        XMMATRIX P_T{ DirectX::XMMatrixTranslation(parent_pos.x, parent_pos.y, parent_pos.z) };
        XMMATRIX C_T{ DirectX::XMMatrixTranslation(child_pos.x, child_pos.y, child_pos.z) };
        XMFLOAT4X4 m4x4 = {};
        XMStoreFloat4x4(&m4x4, C_T * P_T);

        return XMFLOAT3(m4x4._41, m4x4._42, m4x4._43);
    }
    //--------------------------------------------------------------
    // 点(x, y)が点(cx, cy)を中心に角(angle)で回転した時の座標を算出
    //--------------------------------------------------------------
    inline void rotate(float& x, float& y, float cx, float cy, float angle)
    {
        x -= cx;
        y -= cy;

        float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
        float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
        float tx{ x }, ty{ y };
        x = cos * tx + -sin * ty;
        y = sin * tx + cos * ty;

        x += cx;
        y += cy;
    }
    //-------------------------------------------------------
    //  線形補完(float3)
    //-------------------------------------------------------
    //  戻り値：補完された値
    //-------------------------------------------------------
    inline const DirectX::XMFLOAT3& lerp(const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end, float lerp_rate)
    {
        using namespace DirectX;
        XMVECTOR start_vec = XMLoadFloat3(&start);
        XMVECTOR end_vec = XMLoadFloat3(&end);
        XMFLOAT3 lerp;
        XMStoreFloat3(&lerp, XMVectorLerp(start_vec, end_vec, lerp_rate));
        return lerp;
    }
    //-------------------------------------------------------
    //  線形補完(float2)
    //-------------------------------------------------------
    //  戻り値：補完された値
    //-------------------------------------------------------
    inline const DirectX::XMFLOAT2& lerp(const DirectX::XMFLOAT2& start,
        const DirectX::XMFLOAT2& end, float lerp_rate)
    {
        using namespace DirectX;
        XMVECTOR start_vec = XMLoadFloat2(&start);
        XMVECTOR end_vec = XMLoadFloat2(&end);
        XMFLOAT2 lerp;
        XMStoreFloat2(&lerp, XMVectorLerp(start_vec, end_vec, lerp_rate));
        return lerp;
    }
    //-------------------------------------------------------
    //  線形補完(float)
    //-------------------------------------------------------
    //  戻り値：保管された値
    //-------------------------------------------------------
    inline float lerp(const float start, const float end, float lerp_rate)
    {
        using namespace DirectX;
        XMVECTOR start_vec = XMLoadFloat(&start);
        XMVECTOR end_vec = XMLoadFloat(&end);
        float lerp;
        XMStoreFloat(&lerp, XMVectorLerp(start_vec, end_vec, lerp_rate));
        return lerp;
    }
    //-------------------------------------------------------
    //  二点からベクトルを算出する
    //-------------------------------------------------------
    //  戻り値：aからbに向かうベクトル
    //-------------------------------------------------------
    inline const DirectX::XMVECTOR& calc_vector_AtoB(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
    {
        using namespace DirectX;
        XMVECTOR a_vec = DirectX::XMLoadFloat3(&a);
        XMVECTOR b_vec = DirectX::XMLoadFloat3(&b);
        XMVECTOR vec = b_vec - a_vec;
        return vec;
    }
    //-------------------------------------------------------
    //  二点からベクトルを算出する
    //-------------------------------------------------------
    //  戻り値：aからbに向かうベクトル
    //-------------------------------------------------------
    inline const DirectX::XMVECTOR& calc_vector_AtoB(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b)
    {
        using namespace DirectX;
        XMVECTOR a_vec = DirectX::XMLoadFloat2(&a);
        XMVECTOR b_vec = DirectX::XMLoadFloat2(&b);
        XMVECTOR vec = b_vec - a_vec;
        return vec;
    }
    //--------------------------------------------------------------
    //  二点からベクトルを算出する(正規化)
    //--------------------------------------------------------------
    //  戻り値：aからbに向かう正規化されたベクトル
    //--------------------------------------------------------------
    inline const DirectX::XMVECTOR& calc_vector_AtoB_normalize(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
    {
        return DirectX::XMVector3Normalize(calc_vector_AtoB(a, b));
    }
    //--------------------------------------------------------------
    //  二点からベクトルを算出する(正規化)
    //--------------------------------------------------------------
    //  戻り値：aからbに向かう正規化されたベクトル
    //--------------------------------------------------------------
    inline const DirectX::XMVECTOR& calc_vector_AtoB_normalize(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b)
    {
        return DirectX::XMVector2Normalize(calc_vector_AtoB(a, b));
    }
    //--------------------------------------------------------------
    //  二点からベクトルを算出し、その長さを計算する(2乗)
    //--------------------------------------------------------------
    //  戻り値：aからbに向かうベクトルの長さ
    //--------------------------------------------------------------
    inline float calc_vector_AtoB_length_sq(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
    {
        using namespace DirectX;
        XMVECTOR length_sq_vec = XMVector3LengthSq(calc_vector_AtoB(a, b));
        float length_sq;
        XMStoreFloat(&length_sq, length_sq_vec);
        return length_sq;
    }
    //--------------------------------------------------------------
    //  二点からベクトルを算出し、その長さを計算する(2乗)
    //--------------------------------------------------------------
    //  戻り値：aからbに向かうベクトルの長さ
    //--------------------------------------------------------------
    inline float calc_vector_AtoB_length_sq(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b)
    {
        using namespace DirectX;
        XMVECTOR length_sq_vec = XMVector2LengthSq(calc_vector_AtoB(a, b));
        float length_sq;
        XMStoreFloat(&length_sq, length_sq_vec);
        return length_sq;
    }
    //--------------------------------------------------------------
    //  二点からベクトルを算出し、その長さを計算する
    //--------------------------------------------------------------
    //  戻り値：aからbに向かうベクトルの長さ
    //--------------------------------------------------------------
    inline float calc_vector_AtoB_length(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
    {
        using namespace DirectX;
        XMVECTOR length_vec = XMVector3Length(calc_vector_AtoB(a, b));
        float length;
        XMStoreFloat(&length, length_vec);
        return length;
    }
    //--------------------------------------------------------------
    //  二点からベクトルを算出し、その長さを計算する
    //--------------------------------------------------------------
    //  戻り値：aからbに向かうベクトルの長さ
    //--------------------------------------------------------------
    inline float calc_vector_AtoB_length(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b)
    {
        using namespace DirectX;
        XMVECTOR length_vec = XMVector2Length(calc_vector_AtoB(a, b));
        float length;
        XMStoreFloat(&length, length_vec);
        return length;
    }
    //--------------------------------------------------------------
    //  指定方向に指定距離進んだ点を算出する
    //--------------------------------------------------------------
    //  戻り値：startからdirection方向にlength分進んだ地点
    //--------------------------------------------------------------
    inline const DirectX::XMFLOAT3& calc_designated_point(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& direction, float length)
    {
        return { start.x + direction.x * length, start.y + direction.y * length, start.z + direction.z * length };
    }
    //--------------------------------------------------------------
    //  指定方向に指定距離進んだ点を算出する
    //--------------------------------------------------------------
    //  戻り値：startからdirection方向にlength分進んだ地点
    //--------------------------------------------------------------
    inline const DirectX::XMFLOAT2& calc_designated_point(const DirectX::XMFLOAT2& start,
        const DirectX::XMFLOAT2& direction, float length)
    {
        return { start.x + direction.x * length, start.y + direction.y * length };
    }
    //--------------------------------------------------------------
    //  線分 start end と点p0の最近点を算出する
    //--------------------------------------------------------------
    //  戻り値：線分 start end と点p0の最近点
    //--------------------------------------------------------------
    inline const DirectX::XMFLOAT3& calc_closest_point(const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& p0)
    {
        using namespace DirectX;
        XMVECTOR v0_vec = XMLoadFloat3(&end) - XMLoadFloat3(&start);
        XMVECTOR v1_vec = XMLoadFloat3(&p0) - XMLoadFloat3(&start);
        XMVECTOR projection_length_vec = XMVector3Dot(v1_vec, XMVector3Normalize(v0_vec));
        float projection_length;
        XMStoreFloat(&projection_length, projection_length_vec);
        XMFLOAT3 v0_norm;
        XMStoreFloat3(&v0_norm, XMVector3Normalize(v0_vec));
        return calc_designated_point(start, v0_norm, projection_length);
    }
}

//--------------------------------------------------------------
//  strBit16    整数を2進数（16bit）のstringに変換する
//--------------------------------------------------------------
//  　引数：const int n     変換する整数
//  戻り値：std::string     数値を2進数に変換したもの(string)
//--------------------------------------------------------------
inline std::string strBit16(const int n)
{
    std::stringstream ss;
    ss << static_cast<std::bitset<16>>(n);
    return ss.str();
}

// imgui Menu
inline void imgui_menu_bar(std::string menu_label, std::string menu_item_label, bool& selected)
{
#ifdef USE_IMGUI
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(menu_label.c_str()))
        {
            ImGui::MenuItem(menu_item_label.c_str(), NULL, &selected);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
#endif // USE_IMGUI
}

template <typename T>
inline void safe_delete(T*& p)
{
    if (p != nullptr)
    {
        delete(p);
        (p) = nullptr;
    }
}

template<typename T>
inline void safe_delete_array(T*& p)
{
    if (p != nullptr)
    {
        delete[](p);
        (p) = nullptr;
    }
}

template <typename T>
inline void safe_release(T*& p)
{
    if (p != nullptr)
    {
        (p)->Release();
        (p) = nullptr;
    }
}