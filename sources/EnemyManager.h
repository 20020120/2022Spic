#pragma once
#include"BaseEnemy.h"
#include"EnemyFileSystem.h"
#include"EnemiesEditor.h"
#include<map>
#include<vector>
#include <d3d11.h>

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
    };

    //****************************************************************
    // 
    // �֐�
    //  
    //****************************************************************
public:
    EnemyManager() = default;
    ~EnemyManager() = default;

    void fInitialize(ID3D11Device* pDevice_);
    void fUpdate(float elapsedTime_);
    void fRender(ID3D11DeviceContext* pDeviceContext_);
    void fFinalize();

    //--------------------<ImGui>--------------------//
    void fGuiMenu();
    //--------------------<�E�F�[�u�؂�ւ��֐�>--------------------//
    void fStartWave(int WaveIndex_);
private:
    //--------------------<�G�Ɗ֘A���鏈��>--------------------//
    void fSpawn(); // �G�̐������Ǘ�
    void fSpawn(EnemySource Source_);
    void fEnemiesUpdate(float elapsedTime_); // �G�̍X�V����
    void fEnemiesRender(ID3D11DeviceContext* pDeviceContext_); // �G�̕`�揈��

    //--------------------<�G���o���ꏊ���쐬����>--------------------//
    void fRegisterEmitter();

    //--------------------<�O���t�@�C���Ɋ�����>--------------------//
    void fLoad(const char* FileName_);

    //--------------------<�Ǘ��N���X���Ŋ������鏈��>--------------------//
    void fAllClear(); // �G��S�폜����֐�


    //****************************************************************
    // 
    // �ϐ� 
    // 
    //****************************************************************
private:
    std::vector<BaseEnemy*> mEnemyVec;  // �G���i�[����R���e�i
    std::vector<BaseEnemy*> mRemoveVec; // �G���i�[����R���e�i
    std::map<int, Emitter> mEmitterMap{}; // �G�̏o����

    // �������p�Ƀ|�C���^��ێ�
    ID3D11Device* mpDevice{ nullptr };

    //--------------------<�E�F�[�u�Ɋւ���ϐ�>--------------------//
    float mWaveTimer{}; // �e�E�F�[�u�J�n����̌o�ߎ���
    int mCurrentWave{}; // ���݂̃E�F�[�u
    // ���݂̃E�F�[�u�̃f�[�^
    std::vector<EnemySource> mCurrentWaveVec{};

    EnemyEditor mEditor{};

    //--------------------<�v���C���[�̈ʒu>--------------------//
    DirectX::XMFLOAT3 mPlayerPosition{};

    //****************************************************************
    // 
    // �萔
    // 
    //****************************************************************

    const char* mWaveFileNameArray[1]
    {
        "./resources/Data/Test.json",
    };

    // ImGui
    bool mOpenGuiMenu{};
   
};
