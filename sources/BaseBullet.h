#pragma once

#include"effect.h"
#include"graphics_pipeline.h"
#include"practical_entities.h"
#include<memory>
//****************************************************************
// 
// �e�̊��N���X 
// 
//****************************************************************
class BaseBullet : public PracticalEntities
{
public:
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
    BaseBullet(GraphicsPipeline& Graphics_, const char* EffectFileName_);
    ~BaseBullet() override = default;

    virtual void fUpdate(float elapsedTime_);
protected:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    std::unique_ptr<Effect> mpEffect{ nullptr };
    DirectX::XMFLOAT3 mPosition{};
    DirectX::XMFLOAT3 mScale{};
    DirectX::XMFLOAT4 mOrientation{};
    bool mIsAlive{}; // �e�̐�������
    DirectX::XMFLOAT3 mVelocity{}; // ���x

};
