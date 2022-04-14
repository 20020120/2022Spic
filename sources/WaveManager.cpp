#include"WaveManager.h"
#define ProtoType
void WaveManager::fInitialize(GraphicsPipeline& graphics_,AddBulletFunc Func_)
{
    // ������
    mEnemyManager.fInitialize(graphics_,Func_);

    mWaveState = WaveState::Start;
    fSetStartGame(true);
}

void WaveManager::fUpdate(GraphicsPipeline& Graphics_ ,float elapsedTime_, AddBulletFunc Func_)
{
    switch (mWaveState)
    {
    case WaveState::Start:
        // �Q�[���J�n���o���I���������ǂ���
        if (mStartGame)
        {
            mWaveState = WaveState::Game;
            mEnemyManager.fStartWave(0);
            mStartGame = false;
        }
        break;
    case WaveState::Game:
        mEnemyManager.fUpdate(Graphics_,elapsedTime_,Func_);

        // �N���A��ԂɑJ��
        if(mEnemyManager.fGetClearWave())
        {
            mWaveState = WaveState::Clear;
        }
        
        break;
    case WaveState::Clear:
        fClearUpdate(elapsedTime_);
        break;
    default: ;
    }
    fGuiMenu();
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

void WaveManager::fSetStartGame(bool Arg_)
{
    mStartGame = Arg_;
}

void WaveManager::fGuiMenu()
{
    imgui_menu_bar("System", "WaveManager", mOpenGui);

#ifdef USE_IMGUI
    if (mOpenGui)
    {
        ImGui::Begin("WaveManager");
        if (ImGui::Button("StartGame"))
        {
            fSetStartGame(true);
        }

        ImGui::Text("State ");
        ImGui::SameLine();
        switch (mWaveState) {
        case WaveState::Start:
            ImGui::Text("Start");
            break;
        case WaveState::Game:
            ImGui::Text("Game");
            break;
        case WaveState::Clear:
            ImGui::Text("Clear");
            break;
        default:;
        }

        ImGui::End();

    }
#endif


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
    if(ImGui::Button("StartGame"))
    {
        fSetStartGame(true);
    }
    ImGui::End();
#endif
}
