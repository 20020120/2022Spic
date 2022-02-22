#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>

#include "EnemiesEditor.h"
//****************************************************************
// 
// 盾なし通常攻撃の雑魚敵の派生クラス 
// 
//****************************************************************
class NormalEnemy final :public BaseEnemy
{
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
public:

    NormalEnemy(ID3D11Device* pDevice_,
        DirectX::XMFLOAT3 EmitterPoint_/*スポーン位置*/
    ,int UniqueId_, ParamGetFunction Function_
        );

    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;
    void fGuiMenu() override;
   
private:
    // ステートマシンを追加する関数
    void fRegisterFunctions() override;
    //パラメータ初期化関数
    void fParamInitialize();

    void fDamaged(int damage_, float InvinsibleTime_) override;
    //--------------------<各ステートの関数>--------------------//
    void fIdleInit();   // 待機の初期化
    void fIdleUpdate(float elapsedTime_); // 待機の更新処理

    void fMoveInit(); //移動の初期化
    void fmoveUpdate(float elapsedTime_); //移動の更新処理

    void fAttackInit(); //移動の初期化
    void fAttackUpdate(float elapsedTime_); //攻撃の更新処理

    void fDauntedInit(); //移動の初期化
    void fDauntedUpdate(float elapsedTime_); //ひるみの更新処理

    void fSetCapsulePoint() override{};
    enum State
    {
	    IDLE,
        MOVE,
        ATTACK,
        DAUNTED,
    };
    //****************************************************************
   // 
   // 変数 
   // 
   //****************************************************************
    State mNowState;
    float mStayTimer;
    float mAttackingTime;
    bool mAttack_flg;
};
