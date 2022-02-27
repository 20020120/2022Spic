#pragma once
#include"graphics_pipeline.h"
#include"skinned_mesh.h"

//**********************************************************
// 
//  ���[�U�[�r�[��
// 
//****************************************************************
class LaserBeam final
{
    //****************************************************************
    // 
    //  �֐�
    // 
    //****************************************************************
public:
    LaserBeam() = default;
    ~LaserBeam() = default;

    void fInitialize(ID3D11Device* pDevice_, const wchar_t* TextureName_);
    void fUpdate();
    void fRender(GraphicsPipeline& Graphics_);
    void fSetPosition(DirectX::XMFLOAT3 Start_, DirectX::XMFLOAT3 End_);
    void fSetRadius(float Radius_);

private:
    void fCalcTransform(); // �p���̏����Z�o


private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************

    DirectX::XMFLOAT3 mStartPoint{}; // �r�[���̊J�n�n�_
    DirectX::XMFLOAT3 mEndPoint{}; // �r�[���̊J�n�n�_
    float mRadius{};  // ���a
    DirectX::XMFLOAT4 mOrientation{}; // ��]
    DirectX::XMFLOAT3 mScale{}; // �傫��
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };

};

