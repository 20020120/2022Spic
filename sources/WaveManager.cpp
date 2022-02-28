#include"WaveManager.h"
#define ProtoType
void WaveManager::fInitialize(GraphicsPipeline& graphics_)
{
    // 初期化
    mEnemyManager.fInitialize();
}

void WaveManager::fUpdate(GraphicsPipeline& Graphics_ ,float elapsedTime_, AddBulletFunc Func_)
{
    switch (mWaveState)
    {
    case WaveState::Game:

        //mEnemyManager.fUpdate(Graphics_,elapsedTime_);

        break;
    case WaveState::Clear:
        fClearUpdate(elapsedTime_);
        break;
    default: ;
    }

    mEnemyManager.fUpdate(Graphics_,elapsedTime_,Func_);
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
            // ３ウェーブごとにクリアシーンに遷移する
            mWaveState = WaveState::Clear;
        }
        else
        {
            // クリアしたら
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
#ifdef USE_IMGUI
    ImGui::Begin("ClearProto");
    if (ImGui::Button("NextWave"))
    {
        mWaveState = WaveState::Game;
        mCurrentWave++;
        fStartWave();
    }

    ImGui::End();
#endif
}
