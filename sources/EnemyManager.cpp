#include"EnemyManager.h"
#include "EnemyFileSystem.h"

#include"TestEnemy.h"
#include"imgui_include.h" 
#include <fstream>

#include "user.h"


//****************************************************************
// 
// 敵の管理クラス 
// 
//****************************************************************

void EnemyManager::fInitialize(ID3D11Device* pDevice_)
{
    //--------------------<初期化>--------------------//
    mpDevice = pDevice_;
    fAllClear();
    fRegisterEmitter();


}

void EnemyManager::fUpdate(float elapsedTime_)
{
    //--------------------<管理クラス自体の更新処理>--------------------//

    // ウェーブ開始からの時間を更新
    mWaveTimer += elapsedTime_;

    //--------------------<敵の更新処理>--------------------//
    fEnemiesUpdate(elapsedTime_);

    //--------------------<敵のスポナー>--------------------//
    fSpawn();

    // ImGuiのメニュー
    fGuiMenu();
}

void EnemyManager::fRender(ID3D11DeviceContext* pDeviceContext_)
{
    //--------------------<敵の描画処理>--------------------//
    fEnemiesRender(pDeviceContext_);
}

void EnemyManager::fFinalize()
{
    fAllClear();
}

void EnemyManager::fSpawn()
{
    int spawnCounts = 0;

    // 敵をスポーンする関数
    for (const auto data : mCurrentWaveVec)
    {
        // 出現条件を満たしていたら出す
        if (data.mSpawnTimer <= mWaveTimer)
        {
            fSpawn(data);
            spawnCounts++;
        }
    }
    // 追加したら先頭のデータを消す
    for (int i = 0; i < spawnCounts; i++)
    {
        mCurrentWaveVec.erase(mCurrentWaveVec.begin());
    }
}

void EnemyManager::fSpawn(EnemySource Source_)
{
    // 送られてきたデータをもとに敵を出現させる

     // 出現位置を決定
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
    // 更新
    for (const auto enemy : mEnemyVec)
    {
        enemy->fUpdate(elapsedTime_);
        enemy->fSetPlayerPosition(mPlayerPosition);
    }
    // 削除


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
    //--------------------<要素を全削除>--------------------//

    for (const auto enemy : mEnemyVec)
    {
        // 存在していれば削除
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
    //--------------------<ウェーブを開始させる関数>--------------------//
    fAllClear();
    mWaveTimer = 0.0f;
    fLoad(mWaveFileNameArray[WaveIndex_]);
}
