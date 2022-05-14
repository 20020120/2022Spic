#pragma once
#include"BaseEnemy.h"
#include"EnemyFileSystem.h"
#include"EnemiesEditor.h"
#include"practical_entities.h"
#include"BossRushUnit.h"
#include<vector>


#include "Common.h"

//****************************************************************
//
// �G�̊Ǘ��N���X
//
//****************************************************************
class EnemyManager final :public PracticalEntities
{
    //****************************************************************
    //
    // �֐�
    //
    //****************************************************************
public:
    EnemyManager() = default;
    ~EnemyManager() override;

    void fInitialize(GraphicsPipeline& graphics_, AddBulletFunc Func_);
    void fUpdate(GraphicsPipeline & graphics_,float elapsedTime_, AddBulletFunc Func_);
    void fRender(GraphicsPipeline& graphics_);
    void fFinalize();

public:
    //--------------------<�����蔻��>--------------------//

    // �v���C���[�̍U���ƓG�̓����蔻��
    int fCalcPlayerAttackVsEnemies(           // �߂�l �F ���������G�̐�
        DirectX::XMFLOAT3 PlayerCapsulePointA_,// �v���C���[�̃J�v�Z���̏��
        DirectX::XMFLOAT3 PlayerCapsulePointB_,// �v���C���[�̃J�v�Z���̏��
        float PlayerCapsuleRadius_,            // �v���C���[�̃J�v�Z���̏��
        int PlayerAttackPower_   ,              // �v���C���[�̍U����
        GraphicsPipeline& Graphics_,
        float elapsedTime_
    );
    // �G�̍U���ƃv���C���[�̓����蔻��
    bool fCalcEnemiesAttackVsPlayer(
        DirectX::XMFLOAT3 PlayerCapsulePointA_,// �v���C���[�̃J�v�Z���̏��
        DirectX::XMFLOAT3 PlayerCapsulePointB_,// �v���C���[�̃J�v�Z���̏��
        float PlayerCapsuleRadius_,             // �v���C���[�̃J�v�Z���̏��
        AddDamageFunc Func_
    );

    // �G�̍U���ƃv���C���[�̃J�E���^�[�̓����蔻��
    bool fCalcEnemiesAttackVsPlayerCounter(
        DirectX::XMFLOAT3 PlayerCapsulePointA_,// �v���C���[�̃J�v�Z���̏��
        DirectX::XMFLOAT3 PlayerCapsulePointB_,// �v���C���[�̃J�v�Z���̏��
        float PlayerCapsuleRadius_);             // �v���C���[�̃J�v�Z���̏��)
    
    // �v���C���[�̉�荞�ݍU���ƓG�̓����蔻��
    void fCalcPlayerStunVsEnemyBody(const DirectX::XMFLOAT3 PlayerPosition_, float Radius_);

    // �X�^�����̓G�̂����v���C���[�ɂ����Ƃ��߂����̂�Ԃ��i���Ȃ����null�j
    BaseEnemy* fGetNearestStunEnemy();


// �G��ǉ�����
    void fAddRushBoss(BossRushUnit* enemy);
public:
    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]]  BaseEnemy* fGetNearestEnemyPosition();
    [[nodiscard]]  BaseEnemy* fGetSecondEnemyPosition();
    [[nodiscard]] bool fGetClearWave() const;
    //--------------------<�Z�b�^�[�֐�>--------------------//
    void fSetIsPlayerChainTime(bool IsChain_);//�v���C���[���`�F�C����Ԃł��邩�ǂ������G�l�~�[���󂯎��֐�
    void fSetPlayerPosition(DirectX::XMFLOAT3 Position_);
    void fSetPlayerSearch(bool Arg_); // �v���C���[���`�F�C���U������True

public:
    //--------------------<ImGui>--------------------//
    void fGuiMenu(GraphicsPipeline& Graphics_, AddBulletFunc Func_);
    //--------------------<�E�F�[�u�؂�ւ��֐�>--------------------//
    void fStartWave(int WaveIndex_);
    //--------------------<�E�F�[�u�N���A�������ǂ����𔻒肷��>--------------------//
    [[nodiscard]] bool fWaveClear() const;

    //--------------------<�G�̎��̂��폜����֐�>--------------------//
    void fDeleteEnemies();
    
    [[nodiscard]] std::vector<BaseEnemy*> fGetEnemies() const;
    size_t fGetEnemyCounts()const;

    //--------------------<�L���b�V���Ƀ��f���f�[�^��o�^>--------------------//
    void fRegisterCash(GraphicsPipeline& graphics_);
    void fDeleteCash(); // �L���b�V�����폜

    // �{�X����Ăяo��
    void fCreateBossUnit(GraphicsPipeline& Graphics_,
        const DirectX::XMFLOAT3& Position_);
private:
    //--------------------<�G�Ɗ֘A���鏈��>--------------------//
    void fSpawn(GraphicsPipeline& graphics); // �G�̐������Ǘ�
    void fSpawn(EnemySource Source_, GraphicsPipeline& graphics_);
    void fEnemiesUpdate(GraphicsPipeline& Graphics_,float elapsedTime_); // �G�̍X�V����
    void fEnemiesRender(GraphicsPipeline& graphics_); // �G�̕`�揈��

    //--------------------<�G���\�[�g����>--------------------//
    void fSort(std::function<bool(const BaseEnemy* A_, const BaseEnemy* B_)> Function_);

    //--------------------<�Ǘ��N���X���Ŋ������鏈��>--------------------//
    void fAllClear(); // �G��S�폜����֐�

    //--------------------<�G���m�̓����蔻��>--------------------//
    void fCollisionEnemyVsEnemy();


    void fLoad(const char* FileName_);
    //****************************************************************
    //
    // �ϐ�
    //
    //****************************************************************
private:
    std::vector<BaseEnemy*> mEnemyVec;  // �G���i�[����R���e�i
    std::vector<BaseEnemy*> mRemoveVec; // �G���i�[����R���e�i
    // ���\�[�X�}�l�[�W���[������ێ��ł���悤�ɃL���b�V���Ƃ��ă��f���f�[��o�^����
    std::vector<BaseEnemy*> mCashEnemyVec{};

    //--------------------<�E�F�[�u�Ɋւ���ϐ�>--------------------//
    float mWaveTimer{}; // �e�E�F�[�u�J�n����̌o�ߎ���
    int mCurrentWave{}; // ���݂̃E�F�[�u
    // ���݂̃E�F�[�u�̃f�[�^
    std::vector<EnemySource> mCurrentWaveVec{};
    std::vector<EnemySource> mFirstTest{};
    float mLastTimer{};// �Ō�ɏo�Ă���G�̎���
    EnemyEditor mEditor{};

    bool mDebugMode{};
    bool mIsPlayerChainTime = false;
    //--------------------<�v���C���[�̈ʒu>--------------------//
    DirectX::XMFLOAT3 mPlayerPosition{};

    int mUniqueCount{};

    // �J�����V�F�C�N
    float mCameraShakeTime{};
    const float mkOneShakeSec{ 0.15f };


    //****************************************************************
    //
    // �萔
    //
    //****************************************************************

    // �G���m�̊Ԋu�𒲐�����
    const float mAdjustmentEnemies{ 1.0f };
    
    const char* mWaveFileNameArray[6]
    {
        "./resources/Data/Wave1.json",
        "./resources/Data/Wave2.json",
        "./resources/Data/Wave3.json",
        "./resources/Data/Wave4.json",
        "./resources/Data/Wave5.json",
        "./resources/Data/Wave6.json",
    };

    // ImGui
    bool mOpenGuiMenu{};

};
