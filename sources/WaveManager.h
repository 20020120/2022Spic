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
        Start, // �Q�[���J�n�O
        Game,  // �Q�[����
        Clear, // �Q�[���N���A
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
    void fInitialize(GraphicsPipeline& graphics_, AddBulletFunc Func_);
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
    void fSetStartGame(bool Arg_);

    //--------------------<ImGui>--------------------//
    void fGuiMenu();

private:

    void fClearUpdate(float elapsedTime_);

    //****************************************************************
    // 
    //  �ϐ�
    // 
    //****************************************************************
    // ���݂̃E�F�[�u
    int mCurrentWave{};

    // �ŏ��̉��o���I���������ǂ���
    bool mStartGame{};

    EnemyManager mEnemyManager{};
    WaveState mWaveState;

    //--------------------<ImGui>--------------------//
    bool mOpenGui{};

};


