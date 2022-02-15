#pragma once
#include"graphics_pipeline.h"
#include"skinned_mesh.h"
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
    BaseEnemy();
    virtual ~BaseEnemy() = default;

    virtual void fInitialize() = 0;
    virtual void fUpdate(GraphicsPipeline GPipeline_, float elapsedTime_) = 0;
    void fRender(GraphicsPipeline gPipeline_);

    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
private:
    // ���f��
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };

};