#include"TestEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
#include"user.h"
#include"Operators.h"
//****************************************************************
// 
// テストとサンプルを兼ねた敵の派生クラス 
// 
//****************************************************************
TestEnemy::TestEnemy(GraphicsPipeline& graphics_, DirectX::XMFLOAT3 EmitterPoint_, int UniqueId_,
    ParamGetFunction Function_)
    :BaseEnemy(graphics_, UniqueId_, "./resources/Models/Enemy/character_3.fbx")
{
    // 位置を初期化
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.02f,0.02f,0.02f };
    fRegisterFunctions();
    // 各パラメータを初期化
    fGetParam(this, Function_);
}
void TestEnemy::fInitialize()
{
}

void TestEnemy::fUpdate(float elapsedTime_)
{
    //--------------------<更新処理>--------------------//
    fUpdateBase(elapsedTime_);
}

void TestEnemy::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Text(("TestEnemy" + std::to_string(mUniqueId)).c_str());
    ImGui::DragFloat3(("Position" + std::to_string(mUniqueId)).c_str(), &mPosition.x);
    ImGui::
	Separator();
#endif

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
    mFunctionMap.insert(std::make_pair("Sample",tuple));

    mCurrentTuple = mFunctionMap.at("Sample");
}

void TestEnemy::fIdleInit()
{
    
}


void TestEnemy::fIdleUpdate(float elapsedTime_)
{
    
}

void TestEnemy::fSetCapsulePoint()
{
    
    //mCapsuleCollider.mPointA = Math::calc_designated_point(mPosition, up, 3.0f);
    //mCapsuleCollider.mPointB = Math::calc_designated_point(mPosition, -up, 3.0f);
    //mCapsuleCollider.mRadius = 1.0f;
}

