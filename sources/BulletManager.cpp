#include"BulletManager.h"

#include "collision.h"

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


bool BulletManager::fCalcBulletsVsPlayer(DirectX::XMFLOAT3 PlayerCapsulePointA_, DirectX::XMFLOAT3 PlayerCapsulePointB_, float PlayerCapsuleRadius_, AddDamageFunc Func_)
{
    for (const auto bullet : mBulletVec)
    {
        BaseBullet::BulletData data = bullet->fGetBulletData();
        const bool result = Collision::capsule_vs_capsule(
            PlayerCapsulePointA_, PlayerCapsulePointB_, PlayerCapsuleRadius_,
            data.mPointA, data.mPointB, data.mRadius);

        // �������Ă�����
        if (result)
        {
            Func_(data.mDamage, data.mInvincible_time);
            return true;
        }

    }
    return false;
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
