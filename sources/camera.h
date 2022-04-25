#pragma once

#include <DirectXMath.h>

#include "BaseCamera.h"
#include "practical_entities.h"
#include "constants.h"
#include "Player.h"

#if 0
class camera : public BaseCamera
{
public:
    camera(int cameraType, GraphicsPipeline& graphics, Player* player);
    ~camera();

    void Initialize(GraphicsPipeline& graphics, Player* player);
    void Update(float elapsedTime, Player* player);

    void titleInitialize(GraphicsPipeline& graphics ,Player* player);
    void gameInitialize(GraphicsPipeline& graphics, Player* player);

    void titleUpdate(float elapsedTime,  Player* player);
    void gameUpdate(float elapsedTime, Player* player);

    typedef void (camera::* cameraInitialize)(GraphicsPipeline& graphics, Player* player);
    cameraInitialize initialize = &camera::gameInitialize;

    typedef void (camera::* cameraUpdate)(float elapsed_time, Player* player);
    cameraUpdate update = &camera::gameUpdate;

    bool active{ false };
private:


    void SetAngle(float elapsedTime);
    void UpdateEyeVector(float elapsedTime, DirectX::XMVECTOR PlayerUp);
    void AttitudeControl(float elapsedTime);

    void UpdateEye();
    void UpdateTarget(DirectX::XMVECTOR PlayerPosition, DirectX::XMVECTOR PlayerUp);
    void UpdateRockOnTarget(DirectX::XMVECTOR PlayerTarget);

    bool CameraReset(float elapsedTime, DirectX::XMVECTOR PlayerForward, DirectX::XMVECTOR PlayerUp);

    bool RockOnUpdateEyeVector(float elapsedTime, DirectX::XMVECTOR PlayerUp, bool rockOnStart);
    void RockOnCalculateEyeVector(DirectX::XMVECTOR PlayerPosition, DirectX::XMVECTOR RockOnPosition, DirectX::XMVECTOR PlayerUp);

    bool RockOnCameraReset(float elapsedTime, DirectX::XMVECTOR PlayerForward, DirectX::XMVECTOR PlayerUp);

};
#else

class Camera :public BaseCamera
{
public:
    Camera(Player* p) :player(p){}
    Camera(){}
    ~Camera() {}

    virtual void Initialize(GraphicsPipeline& graphics) = 0;
    virtual void Update(float elapsedTime) = 0;

protected:
    Player* player;

};

class TitleCamera : public Camera
{
public:
    TitleCamera(){};
    ~TitleCamera(){};

    void Initialize(GraphicsPipeline& graphics);
    void Update(float elapsedTime);
};

class GameCamera : public Camera
{
public:
    GameCamera(Player* player) : Camera(player) {};
    ~GameCamera(){};

    void Initialize(GraphicsPipeline& graphics);
    void Update(float elapsedTime);

    void gameInitialize(GraphicsPipeline& graphics);

    void gameUpdate(float elapsedTime);


private:


    void SetAngle(float elapsedTime);
    void UpdateEyeVector(float elapsedTime, DirectX::XMVECTOR PlayerUp);
    void AttitudeControl(float elapsedTime);

    void UpdateEye();
    void UpdateTarget(DirectX::XMVECTOR PlayerPosition, DirectX::XMVECTOR PlayerUp);
    void UpdateRockOnTarget(DirectX::XMVECTOR PlayerTarget);

    bool CameraReset(float elapsedTime, DirectX::XMVECTOR PlayerForward, DirectX::XMVECTOR PlayerUp);

    bool RockOnUpdateEyeVector(float elapsedTime, DirectX::XMVECTOR PlayerUp, bool rockOnStart);
    void RockOnCalculateEyeVector(DirectX::XMVECTOR PlayerPosition, DirectX::XMVECTOR RockOnPosition, DirectX::XMVECTOR PlayerUp);

    bool RockOnCameraReset(float elapsedTime, DirectX::XMVECTOR PlayerForward, DirectX::XMVECTOR PlayerUp);

};

class ClearCamera : public Camera
{
public:
    ClearCamera(Player * player):Camera(player){}
    ~ClearCamera(){}

    void Initialize(GraphicsPipeline& graphics);
    void Update(float elapsedTime);

private:
    bool CameraReset(float elapsedTime, DirectX::XMVECTOR PlayerForward, DirectX::XMVECTOR PlayerUp);

    bool cameraReset{ true };
};
#endif
