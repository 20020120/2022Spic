#include"EnemyManager.h"
#include "EnemyFileSystem.h"

#include"TestEnemy.h"
#include"NormalEnemy.h"
#include"ChaseEnemy.h"
#include"ArcherEnemy.h"
#include"FrontShieldEnemy.h"
#include"MiddleBoss.h"
#include"SwordEnemy.h"
#include"SpearEnemy.h"

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

void EnemyManager::fInitialize(GraphicsPipeline& graphics_, AddBulletFunc Func_)
{
    //--------------------<������>--------------------//
    fAllClear();
    fRegisterEmitter();
    mUniqueCount = 0;

    // �L���b�V���ɓo�^
    fRegisterCash(graphics_,Func_);
}

void EnemyManager::fUpdate(GraphicsPipeline& graphics_, float elapsedTime_,AddBulletFunc Func_)
{
    //--------------------<�Ǘ��N���X���̂̍X�V����>--------------------//

    // �E�F�[�u�J�n����̎��Ԃ��X�V
    mWaveTimer += elapsedTime_;

    //--------------------<�G�̍X�V����>--------------------//
    fEnemiesUpdate(graphics_,elapsedTime_);

    //--------------------<�G���m�̓����蔻��>--------------------//
    fCollisionEnemyVsEnemy();

    //--------------------<�G�̃X�|�i�[>--------------------//
    fSpawn(graphics_, Func_);
    // ImGui�̃��j���[
    fGuiMenu(graphics_,Func_);
}

void EnemyManager::fRender(GraphicsPipeline& graphics_)
{
    //--------------------<�G�̕`�揈��>--------------------//
    fEnemiesRender(graphics_);
}

