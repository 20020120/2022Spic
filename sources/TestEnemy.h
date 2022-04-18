#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>

#include "EnemiesEditor.h"
//****************************************************************
// 
// �e�X�g�ƃT���v�������˂��G�̔h���N���X 
// 
//****************************************************************
class TestEnemy final:public BaseEnemy
{
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    
    TestEnemy(GraphicsPipeline& graphics_,
        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/,
        int UniqueId_,
        ParamGetFunction Function_);

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_) override;
    void fStopEffect() override;

    void fGuiMenu()override;
    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
private:
    // �X�e�[�g�}�V����ǉ�����֐�
    void fRegisterFunctions() override;

    //--------------------<�e�X�e�[�g�̊֐�>--------------------//
    void fIdleInit();   // �ҋ@�̏�����
    void fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); // �ҋ@�̍X�V����
    
protected:
    void fSetCapsulePoint() override;
};
