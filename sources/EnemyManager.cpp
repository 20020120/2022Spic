#include"EnemyManager.h"
//#include "EnemyFileSystem.h"
#include"imgui_include.h" 

#include <fstream>


//****************************************************************
// 
// 敵の管理クラス 
// 
//****************************************************************

void EnemyManager::fInitialize(ID3D11Device* pDevice_)
{
    //--------------------<初期化>--------------------//
    fAllClear();


}

void EnemyManager::fUpdate(float elapsedTime_)
{
    //--------------------<管理クラス自体の更新処理>--------------------//

    // ウェーブ開始からの時間を更新
    mWaveTimer += elapsedTime_;


    //--------------------<敵の更新処理>--------------------//
    fEnemiesUpdate(elapsedTime_);


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
    throw std::logic_error("Not implemented");
}


void EnemyManager::fSpawn()
{
}

void EnemyManager::fEnemiesUpdate(float elapsedTime_)
{
    // 更新
    for(const auto enemy: mEnemyVec)
    {
        enemy->fUpdate(elapsedTime_);
    }

    // 削除

}

void EnemyManager::fEnemiesRender(ID3D11DeviceContext* pDeviceContext_)
{
   for(const auto enemy:mEnemyVec)
   {
       enemy->fRender(pDeviceContext_);
   }
}

void EnemyManager::fAllClear()
{
    //--------------------<要素を全削除>--------------------//

    for(const auto enemy: mEnemyVec)
    {
        // 存在していれば削除
        if(enemy)
        {
            delete enemy;
        }
    }
    mEnemyVec.clear();
}



void EnemyManager::fGuiMenu()
{
    ImGui::Begin("EnemyManager");
    ImGui::Text("WaveTimer");
    ImGui::SameLine();
    ImGui::Text(std::to_string(mWaveTimer).c_str());

    if(ImGui::CollapsingHeader("JsonTest"))
    {
        if (ImGui::Button("Create"))
        {
        }
        if (ImGui::Button("Test"))
        {
            std::ofstream ofs;
            ofs.open("./resources/Data/Wave0Enemy.json");
            if (ofs)
            {
               /* nlohmann::json json;
                EnemySource source;
                source.mEmitterNumber = 100;
                source.mType = 0;
                source.mSpawnTimer = 10.0f;
                EnemyJson::to_json(json, source);
                ofs << json;*/
            }
        }

    }

    ImGui::End();
}