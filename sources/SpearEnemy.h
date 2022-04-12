#pragma once

#include"BaseEnemy.h"

class SpearEnemy final  : public BaseEnemy
{
    //****************************************************************
    // 
    //  構造体
    // 
    //****************************************************************
    struct DivedState : public StateTag
    {
        inline static const std::string ThrustBegin = "ThrustBegin";    // 突進前の構え
        inline static const std::string ThrustMiddle = "ThrustMiddle";  // 突進中
        inline static const std::string ThrustEnd = "ThrustEnd";        // 突進後の隙
        inline static const std::string Die = "Die";
    };
    enum AnimationName {
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
    SpearEnemy(GraphicsPipeline& graphics_, 
        int UniqueId_, 
        DirectX::XMFLOAT3 EmitterPoint_/*スポーン位置*/);
    ~SpearEnemy() override;

    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
protected:
    void fRegisterFunctions() override;

private:
    //****************************************************************
    // 
    //  変数
    // 
    //****************************************************************
    float mWaitTime{}; // 待機時間
private:
    //****************************************************************
    // 
    // 定数
    // 
    //****************************************************************
    const float mIdleSec{ 2.0f };       // 待機時間
    const float mAttackLength{ 30.0f }; // プレイヤーとの距離がこの距離以下になったら
    const float mMoveSpeed{ 20.0f };    // 移動速度
    const float mThrustBeginSec{ 1.0f }; // 突進準備の時間

private:
    //****************************************************************
    // 
    // ステートマシン
    // 
    //****************************************************************
    //--------------------<待機>--------------------//
    void fIdleInit();
    void fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<移動>--------------------//
    void fMoveInit();
    void fMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<突き予備動作>--------------------//
    void fThrustBeginInit();
    void fThrustBeginUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<突き中>--------------------//
    void fThrustMiddleInit();
    void fThrustMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<突きの後隙>--------------------//
    void fThrustEndInit();
    void fThrustEndUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<ダメージ>--------------------//
    void fDamageInit();
    void fDamageUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    //--------------------<死亡>--------------------//
    void fDieInit();
    void fDieUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

};

