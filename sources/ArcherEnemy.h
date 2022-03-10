#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>
#include "Common.h"
#include "EnemiesEditor.h"
//****************************************************************
// 
// 盾なし通常攻撃の雑魚敵の派生クラス 
// 
//****************************************************************
class ArcherEnemy final :public BaseEnemy
{
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
public:

    ArcherEnemy(GraphicsPipeline& graphics_,
        DirectX::XMFLOAT3 EmitterPoint_/*スポーン位置*/
        , int UniqueId_, ParamGetFunction Function_,  AddBulletFunc Func_
    );

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fGuiMenu() override;

    void fDamaged(int damage_, float InvinsibleTime_) override;

private:
    // ステートマシンを追加する関数
    void fRegisterFunctions() override;
    //パラメータ初期化関数
    void fParamInitialize();

    //--------------------<各ステートの関数>--------------------//
    void fIdleInit();   // 待機の初期化
    void fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); // 待機の更新処理

    void fMoveInit(); //移動の初期化
    void fmoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //移動の更新処理

	void fMoveApproachInit(); //接近移動の初期化
    void fMoveApproachUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //後退移動の更新処理

    void fMoveLeaveInit(); //接近移動の初期化
    void fMoveLeaveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //後退移動の更新処理

    void fAttackInit(); //移動の初期化
    void fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //攻撃の更新処理

    void fDamagedInit(); //移動の初期化
    void fDamagedUpdate(float elapsedTime_, GraphicsPipeline& Graphics_); //ひるみの更新処理


    // ステートの名前を定義する
    struct  State : public BaseEnemy::StateTag
    {
        inline static const std::string Attack = "Attack";
        inline static const std::string Approach = "Approach";
        inline static const std::string Leave = "Leave";
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
    AddBulletFunc mfAddFunc;

};

