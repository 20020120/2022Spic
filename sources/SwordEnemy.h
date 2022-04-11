#pragma once
#include"BaseEnemy.h"
class SwordEnemy :public BaseEnemy
{
public:
    //****************************************************************
    // 
    // 構造体
    // 
    //****************************************************************
     struct DivedState: public StateTag
     {
         inline static const std::string AttackBegin = "AttackBegin";
         inline static const std::string AttackMiddle = "AttackMiddle";
         inline static const std::string AttackEnd = "AttackEnd";
         inline static const std::string Die = "Die";
     };
     enum  AnimationName {
         idle,
         walk,
         attack_idle,
         attack_up,
         attack_down,
         damage,
     };
public:
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
    SwordEnemy(GraphicsPipeline& graphics_, int UniqueId_, DirectX::XMFLOAT3 EmitterPoint_/*スポーン位置*/);
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;

    void fRegisterFunctions() override; // ステートを登録

    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************
private:
    float mWaitTimer{}; // 待ち時間

    //****************************************************************
    // 
    // 定数 
    // 
    //****************************************************************
    const float mMoveSpeed{ 10.0f };      // 移動速度
    const float mAttackRange{ 10.0f };    // 攻撃範囲
    const float mAttackDelaySec{ 1.0f };  // 攻撃後の隙の長さ（秒）
    const float mSpawnDelaySec{ 1.0f };   // 登場後の長さ（秒）
private:
    //****************************************************************
    // 
    // ステートマシン
    // 
    //****************************************************************
    //--------------------<剣の敵の共通の動き>--------------------//
    // スポーン
    void fSpawnInit();
    void fSpawnUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
 
    // 歩き
    void fWalkInit();
    void fWalkUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // 攻撃はじめ
    void fAttackBeginInit();
    void fAttackBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // 攻撃
    void fAttackInit();
    void fAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // 攻撃終わり
    void fAttackEndInit();
    void fAttackEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // やられ
    void fDieInit();
    void fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
public:
    void fInitialize() override{};
};



