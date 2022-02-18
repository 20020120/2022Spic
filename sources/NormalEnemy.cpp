#include"NormalEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
#include "user.h"
//****************************************************************
// 
// 盾なし通常攻撃の雑魚敵の派生クラス
// 
//****************************************************************
NormalEnemy::NormalEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 EmitterPoint_,int UniqueId_)
    :BaseEnemy(pDevice_, UniqueId_, "./resources/Models/Enemy/character_4.fbx")
{
    // 位置を初期化
    mPosition = EmitterPoint_;
    mPosition = { 10,0, 10 };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.1f,0.1f,0.1f };
    //パラメーターの初期化
	fParamInitialize();
    fRegisterFunctions();

}

void NormalEnemy::fInitialize()
{
}

void NormalEnemy::fUpdate(float elapsedTime_)
{
    //--------------------<更新処理>--------------------//
    fUpdateBase(elapsedTime_);
}


void NormalEnemy::fRegisterFunctions()
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
    mFunctionMap.insert(std::make_pair(IDLE, tuple));

    Ini = [=]()->void
    {
        fMoveInit();
    };
    Up = [=](float elapsedTime_)->void
    {
        fmoveUpdate(elapsedTime_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(MOVE, tuple));

    Ini = [=]()->void
    {
        fAttackInit();
    };
    Up = [=](float elapsedTime_)->void
    {
        fAttackUpdate(elapsedTime_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(MOVE, tuple));

    fChangeState(IDLE);

}
void NormalEnemy::fParamInitialize()
{
    mParam.mHitPoint = 10;      // 体力
    mParam.mAttackPower = 10;   // 攻撃力
    mParam.mMoveSpeed = 10;   // 移動速度
    mParam.mAttackSpeed = 2; // 攻撃間隔
    mStayTimer = 1.0f;
    mAttack_flg = false;
}


void NormalEnemy::fIdleInit()
{
    mStayTimer = 1.0f;
    mNowState = IDLE;
}

void NormalEnemy::fIdleUpdate(float elapsedTime_)
{
    mStayTimer -= elapsedTime_;
    if (mStayTimer > 0.0f) return;
#if 0
    if (fTurnToPlayer(elapsedTime_, 10))
    {
        //プレイヤーへの回転が完了したら移動ステートへ遷移
        fChangeState(MOVE);
    }
#else
    fChangeState(MOVE);
#endif

}

void NormalEnemy::fMoveInit()
{
    mNowState = MOVE;
    max_move_speed = mParam.mMoveSpeed;
}

void NormalEnemy::fmoveUpdate(float elapsedTime_)
{
    MovingProcess(forward.x, forward.z, max_move_speed);
    fTurnToPlayer(elapsedTime_, 5);
    if(mAttack_flg)
    {
        fChangeState(ATTACK);
    }
}

void NormalEnemy::fAttackInit()
{
    mNowState = ATTACK;
}

void NormalEnemy::fAttackUpdate(float elapsedTime_)
{
}

void NormalEnemy::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Text("Name : Normal");
    ImGui::DragFloat3("position", &mPosition.x);
    ImGui::DragFloat3("angle", &mOrientation.x);
    const char* state_list[] = { "IDLE","MOVE","ATTACK" };
    std::string state =  state_list[static_cast<int>(mNowState)];
    ImGui::Text("State"); ImGui::SameLine();
    ImGui::Text(state.c_str());
    ImGui::Checkbox("Attack", &mAttack_flg);
#endif

}