void EnemyManager::fFinalize()
{
    fAllClear();
    fDeleteCash();
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
    DirectX::XMFLOAT3 PlayerCapsulePointB_, float PlayerCapsuleRadius_, AddDamageFunc Func_)
{
    //--------------------<�v���C���[�ƓG�̍U���̓����蔻��>--------------------//

    for (const auto enemy : mEnemyVec)
    {
        // �����蔻������邩�m�F
        if (enemy->fGetIsFrustum())
        {
            if (enemy->fGetAttacking())
            {
                BaseEnemy::CapsuleCollider capsule = enemy->fGetAttackCapsuleData();

                const bool result = Collision::capsule_vs_capsule(
                    PlayerCapsulePointA_, PlayerCapsulePointB_, PlayerCapsuleRadius_,
                    capsule.mPointA, capsule.mPointB, capsule.mRadius);

                // �������Ă�����
                if (result)
                {
                    Func_(enemy->fGetAttackInfo().mDamage, enemy->fGetAttackInfo().mInvincible_time);
                    return  true;
                }
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

void EnemyManager::fSpawn(GraphicsPipeline& graphics, AddBulletFunc Func_)
{
    int spawnCounts = 0;

    // �G���X�|�[������֐�
    for (const auto data : mCurrentWaveVec)
    {
        // �o�������𖞂����Ă�����o��
        if (data.mSpawnTimer <= mWaveTimer)
        {
            fSpawn(data, graphics,Func_);
            spawnCounts++;
        }
    }
    // �ǉ�������擪�̃f�[�^������
    for (int i = 0; i < spawnCounts; i++)
    {
        mCurrentWaveVec.erase(mCurrentWaveVec.begin());
    }
}

void EnemyManager::fSpawn(EnemySource Source_, GraphicsPipeline& graphics_,AddBulletFunc Func_)
{
    // �����Ă����f�[�^�����ƂɓG���o��������

    switch (Source_.mType)
    {
    case EnemyType::Test:
    {
        auto enemy = new TestEnemy(graphics_, Source_.mEmitterPoint, mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Normal:
    {
        auto enemy = new NormalEnemy(graphics_, Source_.mEmitterPoint, mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Chase:
    {
        auto enemy = new ChaseEnemy(graphics_, Source_.mEmitterPoint, mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Archer:
    {
        auto enemy = new ArcherEnemy(graphics_, Source_.mEmitterPoint, mUniqueCount, mEditor.fGetFunction(), Func_);
        mEnemyVec.emplace_back(enemy);
    }
    case EnemyType::Shield:
    {
        auto enemy = new ArcherEnemy(graphics_, Source_.mEmitterPoint, mUniqueCount, mEditor.fGetFunction(), Func_);
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::MiddleBoss_:
    {
        auto enemy = new MiddleBoss(graphics_, mEditor.fGetFunction(),Func_);
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Sword :
    {
        auto enemy = new SwordEnemy(graphics_, mUniqueCount, Source_.mEmitterPoint,
            mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
    }
        break;

    case EnemyType::Spear :
    {
        auto enemy = new SpearEnemy(graphics_, mUniqueCount, Source_.mEmitterPoint,
            mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
    }
    break;
    default:
        _ASSERT_EXPR(0, "Enemy Type No Setting");
        break;
    }
    mUniqueCount++;
}

void EnemyManager::fEnemiesUpdate(GraphicsPipeline& Graphics_,float elapsedTime_)
{
    // �X�V
    for (const auto enemy : mEnemyVec)
    {
        if (enemy->fGetIsAlive())
        {
            enemy->fSetPlayerPosition(mPlayerPosition);
            enemy->fUpdate(Graphics_,elapsedTime_);
        }
        else
        {
            mRemoveVec.emplace_back(enemy);
        }
    }


}

void EnemyManager::fEnemiesRender(GraphicsPipeline& graphics_)
{
    for (const auto enemy : mEnemyVec)
    {
        enemy->fRender(graphics_);
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

void EnemyManager::fSave()
{
    EnemySource source{};
    std::vector<EnemySource> vec{};
    vec.emplace_back(source);
    EnemyFileSystem::fSaveToJson(vec, "Wave1.json");
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

void EnemyManager::fProtoSpawn(GraphicsPipeline& graphics_, AddBulletFunc Func_)
{
    if (!mIsProtoSpawn) return;
    if (mEnemyVec.size() > 10) return;

    static bool  IsSpawn{};
    const int separateTime = static_cast<int>(mWaveTimer) % 4;
    if(separateTime==1&&!IsSpawn)
    {
        auto enemy = new FrontShieldEnemy(graphics_, { 0.0f,0.0f,10.0f }, mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy);
        auto enemy1 = new NormalEnemy(graphics_, { 0.0f,0.0f,10.0f }, mUniqueCount, mEditor.fGetFunction());
        mEnemyVec.emplace_back(enemy1);
        IsSpawn = true;
    }
    else if(separateTime == 2)
    {
        IsSpawn = false;
    }
}


void EnemyManager::fGuiMenu(GraphicsPipeline& Graphics_, AddBulletFunc Func_)
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
        const char* elems_names[EnemyType::Count] = { "Test","Normal","Chase","Archer","Shield","MiddleBoss","SwordEnemy","SpearEnemy"};
        const char* elem_name = (elem >= 0 && elem < EnemyType::Count) ? elems_names[elem] : "Unknown";
        ImGui::SliderInt("slider enum", &elem, 0, EnemyType::Count - 1, elem_name);

        if (ImGui::Button("CreateEnemy"))
        {
            EnemySource source{};
            source.mEmitterPoint = {};
            source.mType = elem;
            fSpawn(source,Graphics_,Func_);
        }

        if(ImGui::Button("ss"))
        {
            fSave();
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

        ImGui::Text("LastTimer");
        ImGui::SameLine();
        ImGui::Text(std::to_string(mLastTimer).c_str());
        ImGui::RadioButton("ClearWave", fWaveClear());

        if (ImGui::Button("AddWaveTimer"))
        {
            mWaveTimer += 10.0f;
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

    // ���b�ł��̃E�F�[�u���I��邩������������
    mLastTimer = mCurrentWaveVec.back().mSpawnTimer;
}

bool EnemyManager::fWaveClear() const
{
    // �Ō�̓G���o�����Ă��邩�𔻒肷��
    if (mLastTimer >= mWaveTimer)
    {
        return false;
    }
    // ���ׂĂ̓G������ł�����
    if (mEnemyVec.size() > 0)
    {
        return false;
    }
    return true;
}

void EnemyManager::fDeleteEnemies()
{
    // �폜
    for (const auto enemy : mRemoveVec)
    {
        auto e = std::find(mEnemyVec.begin(), mEnemyVec.end(), enemy);
        if (e != mEnemyVec.end())
        {
            safe_delete(*e);
            mEnemyVec.erase(e);
        }
    }
    mRemoveVec.clear();
}

void EnemyManager::fRegisterCash(GraphicsPipeline& graphics_, AddBulletFunc Func_)
{
    // �L���b�V���Ƀ��f����o�^
    BaseEnemy* enemy = 
        new SwordEnemy(graphics_, mUniqueCount, { 0.0f,0.0f,0.0f }, mEditor.fGetFunction());
    mCashEnemyVec.emplace_back(enemy);
    enemy = new SpearEnemy(graphics_, mUniqueCount, { 0.0f,0.0f,0.0f }, mEditor.fGetFunction());
    mCashEnemyVec.emplace_back(enemy);
}

void EnemyManager::fDeleteCash()
{

    for (const auto enemy : mCashEnemyVec)
    {
        // ���݂��Ă���΍폜
        if (enemy)
        {
            delete enemy;
        }
    }
    mCashEnemyVec.clear();
}
