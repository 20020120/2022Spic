#pragma once
#include"graphics_pipeline.h"
#include"BaseBullet.h"
#include<functional>
#include<vector>
//****************************************************************
// 
// 弾の管理クラス 
// 
//****************************************************************

class BulletManager final
{
public:
    BulletManager() = default;
    ~BulletManager() = default;

    void fInitialize();
    void fUpdate(float elapsedTime_);
    void fRender(GraphicsPipeline& Graphics_);

    //--------------------<ゲッター関数>--------------------//
    std::function<void(BaseBullet*)> fGetAddFunction{};

private:
    void fAllClear();


    //****************************************************************
    // 
    //  変数
    // 
    //****************************************************************
private:
    std::function<void(BaseBullet*)> mfAddBullet; // 弾を生成する関数
    std::vector<BaseBullet*> mBulletVec{}; // 弾を格納するコンテナ
    std::vector<BaseBullet*> mRemoveVec{}; // 弾を格納するコンテナ

};
