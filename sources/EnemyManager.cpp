#include"EnemyManager.h"

#include"imgui_include.h"
//****************************************************************
// 
// �G�̊Ǘ��N���X 
// 
//****************************************************************

void EnemyManager::fInitialize(ID3D11Device* pDevice_)
{
    //--------------------<������>--------------------//
    fAllClear();


}

void EnemyManager::fUpdate(float elapsedTime_)
{
    //--------------------<�Ǘ��N���X���̂̍X�V����>--------------------//

    // �E�F�[�u�J�n����̎��Ԃ��X�V
    mWaveTimer += elapsedTime_;


    //--------------------<�G�̍X�V����>--------------------//
    fEnemiesUpdate(elapsedTime_);


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
    throw std::logic_error("Not implemented");
}


void EnemyManager::fSpawn()
{
    
}

void EnemyManager::fEnemiesUpdate(float elapsedTime_)
{
    // �X�V
    for(const auto enemy: mEnemyVec)
    {
        enemy->fUpdate(elapsedTime_);
    }

    // �폜

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
    //--------------------<�v�f��S�폜>--------------------//

    for(const auto enemy: mEnemyVec)
    {
        // ���݂��Ă���΍폜
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
    ImGui::End();
}