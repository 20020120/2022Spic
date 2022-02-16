#pragma once
#include"BasePlayer.h"
#include"PlayerMove.h"
#include"graphics_pipeline.h"

class Player :public BasePlayer, private PlayerMove
{
public:
    Player(GraphicsPipeline& graphics);
    ~Player();
public:
   void Initialize()override;
   void Update(float elapsed_time)override;
   void Render(GraphicsPipeline& graphics, float elapsed_time)override;
private:
    DirectX::XMFLOAT3 camera_forward{};
    DirectX::XMFLOAT3 camera_right{};
public:
    void SetCameraDirection(const DirectX::XMFLOAT3& c_forward,  const DirectX::XMFLOAT3& c_right)
    {
        camera_forward = c_forward;
        camera_right = c_right;
    }
};