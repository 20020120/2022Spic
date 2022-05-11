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
#include"SpearEnemy_Ace.h"
#include"LastBoss.h"
#include"TutorialEnemy.h"
#include"SwordEnemyAce.h"
#include"BossUnit.h"

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

EnemyManager::~EnemyManager()
{
}

void EnemyManager::fInitialize(GraphicsPipeline& graphics_, AddBulletFunc Func_)
{
    //--------------------<������>--------------------//
    fAllClear();
    mUniqueCount = 0;

    // �L���b�V���ɓo�^
    fRegisterCash(graphics_);
}

void EnemyManager::fUpdate(GraphicsPipeline& graphics_, float elapsedTime_,AddBulletFunc Func_)
{
    //--------------------<�Ǘ��N���X���̂̍X�V����>--------------------//

    // �E�F�[�u�J�n����̎��Ԃ��X�V
    if (!mDebugMode)
    {
        //mWaveTimer += elapsedTime_;
    }

    //--------------<�v���C���[���`�F�C�����̓G�l�~�[�̍s�������ׂĒ�~������>-------------//
    if(mIsPlayerChainTime)
    {
        return;
    }

    //--------------------<�G�̍X�V����>--------------------//
    fEnemiesUpdate(graphics_,elapsedTime_);

    //--------------------<�G���m�̓����蔻��>--------------------//
    fCollisionEnemyVsEnemy();

    //--------------------<�G�̃X�|�i�[>--------------------//
    fSpawn(graphics_);
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

int EnemyManager::fCalcPlayerAttackVsEnemies(DirectX::XMFLOAT3 PlayerCapsulePointA_,
    DirectX::XMFLOAT3 PlayerCapsulePointB_, float PlayerCapsuleRadius_, int PlayerAttackPower_)
{
    //--------------------<�v���C���[�̍U���ƓG�̓����蔻��>--------------------//
      // �U�������̂̓G�ɓ���������
    int  hitCounts = 0;

    for (const auto enemy : mEnemyVec)
    {
        // �����蔻������邩�m�F
        if (enemy->fComputeAndGetIntoCamera())
        {
            Capsule capsule = enemy->fGetBodyCapsule();

            const bool result = Collision::capsule_vs_capsule(
                PlayerCapsulePointA_, PlayerCapsulePointB_, PlayerCapsuleRadius_,
                capsule.mTop, capsule.mBottom, capsule.mRadius);

            // �������Ă�����
            if (result)
            {
                if(enemy->fDamaged(PlayerAttackPower_, 2.3f))
                {
                    hitCounts++;
                }
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
        if (enemy->fComputeAndGetIntoCamera())
        {
            if (enemy->fGetAttack())
            {
                Capsule capsule = enemy->fGetAttackCapsule();

                const bool result = Collision::capsule_vs_capsule(
                    PlayerCapsulePointA_, PlayerCapsulePointB_, PlayerCapsuleRadius_,
                    capsule.mTop, capsule.mBottom, capsule.mRadius);

                // �������Ă�����
                if (result)
                {
                    Func_(enemy->fGetAttackPower(), enemy->fGetAttackInvTime());
                    return  true;
                }
            }
        }
    }
    return false;
}

void EnemyManager::fCalcPlayerStunVsEnemyBody(const DirectX::XMFLOAT3 PlayerPosition_, float Radius_)
{
    
    if(Radius_<=0.0f)
    {
        return;
    }
    // ��������Ńv���C���[����X�^�������������󂯂�
    for(const auto & enemy: mEnemyVec)
    {
        const DirectX::XMFLOAT3 enemyPosition = enemy->fGetPosition();
        const float enemyRad = enemy->fGetBodyCapsule().mRadius;
        if(Math::Length(mPlayerPosition-enemyPosition) <= Radius_+enemyRad)
        {
            enemy->fSetStun(true);
        }
    }
}

BaseEnemy* EnemyManager::fGetNearestStunEnemy()
{
    BaseEnemy* pEnemy{ nullptr };
    float lengthVsPlayer{ FLT_MAX };
    for(int i=0;i<mEnemyVec.size();++i)
    {
        if(mEnemyVec[i]->fGetStun())
        {
            if(lengthVsPlayer>mEnemyVec[i]->fGetLengthFromPlayer())
            {
                lengthVsPlayer = mEnemyVec[i]->fGetLengthFromPlayer();
                pEnemy = mEnemyVec[i];
            }
        }
    }
    return pEnemy;

}

BaseEnemy* EnemyManager::fGetNearestEnemyPosition()
{
    auto func = [](const BaseEnemy* A_, const BaseEnemy* B_)->bool
    {
        return A_->fGetLengthFromPlayer() < B_->fGetLengthFromPlayer();
    };
    fSort(func);
    for(const auto enemy :mEnemyVec)
    {
        if(enemy->fComputeAndGetIntoCamera())
        {
            // ���̓G����̋������v�Z����
            for(const auto enemy2:mEnemyVec)
            {
                if (enemy2->fComputeAndGetIntoCamera())
                {
                    if (enemy != enemy2)
                    {
                        //enemy2->fCalcNearestEnemy(enemy->fGetPosition());
                    }
                }
            }
            return enemy;
        }
    }

    return nullptr;
}

BaseEnemy* EnemyManager::fGetSecondEnemyPosition()
{
    auto func = [](const BaseEnemy* A_, const BaseEnemy* B_)->bool
    {
        return A_->fGetLengthFromPlayer() < B_->fGetLengthFromPlayer();
    };
    fSort(func);
    for (const auto enemy : mEnemyVec)
    {
        if (enemy->fComputeAndGetIntoCamera())
        {
            // ���̓G����̋������v�Z����
            for (const auto enemy2 : mEnemyVec)
            {
                if (enemy2->fComputeAndGetIntoCamera())
                {
                    if (enemy != enemy2)
                    {
                        //enemy2->fCalcNearestEnemy(enemy->fGetPosition());
                    }
                }
            }
            return enemy;
        }
    }

    return nullptr;
}

bool EnemyManager::fGetClearWave() const
{
    // �c��f�[�^���O���t�B�[���h�ɓG���c���Ă��Ȃ�
    return (mCurrentWaveVec.size() <= 0 && mEnemyVec.size() <= 0);
}



void EnemyManager::fSetIsPlayerChainTime(bool IsChain_)
{
    mIsPlayerChainTime = IsChain_;
}

void EnemyManager::fSetPlayerPosition(DirectX::XMFLOAT3 Position_)
{
    mPlayerPosition = Position_;
    for(const auto& enemy:mEnemyVec)
    {
        enemy->fSetPlayerPosition(Position_);
    }
}

void EnemyManager::fSpawn(GraphicsPipeline& graphics)
{
    int spawnCounts = 0;

    // �G���X�|�[������֐�
    for (const auto data : mCurrentWaveVec)
    {
        // �o�������𖞂����Ă�����o��
        if (data.mSpawnTimer <= mWaveTimer)
        {
            fSpawn(data, graphics);
            spawnCounts++;
        }
    }
    // �ǉ�������擪�̃f�[�^������
    for (int i = 0; i < spawnCounts; i++)
    {
        mCurrentWaveVec.erase(mCurrentWaveVec.begin());
    }
}

void EnemyManager::fSpawn(EnemySource Source_, GraphicsPipeline& graphics_)
{
    // �����Ă����f�[�^�����ƂɓG���o��������
    const auto param = mEditor.fGetParam(Source_.mType);
    switch (Source_.mType)
    {
    case EnemyType::Archer:
    {
        BaseEnemy* enemy = new ArcherEnemy(graphics_,
            Source_.mEmitterPoint, param);
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Shield:
    {
        BaseEnemy* enemy = new ShieldEnemy(graphics_,
            Source_.mEmitterPoint, param);
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Sword:
    {
        BaseEnemy* enemy = new SwordEnemy(graphics_,
            Source_.mEmitterPoint, param);
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Spear:
    {
        BaseEnemy* enemy = new SpearEnemy(graphics_,
            Source_.mEmitterPoint,
            mEditor.fGetParam(Source_.mType));
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Archer_Ace:
        break;
    case EnemyType::Shield_Ace:
        break;
    case EnemyType::Sword_Ace:
    {
        BaseEnemy* enemy = new SwordEnemy_Ace(graphics_,
            Source_.mEmitterPoint,
            mEditor.fGetParam(Source_.mType));
        mEnemyVec.emplace_back(enemy);
    }
    break;
    case EnemyType::Spear_Ace:
        break;
    case EnemyType::Boss:
        {
        BaseEnemy* enemy = new LastBoss(graphics_,
            Source_.mEmitterPoint,
            mEditor.fGetParam(Source_.mType),this);
        mEnemyVec.emplace_back(enemy);
        }
        break;
    case EnemyType::Count: break;
case EnemyType::Tutorial_NoMove:
{
    BaseEnemy* enemy = new TutorialEnemy_NoMove(graphics_,
        Source_.mEmitterPoint,
        mEditor.fGetParam(Source_.mType));
    mEnemyVec.emplace_back(enemy);
}
    break;
case EnemyType::Boss_Unit:
    fCreateBossUnit(graphics_, mPlayerPosition);
    break;
    default:;
    }


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
             auto capsule1 = enemy1->fGetBodyCapsule();
             auto capsule2 = enemy2->fGetBodyCapsule();
            // �������a���O�ȉ��Ȃ�v�Z���Ȃ�
            if(capsule1.mRadius<=0.0f||capsule2.mRadius<=0.0f)
            {
                continue;
            }

            //--------------------<�G���m�̋������𒲐�����>--------------------//
            capsule1.mRadius *= mAdjustmentEnemies;
            capsule2.mRadius *= mAdjustmentEnemies;

            
            const bool result=Collision::capsule_vs_capsule(
                capsule1.mTop, capsule1.mBottom, capsule1.mRadius,
                capsule2.mTop, capsule2.mBottom, capsule2.mRadius);

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
                
            }

        }
    }
}

void EnemyManager::fLoad(const char* FileName_)
{
    // Json�t�@�C������l���擾
    std::filesystem::path path = FileName_;
    path.replace_extension(".json");
    if (std::filesystem::exists(path.c_str()))
    {
        std::ifstream ifs;
        ifs.open(path);
        if (ifs)
        {
            cereal::JSONInputArchive o_archive(ifs);
            o_archive(mCurrentWaveVec);
        }
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
        static int elem = static_cast<int>(EnemyType::Sword);
        constexpr int count = static_cast<int>(EnemyType::Count);
        const char* elems_names[count] =

        {
            "Archer","Shield","Sword","Spear","Archer_Ace",
            "Shield_Ace","Sword_Ace","Spear_Ace",
            "Boss","TutorialNoMove","BossUnit"};

        const char* elem_name = (elem >= 0 && elem < count) ? elems_names[elem] : "Unknown";
        ImGui::SliderInt("slider enum", &elem, 0, count - 1, elem_name);

        if (ImGui::Button("CreateEnemy"))
        {
            EnemySource source{};
            source.mEmitterPoint = {};
            source.mType = static_cast<EnemyType>(elem);
            fSpawn(source,Graphics_);
        }

        ImGui::InputInt("WaveNumber", &mCurrentWave);
        if (ImGui::Button("StartWave"))
        {
            fStartWave(mCurrentWave);
        }
       
        ImGui::Separator();

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

        ImGui::Checkbox("DebugMode", &mDebugMode);

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

std::vector<BaseEnemy*> EnemyManager::fGetEnemies() const
{
    return mEnemyVec;
}

void EnemyManager::fRegisterCash(GraphicsPipeline& graphics_)
{
    // �L���b�V���Ƀ��f����o�^
    BaseEnemy* enemy = new SwordEnemy(graphics_);
    mCashEnemyVec.emplace_back(enemy);
    enemy = new ArcherEnemy(graphics_);
    mCashEnemyVec.emplace_back(enemy);
    enemy = new SpearEnemy(graphics_);
    mCashEnemyVec.emplace_back(enemy);
    enemy = new SpearEnemy(graphics_);
    mCashEnemyVec.emplace_back(enemy);
    enemy = new SpearEnemy_Ace(graphics_);
    mCashEnemyVec.emplace_back(enemy);
    enemy = new SwordEnemy_Ace(graphics_);
    mCashEnemyVec.emplace_back(enemy);
    enemy = new LastBoss(graphics_);
    mCashEnemyVec.emplace_back(enemy);
    enemy = new TutorialEnemy_NoMove(graphics_);
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



void EnemyManager::fCreateBossUnit(GraphicsPipeline& Graphics_,
    const DirectX::XMFLOAT3& Position_)
{
    BaseEnemy* enemy = new BossUnit(Graphics_,
        Position_,
        mEditor.fGetParam(EnemyType::Boss_Unit),
        BulletManager::Instance().fGetAddFunction());
    mEnemyVec.emplace_back(enemy);
}
