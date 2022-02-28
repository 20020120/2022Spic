#pragma once
#include"graphics_pipeline.h"
#include"BaseBullet.h"
#include "Common.h"
#include<functional>
#include<vector>
//****************************************************************
// 
// ’e‚ÌŠÇ—ƒNƒ‰ƒX 
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

    //--------------------<ƒQƒbƒ^[ŠÖ”>--------------------//
    [[nodiscard]]AddBulletFunc fGetAddFunction() const;

private:
    void fAllClear();


    //****************************************************************
    // 
    //  •Ï”
    // 
    //****************************************************************
private:
    AddBulletFunc mfAddBullet; // ’e‚ğ¶¬‚·‚éŠÖ”
    std::vector<BaseBullet*> mBulletVec{}; // ’e‚ğŠi”[‚·‚éƒRƒ“ƒeƒi
    std::vector<BaseBullet*> mRemoveVec{}; // ’e‚ğŠi”[‚·‚éƒRƒ“ƒeƒi

};
