#include"ChaseEnemy.h"
#include"Operators.h"
//****************************************************************
// 
// 移動だけのザコ敵 
// 
//****************************************************************

ChaseEnemy::ChaseEnemy(ID3D11Device* pDevice_, DirectX::XMFLOAT3 Point_, int UniqueId_,
    ParamGetFunction Function_)
    :BaseEnemy(pDevice_, UniqueId_, "./resources/Models/Enemy/character_5.fbx")
{
    //初期化
    mPosition = Point_;
    mScale = { 0.03f,0.03f,0.03f };
    mOrientation = { 0.0f,0.0f,0.0f,1.0f };

    // パラメーターを初期化
    fGetParam(this, Function_);

    // 距離を初期化
    mDistance = static_cast<float>((rand() / RAND_MAX));

    fRegisterFunctions();
}

void ChaseEnemy::fInitialize()
{
   
}

 void ChaseEnemy::fUpdate(float elapsedTime_)
{
     fUpdateBase(elapsedTime_);
     mTimerComponent.fUpdate(elapsedTime_);

    
}

 void ChaseEnemy::fRender(ID3D11DeviceContext* device_context)
 {
     // ワールド行列を作成
     const auto worldMatrix = Math::calc_world_matrix(mScale, mOrientation, mPosition);
     mpSkinnedMesh->render(device_context, worldMatrix, { 1.0f,1.0f,1.0f,1.0f });
 }

void ChaseEnemy::fMove(float elapsedTime_)
{
   
}

 void ChaseEnemy::fRotate(float elapsedTime_)
 {
     using namespace DirectX;
     //ターゲットに向かって回転
     XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&mOrientation);
     DirectX::XMVECTOR forward, right, up;
     DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
     DirectX::XMFLOAT4X4 m4x4 = {};
     DirectX::XMStoreFloat4x4(&m4x4, m);
     right = { m4x4._11, m4x4._12, m4x4._13 };
     //up = { m4x4._21, m4x4._22, m4x4._23 };
     up = { 0, 1, 0 };
     forward = { m4x4._31, m4x4._32, m4x4._33 };

     XMVECTOR pos_vec = XMLoadFloat3(&mPosition);//自分の位置
     DirectX::XMFLOAT3 front{};
     DirectX::XMStoreFloat3(&front, forward);
     DirectX::XMFLOAT3 t{ mPlayerPosition };
     XMVECTOR target_vec = XMLoadFloat3(&t);
     XMVECTOR d = XMVector3Normalize(target_vec - pos_vec);
     float d_length = Math::calc_vector_AtoB_length(mPosition, t);
     {
         DirectX::XMFLOAT3 point = Math::calc_designated_point(mPosition, front, d_length);
         point.y = mPlayerPosition.y;
         DirectX::XMVECTOR point_vec = DirectX::XMLoadFloat3(&point);

         XMVECTOR d2 = XMVector3Normalize(point_vec - pos_vec);

         float an;
         XMVECTOR a = XMVector3Dot(d2, d);
         XMStoreFloat(&an, a);
         an = acosf(an);
         float de = DirectX::XMConvertToDegrees(an);

         if (fabs(an) > DirectX::XMConvertToRadians(0.1f))
         {
             XMVECTOR q;
             DirectX::XMFLOAT3 a{};
             DirectX::XMStoreFloat3(&a, d2);
             DirectX::XMFLOAT3 b{};
             DirectX::XMStoreFloat3(&b, d);
             float cross{ (b.x * a.z) - (b.z * a.x) };

             if (cross > 0)
             {
                 q = XMQuaternionRotationAxis(up, an);//正の方向に動くクオータニオン
             }
             else
             {
                 q = XMQuaternionRotationAxis(up, -an);//正の方向に動くクオータニオン
             }
             XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
             orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsedTime_);
         }
     }
     //right
     {

         DirectX::XMFLOAT3 point = Math::calc_designated_point(mPosition, front, d_length);
         point.x = mPlayerPosition.x;
         point.z = mPlayerPosition.z;
         DirectX::XMVECTOR point_vec = DirectX::XMLoadFloat3(&point);


         XMVECTOR d2 = XMVector3Normalize(point_vec - pos_vec);

         float an;
         XMVECTOR a = XMVector3Dot(d2, d);
         XMStoreFloat(&an, a);
         an = acosf(an);
         float de = DirectX::XMConvertToDegrees(an);
         if (fabs(an) > DirectX::XMConvertToRadians(0.1f) && fabs(an) < DirectX::XMConvertToRadians(170.0f))
         {
             //回転軸と回転角から回転クオータニオンを求める
             XMVECTOR q;
             if (point.y > mPlayerPosition.y)
             {
                 q = XMQuaternionRotationAxis(right, an);//正の方向に動くクオータニオン
             }
             else
             {
                 q = XMQuaternionRotationAxis(right, -an);//正の方向に動くクオータニオン
             }
             XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
             orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsedTime_);
         }
     }
     DirectX::XMStoreFloat4(&mOrientation, orientation_vec);
 }

