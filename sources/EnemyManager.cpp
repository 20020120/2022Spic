#include"EnemyManager.h"
#include "EnemyFileSystem.h"

#include"TestEnemy.h"
#include"NormalEnemy.h"
#include"imgui_include.h" 
#include "user.h"
#include"collision.h"

#include <fstream>


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
    mUniqueCount = 0;
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
int EnemyManager::fCalcPlayerCapsuleVsEnemies(DirectX::XMFLOAT3 PlayerCapsulePointA_,
    DirectX::XMFLOAT3 PlayerCapsulePointB_, float PlayerCapsuleRadius_, int PlayerAttackPower_)
{
    //--------------------<プレイヤーと敵の攻撃の当たり判定>--------------------//
    // 攻撃が何体の敵に当たったか
    int  hitCounts = 0;

    for(const auto enemy: mEnemyVec)
    {
        // 当たり判定をするか確認
        if(enemy->fGetIsFrustum())
        {
            BaseEnemy::CapsuleCollider capsule = enemy->fGetCapsuleData();

            const bool result = Collision::capsule_vs_capsule(
                PlayerCapsulePointA_, PlayerCapsulePointB_, PlayerCapsuleRadius_,
                capsule.mPointA, capsule.mPointB, capsule.mRadius);

            // 当たっていたら
            if(result)
            {
                enemy->fDamaged(PlayerAttackPower_);
                hitCounts++;
            }
        }
    }

    return hitCounts;
}

const BaseEnemy* EnemyManager::fGetNearestEnemyPosition()
{
    auto func = [](const BaseEnemy* A_, const BaseEnemy* B_)->bool
    {
        return A_->fGetLengthFromPlayer() < B_->fGetLengthFromPlayer();
    };
    fSort(func);
    for(const auto enemy :mEnemyVec)
    {
        if(enemy->fGetIsFrustum())
        {
            // この敵からの距離を計算する
            for(const auto enemy2:mEnemyVec)
            {
                if (enemy2->fGetIsFrustum())
                {
                    if (enemy != enemy2)
                    {
                        enemy2->fCalcNearestEnemy(enemy->fGetPosition());
                    }
                }
            }
            return enemy;
        }
    }

    return nullptr;
}

const BaseEnemy* EnemyManager::fGetSecondEnemyPosition()
{

    return nullptr;
}

void EnemyManager::fSetPlayerPosition(DirectX::XMFLOAT3 Position_)
{
    mPlayerPosition = Position_;
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
        mEnemyVec.emplace_back(new TestEnemy(mpDevice, point.fGetPosition(), mUniqueCount));
        break;
    case EnemyType::Normal:
        mEnemyVec.emplace_back(new NormalEnemy(mpDevice, point.fGetPosition(), mUniqueCount));
        break;
    default:
        _ASSERT_EXPR(0, "Enemy Type No Setting");
        break;
    }
    mUniqueCount++;
}

void EnemyManager::fEnemiesUpdate(float elapsedTime_)
{
    // 更新
    for (const auto enemy : mEnemyVec)
    {
        if (enemy->fGetIsAlive())
        {
            enemy->fSetPlayerPosition(mPlayerPosition);
            enemy->fUpdate(elapsedTime_);
        }
        else
        {
            mRemoveVec.emplace_back(enemy);
        }
    }
    // 削除
    for(const auto enemy: mRemoveVec)
    {
        auto e=std::find(mEnemyVec.begin(), mEnemyVec.end(), enemy);
        if(e!=mEnemyVec.end())
        {
            safe_delete(*e);
            mEnemyVec.erase(e);
        }
    }
    mRemoveVec.clear();

}

void EnemyManager::fEnemiesRender(ID3D11DeviceContext* pDeviceContext_)
{
    for (const auto enemy : mEnemyVec)
    {
        enemy->fRender(pDeviceContext_);
    }
}

void EnemyManager::fSort(std::function<bool(const BaseEnemy* A_, const BaseEnemy* B_)> Function_)
{
    // プレイヤーとの距離順に敵をソート
    std::sort(mEnemyVec.begin(), mEnemyVec.end(), Function_);
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

        ImGui::Separator();
        static int elem = EnemyType::Test;
        const char* elems_names[EnemyType::Count] = { "Test","Normal"};
        const char* elem_name = (elem >= 0 && elem < EnemyType::Count) ? elems_names[elem] : "Unknown";
        ImGui::SliderInt("slider enum", &elem, 0, EnemyType::Count - 1, elem_name);

        if (ImGui::Button("CreateEnemy"))
        {
            EnemySource source{};
            source.mEmitterNumber = 0;
            source.mType = elem;
            fSpawn(source);
        }

        ImGui::InputInt("WaveNumber", &mCurrentWave);
        if (ImGui::Button("StartWave"))
        {
            fStartWave(mCurrentWave);
        }

        ImGui::Separator();
        if (ImGui::CollapsingHeader("List"))
        {
            auto func = [=](const BaseEnemy* A_, const BaseEnemy* B_)->bool
            {
                return A_->fGetUniqueId() < B_->fGetUniqueId();
            };
            std::sort(mEnemyVec.begin(), mEnemyVec.end(), func);

            int index = 0;
            for (const auto enemy : mEnemyVec)
            {
                enemy->fGuiMenu();
                index++;
            }
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
