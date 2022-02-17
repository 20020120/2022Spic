#include"TestEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
//****************************************************************
// 
// �e�X�g�ƃT���v�������˂��G�̔h���N���X 
// 
//****************************************************************
TestEnemy::TestEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 EmitterPoint_)
    :BaseEnemy(pDevice_, "./resources/Models/Enemy/character_3.fbx")
{
    // �ʒu��������
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.02f,0.02f,0.02f };
    fRegisterFunctions();

}

void TestEnemy::fInitialize()
{
}

void TestEnemy::fUpdate(float elapsedTime_)
{
    //--------------------<�X�V����>--------------------//
    fUpdateBase(elapsedTime_);

    fGuiMenu();
}

void TestEnemy::fRegisterFunctions()
{
    InitFunc Ini = [=]()->void
    {
        fIdleInit();
    };
    UpdateFunc Up = [=](float elapsedTime_)->void
    {
        fIdleUpdate(elapsedTime_);
    };
    FunctionTuple tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(0,tuple));

    mCurrentTuple = mFunctionMap.at(0);
}

void TestEnemy::fIdleInit()
{
    
}


void TestEnemy::fIdleUpdate(float elapsedTime_)
{

}

void TestEnemy::fGuiMenu()
{
    // �e�X�g�p��ImGUI
#ifdef USE_IMGUI
    ImGui::Begin("TestEnemy");
    bool a = fGetIsFrustum();
    ImGui::Checkbox("frustum", &a);
    ImGui::End();
#endif

}

