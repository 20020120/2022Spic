#pragma once
//#include"BaseEnemy.h"
//#include "EnemiesEditor.h"
////****************************************************************
//// 
//// 剣の敵の中ボス 
//// 
////****************************************************************
//class SwordEnemyAce final :public BaseEnemy
//{
//public:
//    struct DivideState:public StateTag
//    {
//        inline static const std::string CounterBegin     = "CounterBegin";      // カウンターはじめ
//        inline static const std::string CounterMiddle    = "CounterMiddle";     // カウンター受付中
//        inline static const std::string CounterEnd       = "CounterEnd";        // カウンター受付終了
//        inline static const std::string CounterActivate  = "CounterActivate";   // カウンター攻撃始動
//        inline static const std::string CounterAttack    = "CounterAttack";     // カウンター攻撃
//        inline static const std::string CounterAttackEnd = "CounterAttackEnd";  // カウンター攻撃終了
//    };
//    //****************************************************************
//    // 
//    // 関数
//    // 
//    //****************************************************************
//    SwordEnemyAce(GraphicsPipeline& graphics_, int UniqueId_,
//        DirectX::XMFLOAT3 EmitterPoint_/*スポーン位置*/,
//        ParamGetFunction Func_);
//    ~SwordEnemyAce() override = default;
//
//    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
//    void fRegisterFunctions() override; // ステートを登録
//
//    void fSetAttackCapsuleCollider();
//
//    void fSetVernierEffectPos();
//
//    void fStopEffect() override;
//    //****************************************************************
//    // 
//    // 変数
//    // 
//    //****************************************************************
//
//    //****************************************************************
//    // 
//    // ステートマシン
//    // 
//    //****************************************************************
//    //--------------------<登場>--------------------//
//    void fStartInit();
//    void fStartUpdate(GraphicsPipeline& Graphics_, float elapsedTime_);
//    //--------------------<待機>--------------------//
//
//
//    //--------------------<移動>--------------------//
//
//};
//
