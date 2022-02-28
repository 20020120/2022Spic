#pragma once 
#include"EnemyManager.h"
//****************************************************************
// 
// �E�F�[�u���Ǘ�����N���X
// 
//****************************************************************
class WaveManager final 
{
    //****************************************************************
    // 
    // �\����
    // 
    //****************************************************************
public:
    enum class WaveState
    {
        Game,
        Clear,
    };

    //****************************************************************
    // 
    //  �֐�
    // 
    //****************************************************************
public:
    WaveManager() = default;
    ~WaveManager() = default;

    // ������
    void fInitialize(GraphicsPipeline& graphics_);
    // �X�V
    void fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_, AddBulletFunc Func_);
    // �I������
    void fFinalize();

    // �N���A����
    void fWaveClear();
    // �E�F�[�u�J�n
    void fStartWave();

    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]]  EnemyManager* fGetEnemyManager() ;

    //--------------------<�Z�b�^�[�֐�>--------------------//


private:

    void fClearUpdate(float elapsedTime_);

    //****************************************************************
    // 
    //  �ϐ�
    // 
    //****************************************************************
    // ���݂̃E�F�[�u
    int mCurrentWave{};

    EnemyManager mEnemyManager{};
    WaveState mWaveState;
};


