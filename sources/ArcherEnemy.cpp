#include"ArcherEnemy.h"
#include"EnemyFileSystem.h"
#include"imgui_include.h"
#include "NormalEnemy.h"
#include "StraightBullet.h"
#include "Operators.h"
//****************************************************************
// 
// 盾なし通常攻撃の雑魚敵の派生クラス
// 
//****************************************************************
ArcherEnemy::ArcherEnemy(GraphicsPipeline& graphics_, DirectX::XMFLOAT3 EmitterPoint_, int UniqueId_,
    ParamGetFunction Function_, AddBulletFunc Func_)
    :BaseEnemy(graphics_, UniqueId_, "./resources/Models/Enemy/character_4.fbx")
{
    // 位置を初期化
    mPosition = EmitterPoint_;
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };
    mScale = { 0.03f,0.03f,0.03f };
    //パラメーターの初期化
    fParamInitialize();
    fGetParam(this, Function_);
    fRegisterFunctions();

    mfAddFunc = Func_;

}

void ArcherEnemy::fInitialize()
{

}

void ArcherEnemy::fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_)
{
    //--------------------<更新処理>--------------------//
    fUpdateBase(elapsedTime_, Graphics_);
}


void ArcherEnemy::fRegisterFunctions()
{
    //待機状態の登録
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

    //移動状態の登録
    Ini = [=]()->void
    {
        fMoveInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fmoveUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(State::Move, tuple));

    //接近移動状態の登録
    Ini = [=]()->void
    {
        fMoveApproachInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fMoveApproachUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(State::Approach, tuple));

    //後退移動状態の登録
    Ini = [=]()->void
    {
        fMoveLeaveInit();
    };
    Up = [=](float elapsedTime_, GraphicsPipeline& Graphics_)->void
    {
        fMoveLeaveUpdate(elapsedTime_, Graphics_);
    };
    tuple = std::make_tuple(Ini, Up);
    mFunctionMap.insert(std::make_pair(State::Leave, tuple));

    //攻撃状態の登録
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

    //ひるみ状態の登録
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

    //初期化
    fChangeState(State::Idle);

}

void ArcherEnemy::fParamInitialize()
{
    //mParam.mHitPoint = 10;      // 体力
    //mParam.mAttackPower = 10;   // 攻撃力
    //mParam.mMoveSpeed = 10;   // 移動速度
    //mParam.mAttackSpeed = 2; // 攻撃間隔
    mStayTimer = 1.0f;
    mAttack_flg = false;
}

void ArcherEnemy::fDamaged(int Damage_, float InvinsibleTime_)
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


void ArcherEnemy::fIdleInit()
{
    //mpSkinnedMesh->play_animation(IDLE, true, 0.1f);

}

void ArcherEnemy::fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{

    fChangeState(State::Move);
}

void ArcherEnemy::fMoveInit()
{
    max_move_speed = mParam.mMoveSpeed;
    // mpSkinnedMesh->play_animation(MOVE, true, 0.1f);
    mAttackingTime = 0.0f;
}

void ArcherEnemy::fmoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    MovingProcess(forward, max_move_speed);
    fTurnToTarget(elapsedTime_, mPlayerPosition);

    //プレイヤーとの距離が８以下なら離れていく行動をとる
    if (mLengthFromPlayer < 8.0f)
    {
        fChangeState(State::Leave);
        return;
    }

    //プレイヤーとの距離が15以上なら近づく行動をとる
    if (mLengthFromPlayer > 15.0f)
    {
        fChangeState(State::Approach);
    }

}

void ArcherEnemy::fMoveApproachInit()
{
    
}

void ArcherEnemy::fMoveApproachUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    MovingProcess(forward, max_move_speed);
    fTurnToTarget(elapsedTime_, mPlayerPosition);
    if (mLengthFromPlayer > 15.0f  && mLengthFromPlayer < 30.0f)
    {
        fChangeState(State::Attack);
        //弾装填
        auto straightBullet = new StraightBullet(Graphics_,
            mPosition, Math::GetFront(mOrientation) * 10.0f);
        mfAddFunc(straightBullet); 
        return;
    }

    if(mLengthFromPlayer < 14.0f)
    {
        fChangeState(State::Leave);
    }
}

void ArcherEnemy::fMoveLeaveInit()
{
}

void ArcherEnemy::fMoveLeaveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    using namespace DirectX;
    //エネミーからプレイヤーへの逆方向を算出
    XMVECTOR pos_vec = XMLoadFloat3(&mPosition);//自分の位置
    DirectX::XMFLOAT3 t = mPlayerPosition;
    XMVECTOR target_vec = XMLoadFloat3(&t);
    //自分のポジションからプレイヤーのポジションを引くことでプレイヤーと反対方向のベクトルを出す
    XMVECTOR tar = XMVector3Normalize(pos_vec - target_vec);
    XMVectorScale(tar, 3.0f);
    XMFLOAT3 t_vec{};
    XMStoreFloat3(&t_vec, tar);
    XMFLOAT3 target_pos = mPosition + t_vec;
    MovingProcess(forward, max_move_speed);
    fTurnToTarget(elapsedTime_, target_pos);

    if (mLengthFromPlayer > 15.0f && mLengthFromPlayer < 30.0f)
    {
        fChangeState(State::Attack);

        //弾装填
        auto straightBullet = new StraightBullet(Graphics_,
            mPosition, Math::GetFront(mOrientation) * 10.0f);
        mfAddFunc(straightBullet);
        return;
    }

    if (mLengthFromPlayer > 30.0f)
    {
        fChangeState(State::Approach);
    }
}

void ArcherEnemy::fAttackInit()
{
    // mpSkinnedMesh->play_animation(ATTACK, true, 0.1f);
    mAttackingTime = 0.0f;
    fSetAttackPower(2, 1.5f);

}

void ArcherEnemy::fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
{
    fAttackStart();
    DirectX::XMVECTOR Forward = DirectX::XMLoadFloat3(&forward);
    DirectX::XMFLOAT3 attack_pos;
    DirectX::XMStoreFloat3(&attack_pos, DirectX::XMVectorScale(Forward, 2.0f));
    fSetAttackRange(attack_pos, up, 1.5f, 1.5f);

    mAttackingTime += elapsedTime_;
    if (mAttackingTime > 2.0f)
    {
        fAttackEnd();

        fChangeState(State::Idle);
    }
}

void ArcherEnemy::fDamagedInit()
{
}


void ArcherEnemy::fDamagedUpdate(float elapsedTime_, GraphicsPipeline& Graphics_)
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

void ArcherEnemy::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Text("Name : Normal");
    ImGui::DragFloat3("position", &mPosition.x);
    ImGui::DragFloat3("angle", &mOrientation.x);
    const char* state_list[] = { "IDLE","MOVE","ATTACK","DAUNTED" };
    /* std::string state =  state_list[static_cast<int>(mNowState)];
     ImGui::Text("State"); ImGui::SameLine();
     ImGui::Text(state.c_str());*/
    ImGui::Checkbox("Attack", &mAttack_flg);
    if (ImGui::Button("dameged", { 70.0f,30.0f }))
    {
        fDamaged(1, 0.6f);
    }
#endif

}
