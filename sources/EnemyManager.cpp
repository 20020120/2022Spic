#include"EnemyManager.h"
#include "EnemyFileSystem.h"

#include"TestEnemy.h"
#include"NormalEnemy.h"
#include"ChaseEnemy.h"
#include"imgui_include.h" 
#include "user.h"
#include"collision.h"
#include"Operators.h"
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

    //--------------------<�G���m�̓����蔻��>--------------------//
    fCollisionEnemyVsEnemy();

    //--------------------<�G�̃X�|�i�[>--------------------//
    // fSpawn();
    fProtoSpawn();
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
    //--------------------<�v���C���[�̍U���ƓG�̓����蔻��>--------------------//
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
                enemy->fDamaged(PlayerAttackPower_,0.3f);
                hitCounts++;
            }
        }
    }

    return hitCounts;
}

bool EnemyManager::fCalcEnemiesAttackVsPlayer(DirectX::XMFLOAT3 PlayerCapsulePointA_,
    DirectX::XMFLOAT3 PlayerCapsulePointB_, float PlayerCapsuleRadius_)
{
    //--------------------<�v���C���[�ƓG�̍U���̓����蔻��>--------------------//

    for (const auto enemy : mEnemyVec)
    {
        // �����蔻������邩�m�F
        if (enemy->fGetIsFrustum())
        {
            BaseEnemy::CapsuleCollider capsule = enemy->fGetCapsuleData();

            const bool result = Collision::capsule_vs_capsule(
                PlayerCapsulePointA_, PlayerCapsulePointB_, PlayerCapsuleRadius_,
                capsule.mPointA, capsule.mPointB, capsule.mRadius);

            // �������Ă�����
            if (result)
            {

            }
        }
    }
    return false;
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
    auto func = [](const BaseEnemy* A_, const BaseEnemy* B_)->bool
    {
        return A_->fGetLengthFromPlayer() < B_->fGetLengthFromPlayer();
    };
    fSort(func);
    for (const auto enemy : mEnemyVec)
    {
        if (enemy->fGetIsFrustum())
        {
            // ���̓G����̋������v�Z����
            for (const auto enemy2 : mEnemyVec)
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

const bool EnemyManager::fGetClearWave() const
{
    // �c��f�[�^���O���t�B�[���h�ɓG���c���Ă��Ȃ�
    return (mCurrentWaveVec.size() <= 0 && mEnemyVec.size() <= 0);
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
    {
        auto enemy = new TestEnemy(mpDevice, point.fGetPosition(), mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
    }
        break;
    case EnemyType::Normal:
    {
        auto enemy = new NormalEnemy(mpDevice, point.fGetPosition(), mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
    }
        break;
    case EnemyType::Chase :
    {
        auto enemy = new ChaseEnemy(mpDevice, point.fGetPosition(), mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
    }
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

void EnemyManager::fReLoadEnemyParam()
{
    for(const auto enemy: mEnemyVec)
    {
        enemy->fGetParam(enemy, mEditor.fGetFunction());
    }
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

void EnemyManager::fCollisionEnemyVsEnemy()
{
    for(const auto enemy1 :mEnemyVec)
    {
        for (const auto enemy2 : mEnemyVec)
        {
            // �������g�Ƃ͔��肵�Ȃ�
            if (enemy1 == enemy2)
            {
                continue;
            }
            const auto capsule1 = enemy1->fGetCapsuleData();
            const auto capsule2 = enemy2->fGetCapsuleData();
            // �������a���O�ȉ��Ȃ�v�Z���Ȃ�
            if(capsule1.mRadius<=0.0f||capsule2.mRadius<=0.0f)
            {
                continue;
            }

            const bool result=Collision::capsule_vs_capsule(
                capsule1.mPointA, capsule1.mPointB, capsule1.mRadius,
                capsule2.mPointA, capsule2.mPointB, capsule2.mRadius);

            // ��������������
            if(result)
            {
                // ���������G�Ԃ̃x�N�g�����Z�o����
                DirectX::XMFLOAT3 vec = enemy2->fGetPosition() - enemy1->fGetPosition();
                if (Math::Length(vec) <= 0.0f)
                {
                    vec = { 0.0f,0.0f,1.0f };
                }
                // ���K��
                vec = Math::Normalize(vec);
                // ��̂̂߂荞��ł��鋗�����v�Z����

                // ��̂̔��a�̍��v
                const float radiusAdd = capsule1.mRadius + capsule2.mRadius;
                // ��̂̋���
                float length = Math::Length(
                    enemy1->fGetPosition() - enemy2->fGetPosition());
                if(length<=0.0f)
                {
                    length = 0.1f;
                }
                // �߂荞�݋���
                const float raidLength = radiusAdd - length;
                DirectX::XMFLOAT3 res = enemy2->fGetPosition() + (vec * raidLength);
                enemy2->fSetPosition(res);
            }

        }
    }
}

void EnemyManager::fProtoSpawn()
{
    if (!mIsProtoSpawn) return;

    static bool  IsSpawn{};
    const int separateTime = static_cast<int>(mWaveTimer) % 3;
    if(separateTime==1&&!IsSpawn)
    {
        auto enemy = new ChaseEnemy(mpDevice, {0.0f,0.0f,10.0f}, mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
        auto enemy1 = new NormalEnemy(mpDevice, { 0.0f,0.0f,10.0f }, mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy1);
        IsSpawn = true;
    }
    else if(separateTime == 2)
    {
        IsSpawn = false;
    }
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
        if(ImGui::Button("ReLoadEnemies"))
        {
            fReLoadEnemyParam();
        }

        ImGui::Separator();
        static int elem = EnemyType::Test;
        const char* elems_names[EnemyType::Count] = { "Test","Normal","Chase"};
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

        ImGui::Checkbox("ProtoSpawn", &mIsProtoSpawn);
        if (ImGui::Button("AllClear"))
        {
            fAllClear();
        }


        if (ImGui::Button("Close"))
        {
            mOpenGuiMenu = false;
        }
        ImGui::End();
    }
#endif
    mEditor.fGuiMenu();
    mEditor.fGui_ParamEditor();
}

void EnemyManager::fStartWave(int WaveIndex_)
{
    //--------------------<�E�F�[�u���J�n������֐�>--------------------//
    fAllClear();
    mWaveTimer = 0.0f;
    fLoad(mWaveFileNameArray[WaveIndex_]);
}
