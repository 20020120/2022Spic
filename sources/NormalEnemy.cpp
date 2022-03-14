#include"NormalEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
#include "user.h"
//****************************************************************
// 
// 盾なし通常攻撃の雑魚敵の派生クラス
// 
//****************************************************************
NormalEnemy::NormalEnemy(GraphicsPipeline& graphics_, DirectX::XMFLOAT3 EmitterPoint_,int UniqueId_,
    ParamGetFunction Function_)
    :BaseEnemy(graphics_, UniqueId_, "./resources/Models/Enemy/tx_arrow.fbx")
{
    // 位置を初期化
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.03f,0.03f,0.03f };
    //パラメーターの初期化
	fParamInitialize();
    fGetParam(this, Function_);
    fRegisterFunctions();

    mpSkinnedMesh->play_animation(0, true);
}

void NormalEnemy::fInitialize()
{

}

void NormalEnemy::fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_)
{
    //--------------------<更新処理>--------------------//
    fUpdateBase(elapsedTime_, Graphics_);
}


void NormalEnemy::fRegisterFunctions()
{
    InitFunc Ini = [=]()->void
    {
        fIdleInit();
    };
    UpdateFunc Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fIdleUpdate(elapsedTime_, Graphics_);
    };
    FunctionTuple tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(State::Idle, tuple));

    Ini = [=]()->void
    {
        fMoveInit();
    };
    Up = [=](float elapsedTime_,GraphicsPipeline& Graphics_)->void
    {
        fmoveUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(State::Move, tuple));

    Ini = [=]()->void
    {
        fAttackInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fAttackUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(State::Attack, tuple));

    Ini = [=]()->void
    {
        fDamagedInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fDamagedUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(State::Damaged, tuple));

    fChangeState(State::Idle);

}

void NormalEnemy::fParamInitialize()
{
    //mParam.mHitPoint = 10;      // 体力
    //mParam.mAttackPower = 10;   // 攻撃力
    //mParam.mMoveSpeed = 10;   // 移動速度
    //mParam.mAttackSpeed = 2; // 攻撃間隔
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
    fChangeState(State::Damaged);

    // 死亡したら爆発エフェクトを出す
    if (mParam.mHitPoint <= 0)
    {
        fDieEffect();
    }
}


void NormalEnemy::fIdleInit()
{
    //mpSkinnedMesh->play_animation(IDLE, true, 0.1f);
    
}

void NormalEnemy::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    mStayTimer -= elapsedTime_;
    if (mStayTimer > 0.0f) return;
    if (mAttack_flg)
    {
        fChangeState(State::Attack);
        mAttack_flg = false;
        return;
    }

    fChangeState(State::Move);

}

void NormalEnemy::fMoveInit()
{
    max_move_speed = mParam.mMoveSpeed;
   // mpSkinnedMesh->play_animation(MOVE, true, 0.1f);
	mAttackingTime = 0.0f;
}

void NormalEnemy::fmoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    MovingProcess(forward, max_move_speed);
    fTurnToTarget(elapsedTime_, mPlayerPosition);
    if(mLengthFromPlayer < 4.0f)
    {
        mAttack_flg = true;
        mStayTimer = 1.0f;
        fChangeState(State::Idle);
    }
}

void NormalEnemy::fAttackInit()
{
   // mpSkinnedMesh->play_animation(ATTACK, true, 0.1f);
    mAttackingTime = 0.0f;
    fSetAttackPower(2, 1.5f);
}

void NormalEnemy::fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fAttackStart();
    DirectX::XMVECTOR Forward = DirectX::XMLoadFloat3(&forward);
    DirectX::XMFLOAT3 attack_pos;
	DirectX::XMStoreFloat3(&attack_pos,DirectX::XMVectorScale(Forward, 2.0f));
    fSetAttackRange(attack_pos, up, 1.5f, 1.5f);

    mAttackingTime += elapsedTime_;
    if (mAttackingTime > 2.0f)
    {
        fAttackEnd();

        fChangeState(State::Idle);
    }
}

void NormalEnemy::fDamagedInit()
{
}


void NormalEnemy::fDamagedUpdate(float elapsedTime_,GraphicsPipeline& Graphics_)
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

    fChangeState(State::Idle);
}

void NormalEnemy::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Text("Name : Normal");
    ImGui::DragFloat3("position", &mPosition.x);
    ImGui::DragFloat3("angle", &mOrientation.x);
    const char* state_list[] = { "IDLE","MOVE","ATTACK","DAUNTED"};
   /* std::string state =  state_list[static_cast<int>(mNowState)];
    ImGui::Text("State"); ImGui::SameLine();
    ImGui::Text(state.c_str());*/
    ImGui::Checkbox("Attack", &mAttack_flg);
    if(ImGui::Button("dameged", { 70.0f,30.0f }))
    {
        fDamaged(1,0.6f);
    }
#endif

}
