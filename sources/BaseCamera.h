#pragma once

#include <DirectXMath.h>
#include "practical_entities.h"
#include "constants.h"

class BaseCamera : public PracticalEntities
{
public:
    BaseCamera(){}
    virtual ~BaseCamera(){};
public:

    //forward
    DirectX::XMFLOAT3 GetForward() { return forward; }
    //right
    DirectX::XMFLOAT3 GetRight() { return right; }

    void UpdateViewProjection();

    void AttitudeControl(float elapsedTime);

    void DebugGUI();
protected:
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
    DirectX::XMFLOAT3 forward{};
    DirectX::XMFLOAT3 right{};
    DirectX::XMFLOAT4X4 view{};
    DirectX::XMFLOAT4X4 projection{};

    DirectX::XMFLOAT3 eyeVector{};
    DirectX::XMFLOAT3 rockOnEyeVector{};
    DirectX::XMFLOAT3 playerToRockOn{};

    float radius{};
    float horizonDegree{};
    float verticalDegree{};

    bool rockOnStart{ true };
    float rockOnTimer{};

};