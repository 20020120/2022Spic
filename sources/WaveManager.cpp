#include"WaveManager.h"
#define ProtoType
void WaveManager::fInitialize(ID3D11Device* pDevice_)
{
    // ������
    mEnemyManager.fInitialize(pDevice_);
}

void WaveManager::fUpdate(float elapsedTime_)
{
    switch (mWaveState)
    {
    case WaveState::Game:

        mEnemyManager.fUpdate(elapsedTime_);

        break;
    case WaveState::Clear:
        fClearUpdate(elapsedTime_);
        break;
    default: ;
    }

    mEnemyManager.fUpdate(elapsedTime_);
}

void WaveManager::fFinalize()
{
    mEnemyManager.fFinalize();
}

void WaveManager::fWaveClear()
{
    if(mEnemyManager.fGetClearWave())
    {
        if (mCurrentWave % 3 == 2)
        {
            // �R�E�F�[�u���ƂɃN���A�V�[���ɑJ�ڂ���
            mWaveState = WaveState::Clear;
        }
        else
        {
            // �N���A������
            mCurrentWave++;
            fStartWave();
        }
    }
}

void WaveManager::fStartWave()
{
    mEnemyManager.fStartWave(mCurrentWave);
}

 EnemyManager* WaveManager::fGetEnemyManager() 
{
    return &mEnemyManager;
}


void WaveManager::fClearUpdate(float elapsedTime_)
{
    ImGui::Begin("ClearProto");
    if (ImGui::Button("NextWave"))
    {
        mWaveState = WaveState::Game;
        mCurrentWave++;
        fStartWave();
    }

    ImGui::End();
}
