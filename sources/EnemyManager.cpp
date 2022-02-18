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
// �G�̊Ǘ��N���X 
// 
//****************************************************************

void EnemyManager::fInitialize(ID3D11Device* pDevice_)
{
    //--------------------<������>--------------------//
    mpDevice = pDevice_;
    fAllClear();
    fRegisterEmitter();
    mUniqueCount = 0;
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
int EnemyManager::fCalcPlayerCapsuleVsEnemies(DirectX::XMFLOAT3 PlayerCapsulePointA_,
    DirectX::XMFLOAT3 PlayerCapsulePointB_, float PlayerCapsuleRadius_, int PlayerAttackPower_)
{
    //--------------------<�v���C���[�ƓG�̍U���̓����蔻��>--------------------//
    // �U�������̂̓G�ɓ���������
    int  hitCounts = 0;

    for(const auto enemy: mEnemyVec)
    {
        // �����蔻������邩�m�F
        if(enemy->fGetIsFrustum())
        {
            BaseEnemy::CapsuleCollider capsule = enemy->fGetCapsuleData();

            const bool result = Collision::capsule_vs_capsule(
                PlayerCapsulePointA_, PlayerCapsulePointB_, PlayerCapsuleRadius_,
                capsule.mPointA, capsule.mPointB, capsule.mRadius);

            // �������Ă�����
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
            // ���̓G����̋������v�Z����
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
    // �X�V
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
    // �폜
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
    // �v���C���[�Ƃ̋������ɓG���\�[�g
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
    //--------------------<�E�F�[�u���J�n������֐�>--------------------//
    fAllClear();
    mWaveTimer = 0.0f;
    fLoad(mWaveFileNameArray[WaveIndex_]);
}
