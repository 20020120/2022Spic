#include"NormalEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
#include "user.h"
//****************************************************************
// 
// 盾なし通常攻撃の雑魚敵の派生クラス
// 
//****************************************************************
NormalEnemy::NormalEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 EmitterPoint_,int UniqueId_, 
    ParamGetFunction Function_)
    :BaseEnemy(pDevice_, UniqueId_, "./resources/Models/Enemy/character_4.fbx")
{
    // 位置を初期化
    mPosition = EmitterPoint_;
    mPosition = { 10,0, 10 };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.05f,0.05f,0.05f };
    //パラメーターの初期化
	fParamInitialize();
    fRegisterFunctions();
    fGetParam(this, Function_);
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
    mFunctionMap.insert(std::make_pair(ATTACK, tuple));

    Ini = [=]()->void
    {
        fDauntedInit();
    };
    Up = [=](float elapsedTime_)->void
    {
        fDauntedUpdate(elapsedTime_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(DAUNTED, tuple));

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

void NormalEnemy::fDamaged(int Damage_, float InvinsibleTime_)
{
    //ダメージが0の場合は健康状態を変更する必要がない
    if (Damage_ == 0)return;

    //死亡している場合は健康状態を変更しない
    if (mParam.mHitPoint <= 0)return;


    if (mInvinsibleTimer > 0.0f)return;

    //無敵時間設定
    mInvinsibleTimer = InvinsibleTime_;
    //ダメージ処理
    mParam.mHitPoint -= Damage_;
    fChangeState(DAUNTED);
}


void NormalEnemy::fIdleInit()
{
    mStayTimer = 1.0f;
    mNowState = IDLE;
    //mpSkinnedMesh->play_animation(IDLE, true, 0.1f);
    
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
   // mpSkinnedMesh->play_animation(MOVE, true, 0.1f);

}

void NormalEnemy::fmoveUpdate(float elapsedTime_)
{
    move_vec_y = forward.y;
    MovingProcess(forward.x, forward.z, max_move_speed);
    fTurnToPlayer(elapsedTime_);
    if(mLengthFromPlayer < 2.0f)
    {
        fChangeState(ATTACK);
    }
}

void NormalEnemy::fAttackInit()
{
    mNowState = ATTACK;
   // mpSkinnedMesh->play_animation(ATTACK, true, 0.1f);
    mAttackingTime = 0.0f;
}

void NormalEnemy::fAttackUpdate(float elapsedTime_)
{
    mAttackingTime += elapsedTime_;
   
    if (mAttackingTime > 2.0f)
    {
        fChangeState(IDLE);
    }
}

void NormalEnemy::fDauntedInit()
{
    mNowState = DAUNTED;
}


void NormalEnemy::fDauntedUpdate(float elapsedTime_)
{
    using namespace DirectX;
    XMVECTOR P_Pos = XMLoadFloat3(&mPlayerPosition);
    XMVECTOR E_Pos = XMLoadFloat3(&mPosition);
    XMVECTOR Vec = E_Pos - P_Pos;
    Vec = XMVector3Normalize(Vec);
    XMFLOAT3 v;
    XMStoreFloat3(&v, Vec);
    velocity.x = 20.0f * v.x;
    velocity.y = 20.0f * v.y;
    velocity.z = 20.0f * v.z;

    fChangeState(IDLE);
}

void NormalEnemy::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Text("Name : Normal");
    ImGui::DragFloat3("position", &mPosition.x);
    ImGui::DragFloat3("angle", &mOrientation.x);
    const char* state_list[] = { "IDLE","MOVE","ATTACK","DAUNTED"};
    std::string state =  state_list[static_cast<int>(mNowState)];
    ImGui::Text("State"); ImGui::SameLine();
    ImGui::Text(state.c_str());
    ImGui::Checkbox("Attack", &mAttack_flg);
    if(ImGui::Button("dameged", { 70.0f,30.0f }))
    {
        fDamaged(1,0.6f);
    }
#endif

}
