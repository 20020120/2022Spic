#include"BulletManager.h"

void BulletManager::fInitialize()
{
    // 初期化
    fAllClear();
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
    for (const auto bullet : mBulletVec)
    {
        bullet->fRender(Graphics_);
    }
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
