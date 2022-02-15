#pragma once

#include <DirectXMath.h>
#include "practical_entities.h"
#include "constants.h"

class Camera : public PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    Camera(GraphicsPipeline& graphics);
    ~Camera() {}
public:
    //--------< 関数 >--------//
    void update_with_euler_angles(float elapsed_time);
    void update_with_quaternion(float elapsed_time);
    void calc_view_projection(GraphicsPipeline& graphics);
    void debug_gui();
    //--------<getter/setter>--------//
    // range
    void set_range(float r) { range = r; }
    // target
    void set_target(const DirectX::XMFLOAT3& t) { target = t; }
    const DirectX::XMFLOAT3& get_target() { return target; }
    // eye
    const DirectX::XMFLOAT3& get_eye() { return eye; }
    // angle
    void set_angle(const DirectX::XMFLOAT3& a) { angle = a; }
    // light_direction
    const DirectX::XMFLOAT4& get_light_direction() { return scene_constants->data.light_direction; }
    // view
    static const DirectX::XMFLOAT4X4& get_keep_view() { return keep_view; }
    // projection
    static const DirectX::XMFLOAT4X4& get_keep_projection() { return keep_projection; }
    // view_projection
    const DirectX::XMFLOAT4X4& get_view_projection()
    {
        using namespace DirectX;
        XMMATRIX V = XMLoadFloat4x4(&view);
        XMMATRIX P = XMLoadFloat4x4(&projection);
        XMFLOAT4X4 view_projection;
        XMStoreFloat4x4(&view_projection, V * P);

        return view_projection;
    }
    // near_far
    void set_near_far(const DirectX::XMFLOAT2& n_f) { near_far = n_f; }
private:
    //--------< 関数 >--------//
    void mouse_operation_with_euler_angles(float elapsed_time);
    void keyboard_operation_with_euler_angles(float elapsed_time);
    void mouse_operation_with_quaternion(float elapsed_time);
    void keyboard_operation_with_quaternion(float elapsed_time);
    //--------< 定数/構造体 >--------//
    struct SceneConstants
    {
        DirectX::XMFLOAT4X4 view_projection;              //ビュー・プロジェクション変換行列
        DirectX::XMFLOAT4 light_direction{ 0, -1, 1, 1 }; //ライトの向き
        DirectX::XMFLOAT4 light_color{ 1,1,1,1 };         //ライトの色
        DirectX::XMFLOAT4 camera_position;
        DirectX::XMFLOAT4X4 shake_matrix;
        DirectX::XMFLOAT4X4 inverse_view_projection;
    };
    std::unique_ptr<Constants<SceneConstants>> scene_constants;
    //--------< 変数 >--------//
    float range{ 0.5f };
    DirectX::XMFLOAT3 eye{};
    DirectX::XMFLOAT2 near_far = { 0.1f, 700.0f }; // x:near y:far
    DirectX::XMFLOAT3 target{};
    DirectX::XMFLOAT3 angle{ 0.4f, 0, 0 };
    DirectX::XMFLOAT4 orientation;
    float vertical_rotation_degree = 0;
    float horizon_rotation_degree = 0;
    float roll_speed{ DirectX::XMConvertToRadians(90) };
    float max_angle_x{ DirectX::XMConvertToRadians(60) };
    float min_angle_x{ DirectX::XMConvertToRadians(-60) };
    bool is_mouse_operation{ false };
    DirectX::XMFLOAT4X4 view{};
    DirectX::XMFLOAT4X4 projection{};
    bool display_camera_imgui{ false };

    static DirectX::XMFLOAT4X4 keep_view;
    static DirectX::XMFLOAT4X4 keep_projection;
};