void ChaseEnemy::fRegisterFunctions()
{
    {
        InitFunc ini = [=]()->void
        {
            fStartInit();
        };
        UpdateFunc up = [=](float elapsedTIme_)->void
        {
            fStartUpdate(elapsedTIme_);
        };

        FunctionTuple tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(State::Start, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fChaseInit();
        };
        UpdateFunc up = [=](float elapsedTIme_)->void
        {
            fChaseUpdate(elapsedTIme_);
        };

        FunctionTuple tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(State::Chase, tuple));
    }
    {
        InitFunc ini = [=]()->void
        {
            fIntimidationInit();
        };
        UpdateFunc up = [=](float elapsedTIme_)->void
        {
            fIntimidationUpdate(elapsedTIme_);
        };

        FunctionTuple tuple = std::make_tuple(ini, up);
        mFunctionMap.insert(std::make_pair(State::Intimidation, tuple));
    }

    mCurrentTuple = mFunctionMap.at(State::Start);
}

void ChaseEnemy::fStartInit()
{
    // 待機時間を設定
    mTimerComponent.StartTimer(5.0f);
}

void ChaseEnemy::fStartUpdate(float elapsedTime_)
{
    if (mTimerComponent.fGetOver())
    {
        fChangeState(State::Chase);
    }
    fRotate(elapsedTime_);
}

void ChaseEnemy::fChaseInit()
{
    mChaseDirection = Math::Normalize(mPlayerPosition - mPosition);
    mTimerComponent.StartTimer(5.0f);
}

void ChaseEnemy::fChaseUpdate(float elapsedTime_)
{
    if (!mTimerComponent.fGetOver())
    {
        // 位置を更新
        mPosition += mChaseDirection * mParam.mMoveSpeed * elapsedTime_;
    }
    if(mLengthFromPlayer<mDistance||mTimerComponent.fGetOver())
    {
        fChangeState(State::Intimidation);
    }
}

void ChaseEnemy::fIntimidationInit()
{
    mTimerComponent.StartTimer(5.0f);
}

void ChaseEnemy::fIntimidationUpdate(float elapsedTime_)
{
    if(mTimerComponent.fGetOver())
    {
        fChangeState(State::Chase);
    }
    fRotate(elapsedTime_);
}

void ChaseEnemy::fGuiMenu()
{
#ifdef USE_IMGUI
    ImGui::Text(("ChaseEnemy" + std::to_string(mUniqueId)).c_str());
    ImGui::DragFloat3(("Position" + std::to_string(mUniqueId)).c_str(), &mPosition.x);
    ImGui::InputInt("Hp", &mParam.mHitPoint);
    if (ImGui::Button("Damaged"))
    {
        fDamaged(1,0.0f);
    }
    ImGui::Text(std::to_string(fGetPercentHitPoint()).c_str());
    ImGui::Text(fGetType().c_str());
    ImGui::Separator();
#endif
}
