#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>
#include "graphics_pipeline.h"

class Effect
{
public:
    Effect(GraphicsPipeline& graphics, Effekseer::Manager* effekseer_manager, const char* filename);
    ~Effect();
public:
    // �Đ�
    void play(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& position, float scale = 1.0f);
    // ��~
    void stop(Effekseer::Manager* effekseer_manager);
    // ���W�ݒ�
    void set_position(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& position);
    // �X�P�[���ݒ�
    void set_scale(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& scale);
    // �X�s�[�h�ݒ�
    void set_speed(Effekseer::Manager* effekseer_manager, float speed);
    // �s����Z�b�g
    void set_rotationY(Effekseer::Manager* effekseer_manager, const float angle);
    // �N�I�[�^�j�I��
    void set_quaternion(Effekseer::Manager* effekseer_manager, DirectX::XMFLOAT4 orientation);
private:
    Effekseer::Effect* effekseer_effect{ nullptr };
    Effekseer::Handle effekseer_handle{ -1 };
};