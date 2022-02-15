#pragma once
#include"BaseEnemy.h"

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
    void Initialize();

    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition();
private:
    DirectX::XMFLOAT3 mPosition{};
};

struct EnemySource
{
    float mSpawnTimer{};  // �o���^�C�~���O���L�^
    int mEmitterNumber{}; // �o���ʒu�̔ԍ�
    int mType{};          // �G�̎��
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

private:
    //--------------------<�G�Ɗ֘A���鏈��>--------------------//
    void fSpawn(); // �G�̐������Ǘ�
    void fEnemiesUpdate(float elapsedTime_); // �G�̍X�V����
    void fEnemiesRender(ID3D11DeviceContext* pDeviceContext_); // �G�̕`�揈��

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
    const ID3D11Device* mpDevice{ nullptr };

    // �e�E�F�[�u�J�n����̌o�ߎ���
    float mWaveTimer{};
};
