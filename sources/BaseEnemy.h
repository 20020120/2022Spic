#pragma once
#include"graphics_pipeline.h"
#include"skinned_mesh.h"
#include<memory>
//****************************************************************
// 
// 敵の基底クラス 
// 
//****************************************************************

class BaseEnemy
{
    //****************************************************************
    // 
    // 関数
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
    // 変数 
    // 
    //****************************************************************
private:
    // モデル
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };

};