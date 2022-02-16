#pragma once
#include"BasePlayer.h"
#include"PlayerMove.h"
#include"collision.h"
#include"graphics_pipeline.h"

class Player :public BasePlayer, private PlayerMove
{
public:
    Player(GraphicsPipeline& graphics);
    ~Player();
public:
    void Initialize()override;
    void Update(float elapsed_time, SkyDome* sky_dome)override;
    void Render(GraphicsPipeline& graphics, float elapsed_time)override;
private:
    DirectX::XMFLOAT3 camera_forward{};//カメラの前方向
    DirectX::XMFLOAT3 camera_right{};//カメラの右方向

    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up;
    //カメラの向きをリセット
    bool camera_reset{ false };
public:
    DirectX::XMFLOAT3 GetForward() { return forward; }
    DirectX::XMFLOAT3 GetRight() { return right; }
    DirectX::XMFLOAT3 GetUp() { return up; }
    DirectX::XMFLOAT3 GetPosition() { return position; }
    DirectX::XMFLOAT3 GetVelocity() { return velocity; }
    bool GetCameraReset() { return camera_reset; }
    void FalseCameraReset() { camera_reset = false; }
    HitResult& GetPlayerHitResult() { return hit; }
    void SetRaycast(bool r) { raycast = r; }
private:
    void GetPlayerDirections();
public:
    void SetCameraDirection(const DirectX::XMFLOAT3& c_forward,  const DirectX::XMFLOAT3& c_right)
    {
        camera_forward = c_forward;
        camera_right = c_right;
    }
};