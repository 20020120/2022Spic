#pragma once
#include"BaseEnemy.h"
#include"EnemyFileSystem.h"
#include"EnemiesEditor.h"
#include<map>
#include<vector>
#include <d3d11.h>

#include "Common.h"

//****************************************************************
//
// �\����
//
//****************************************************************
class Emitter
{
public:
    void Initialize(DirectX::XMFLOAT3 Position_)
    {
        mPosition = Position_;
    }

    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition()const
    {
        return mPosition;
    }
private:
    DirectX::XMFLOAT3 mPosition{};
};

//****************************************************************
//
// �G�̊Ǘ��N���X
//
//****************************************************************
class EnemyManager final
{
    //****************************************************************
    //
    //  �\����
    //
    //****************************************************************
    // �G�̎��
    enum EnemyType
    {
        Test, // �e�X�g�p�i�{�Ԃł͎g��Ȃ��j
        // �������������������������牺�ɑ��₷��������������������������
        Normal,
        Chase,  // �U���Ȃ����ǐՂ��Ă���
        Archer, //�������U��
        Shield,//������
        MiddleBoss_, // ���{�X
        Sword,// ���̓G 
        Spear,
        // ���������������������������ɑ��₷��������������������������
        Count, // ��ނ̑���
    };

    //****************************************************************
    //
    // �֐�
    //
    //****************************************************************
public:
    EnemyManager() = default;
    ~EnemyManager() = default;

    void fInitialize(GraphicsPipeline& graphics_, AddBulletFunc Func_);
    void fUpdate(GraphicsPipeline & graphics_,float elapsedTime_, AddBulletFunc Func_);
    void fRender(GraphicsPipeline& graphics_);
    void fFinalize();

public:
    //--------------------<�����蔻��>--------------------//

    // �v���C���[�̍U���ƓG�̓����蔻��
    int fCalcPlayerCapsuleVsEnemies(           // �߂�l �F ���������G�̐�
        DirectX::XMFLOAT3 PlayerCapsulePointA_,// �v���C���[�̃J�v�Z���̏��
        DirectX::XMFLOAT3 PlayerCapsulePointB_,// �v���C���[�̃J�v�Z���̏��
        float PlayerCapsuleRadius_,            // �v���C���[�̃J�v�Z���̏��
        int PlayerAttackPower_                 // �v���C���[�̍U����
    );
    // �G�̍U���ƃv���C���[�̓����蔻��
    bool fCalcEnemiesAttackVsPlayer(
        DirectX::XMFLOAT3 PlayerCapsulePointA_,// �v���C���[�̃J�v�Z���̏��
        DirectX::XMFLOAT3 PlayerCapsulePointB_,// �v���C���[�̃J�v�Z���̏��
        float PlayerCapsuleRadius_,             // �v���C���[�̃J�v�Z���̏��
        AddDamageFunc Func_
    );
public:
    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]] const BaseEnemy* fGetNearestEnemyPosition();
    [[nodiscard]] const BaseEnemy* fGetSecondEnemyPosition();
    [[nodiscard]] const bool fGetClearWave() const;
    //--------------------<�Z�b�^�[�֐�>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 Position_);

public:
    //--------------------<ImGui>--------------------//
    void fGuiMenu(GraphicsPipeline& Graphics_, AddBulletFunc Func_);
    //--------------------<�E�F�[�u�؂�ւ��֐�>--------------------//
    void fStartWave(int WaveIndex_);
    //--------------------<�E�F�[�u�N���A�������ǂ����𔻒肷��>--------------------//
    [[nodiscard]] bool fWaveClear() const;

    //--------------------<�G�̎��̂��폜����֐�>--------------------//
    void fDeleteEnemies();

    //--------------------<�L���b�V���Ƀ��f���f�[�^��o�^>--------------------//
    void fRegisterCash(GraphicsPipeline& graphics_, AddBulletFunc Func_);
    void fDeleteCash(); // �L���b�V�����폜
        //--------------------<�L���b�V���f�[�^�ɂ���G�t�F�N�g��~>--------------------//
    void fCashEnemysStopEffect(GraphicsPipeline& graphics_);

private:
    //--------------------<�G�Ɗ֘A���鏈��>--------------------//
    void fSpawn(GraphicsPipeline& graphics, AddBulletFunc Func_); // �G�̐������Ǘ�
    void fSpawn(EnemySource Source_, GraphicsPipeline& graphics_, AddBulletFunc Func_);
    void fEnemiesUpdate(GraphicsPipeline& Graphics_,float elapsedTime_); // �G�̍X�V����
    void fEnemiesRender(GraphicsPipeline& graphics_); // �G�̕`�揈��

    //--------------------<�G���\�[�g����>--------------------//
    void fSort(std::function<bool(const BaseEnemy* A_, const BaseEnemy* B_)> Function_);

    //--------------------<�G���o���ꏊ���쐬����>--------------------//
    void fRegisterEmitter();

    //--------------------<�O���t�@�C���Ɋ�����>--------------------//
    void fLoad(const char* FileName_);
    void fReLoadEnemyParam();
    void fSave();

    //--------------------<�Ǘ��N���X���Ŋ������鏈��>--------------------//
    void fAllClear(); // �G��S�폜����֐�

    //--------------------<�G���m�̓����蔻��>--------------------//
    void fCollisionEnemyVsEnemy();
    
    //--------------------<�v���g>--------------------//
    void fProtoSpawn(GraphicsPipeline& graphics_, AddBulletFunc Func_);

    //****************************************************************
    //
    // �ϐ�
    //
    //****************************************************************
private:
    std::vector<BaseEnemy*> mEnemyVec;  // �G���i�[����R���e�i
    std::vector<BaseEnemy*> mRemoveVec; // �G���i�[����R���e�i
    std::map<int, Emitter> mEmitterMap{}; // �G�̏o����
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

    //--------------------<�v���C���[�̈ʒu>--------------------//
    DirectX::XMFLOAT3 mPlayerPosition{};

    int mUniqueCount{};

    //--------------------<�v���g>--------------------//
    bool mIsProtoSpawn{ true };
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
