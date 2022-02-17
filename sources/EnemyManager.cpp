#include"EnemyManager.h"
#include "EnemyFileSystem.h"

#include"TestEnemy.h"
#include"imgui_include.h" 
#include <fstream>

#include "user.h"


//****************************************************************
// 
// �G�̊Ǘ��N���X 
// 
//****************************************************************

void EnemyManager::fInitialize(ID3D11Device* pDevice_)
{
    //--------------------<������>--------------------//
    mpDevice = pDevice_;
    fAllClear();
    fRegisterEmitter();


}

void EnemyManager::fUpdate(float elapsedTime_)
{
    //--------------------<�Ǘ��N���X���̂̍X�V����>--------------------//

    // �E�F�[�u�J�n����̎��Ԃ��X�V
    mWaveTimer += elapsedTime_;

    //--------------------<�G�̍X�V����>--------------------//
    fEnemiesUpdate(elapsedTime_);

    //--------------------<�G�̃X�|�i�[>--------------------//
    fSpawn();

    // ImGui�̃��j���[
    fGuiMenu();
}

void EnemyManager::fRender(ID3D11DeviceContext* pDeviceContext_)
{
    //--------------------<�G�̕`�揈��>--------------------//
    fEnemiesRender(pDeviceContext_);
}

void EnemyManager::fFinalize()
{
    fAllClear();
}

void EnemyManager::fSpawn()
{
    int spawnCounts = 0;

    // �G���X�|�[������֐�
    for (const auto data : mCurrentWaveVec)
    {
        // �o�������𖞂����Ă�����o��
        if (data.mSpawnTimer <= mWaveTimer)
        {
            fSpawn(data);
            spawnCounts++;
        }
    }
    // �ǉ�������擪�̃f�[�^������
    for (int i = 0; i < spawnCounts; i++)
    {
        mCurrentWaveVec.erase(mCurrentWaveVec.begin());
    }
}

void EnemyManager::fSpawn(EnemySource Source_)
{
    // �����Ă����f�[�^�����ƂɓG���o��������

     // �o���ʒu������
    const auto point = mEmitterMap.at(Source_.mEmitterNumber);

    switch (Source_.mType)
    {
    case EnemyType::Test:
        mEnemyVec.emplace_back(new TestEnemy(mpDevice, point.fGetPosition()));
        break;
    default:
        _ASSERT_EXPR(0, "Enemy Type No Setting");
        break;
    }

}

void EnemyManager::fEnemiesUpdate(float elapsedTime_)
{
    // �X�V
    for (const auto enemy : mEnemyVec)
    {
        enemy->fUpdate(elapsedTime_);
        enemy->fSetPlayerPosition(mPlayerPosition);
    }
    // �폜


}

void EnemyManager::fEnemiesRender(ID3D11DeviceContext* pDeviceContext_)
{
    for (const auto enemy : mEnemyVec)
    {
        enemy->fRender(pDeviceContext_);
    }
}

void EnemyManager::fRegisterEmitter()
{
    Emitter emitter{};
    emitter.Initialize({ 0.0f,0.0f,0.0f });
    mEmitterMap.insert(std::make_pair(0, emitter));
}

void EnemyManager::fLoad(const char* FileName_)
{
    EnemyFileSystem::fLoadFromJson(mCurrentWaveVec, FileName_);
}

void EnemyManager::fAllClear()
{
    //--------------------<�v�f��S�폜>--------------------//

    for (const auto enemy : mEnemyVec)
    {
        // ���݂��Ă���΍폜
        if (enemy)
        {
            delete enemy;
        }
    }
    mEnemyVec.clear();
}



void EnemyManager::fGuiMenu()
{
    imgui_menu_bar("Game", "EnemyManager", mOpenGuiMenu);

#ifdef USE_IMGUI
    if (mOpenGuiMenu)
    {
        ImGui::Begin("EnemyManager");

        ImGui::Text("WaveNumber");
        ImGui::SameLine();
        ImGui::Text(std::to_string(mCurrentWave).c_str());
        ImGui::Text("WaveTimer");
        ImGui::SameLine();
        ImGui::Text(std::to_string(mWaveTimer).c_str());

        ImGui::Separator();

        ImGui::Text("EnemyValues");
        ImGui::SameLine();
        ImGui::Text(std::to_string(mEnemyVec.size()).c_str());
        ImGui::SameLine();
        ImGui::Text("DataSize");
        ImGui::SameLine();
        ImGui::Text(std::to_string(mCurrentWaveVec.size()).c_str());
        ImGui::Separator();

        if (ImGui::Button("CreateEnemy"))
        {
            DirectX::XMFLOAT3 point{};

            mEnemyVec.emplace_back(new TestEnemy(mpDevice, point));
        }

        ImGui::InputInt("WaveNumber", &mCurrentWave);
        if (ImGui::Button("StartWave"))
        {
            fStartWave(mCurrentWave);
        }

        ImGui::Separator();

        if (ImGui::Button("Close"))
        {
            mOpenGuiMenu = false;
        }
        ImGui::End();
    }
#endif
    mEditor.fGuiMenu();
}

void EnemyManager::fStartWave(int WaveIndex_)
{
    //--------------------<�E�F�[�u���J�n������֐�>--------------------//
    fAllClear();
    mWaveTimer = 0.0f;
    fLoad(mWaveFileNameArray[WaveIndex_]);
}
