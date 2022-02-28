#include"BulletManager.h"

void BulletManager::fInitialize()
{
    // 初期化
    fAllClear();

    // 弾をマネージャに追加する関数
    mfAddBullet = [=](BaseBullet* pBaseBullet_)->void
    {
        mBulletVec.emplace_back(pBaseBullet_);
    };
}

void BulletManager::fUpdate(float elapsedTime_)
{
    for(const auto bullet: mBulletVec)
    {
        bullet->fUpdate(elapsedTime_);
    }
}

void BulletManager::fRender(GraphicsPipeline& Graphics_)
{
   
}

void BulletManager::fFinalize()
{
    fAllClear();
}

AddBulletFunc BulletManager::fGetAddFunction() const
{
    return mfAddBullet;
}

void BulletManager::fAllClear()
{
    // コンテナ内のすべての要素を削除
    for(auto bullet : mBulletVec)
    {
        if (bullet)
        {
            delete bullet;
            bullet = nullptr;
        }
    }
    for (auto bullet : mRemoveVec)
    {
        if (bullet)
        {
            delete bullet;
            bullet = nullptr;
        }
    }
    mBulletVec.clear();
    mRemoveVec.clear();
}
