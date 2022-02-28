#include"BulletManager.h"

void BulletManager::fInitialize()
{
    // ������
    fAllClear();

    // �e���}�l�[�W���ɒǉ�����֐�
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
    // �R���e�i���̂��ׂĂ̗v�f���폜
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
