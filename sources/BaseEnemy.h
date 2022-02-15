#pragma once
#include"skinned_mesh.h"
#include<d3d11.h>
#include<memory>

//****************************************************************
// 
// �G�̊��N���X 
// 
//****************************************************************
class BaseEnemy
{
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    BaseEnemy(ID3D11Device *pDevice_,const char* ModelName_);
    virtual ~BaseEnemy();

    virtual void fInitialize() = 0;
    virtual void fUpdate(float elapsedTime_) = 0;
    void fRender(ID3D11DeviceContext* pDeviceContext_) const;

    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
protected:
    DirectX::XMFLOAT3 mPosition{};    // �ʒu
    DirectX::XMFLOAT4 mOrientation{}; // ��]
    DirectX::XMFLOAT3 mScale{};       // �傫��
private:
    // ���f��
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };
};