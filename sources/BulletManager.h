#pragma once
#include"graphics_pipeline.h"
#include"BaseBullet.h"
#include<functional>
#include<vector>
//****************************************************************
// 
// �e�̊Ǘ��N���X 
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

    //--------------------<�Q�b�^�[�֐�>--------------------//
    std::function<void(BaseBullet*)> fGetAddFunction{};

private:
    void fAllClear();


    //****************************************************************
    // 
    //  �ϐ�
    // 
    //****************************************************************
private:
    std::function<void(BaseBullet*)> mfAddBullet; // �e�𐶐�����֐�
    std::vector<BaseBullet*> mBulletVec{}; // �e���i�[����R���e�i
    std::vector<BaseBullet*> mRemoveVec{}; // �e���i�[����R���e�i

};
