#pragma once
#include"graphics_pipeline.h"
#include"BaseBullet.h"
#include "Common.h"
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
    void fFinalize();
    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]]AddBulletFunc fGetAddFunction() const;

    // �e�ƃv���C���[�̓����蔻��
    bool fCalcBulletsVsPlayer(
        DirectX::XMFLOAT3 PlayerCapsulePointA_,// �v���C���[�̃J�v�Z���̏��
        DirectX::XMFLOAT3 PlayerCapsulePointB_,// �v���C���[�̃J�v�Z���̏��
        float PlayerCapsuleRadius_,             // �v���C���[�̃J�v�Z���̏��
        AddDamageFunc Func_
    );
private:
    void fAllClear();


    //****************************************************************
    // 
    //  �ϐ�
    // 
    //****************************************************************
private:
    AddBulletFunc mfAddBullet; // �e�𐶐�����֐�
    std::vector<BaseBullet*> mBulletVec{}; // �e���i�[����R���e�i
    std::vector<BaseBullet*> mRemoveVec{}; // �e���i�[����R���e�i

};
