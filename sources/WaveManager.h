#pragma once 
#include"EnemyManager.h"
//****************************************************************
// 
// ウェーブを管理するクラス
// 
//****************************************************************
class WaveManager final 
{
    //****************************************************************
    // 
    // 構造体
    // 
    //****************************************************************
public:
    enum class WaveState
    {
        Start, // ゲーム開始前
        Game,  // ゲーム中
        Clear, // ゲームクリア
    };

    //****************************************************************
    // 
    //  関数
    // 
    //****************************************************************
public:
    WaveManager() = default;
    ~WaveManager() = default;

    // 初期化
    void fInitialize(GraphicsPipeline& graphics_, AddBulletFunc Func_);
    // 更新
    void fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_, AddBulletFunc Func_);
    // 終了処理
    void fFinalize();

    // クリア処理
    void fWaveClear();
    // ウェーブ開始
    void fStartWave();


    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]]  EnemyManager* fGetEnemyManager() ;

    //--------------------<セッター関数>--------------------//
    void fSetStartGame(bool Arg_);

    //--------------------<ImGui>--------------------//
    void fGuiMenu();

private:

    void fClearUpdate(float elapsedTime_);

    //****************************************************************
    // 
    //  変数
    // 
    //****************************************************************
    // 現在のウェーブ
    int mCurrentWave{};

    // 最小の演出が終了したかどうか
    bool mStartGame{};

    EnemyManager mEnemyManager{};
    WaveState mWaveState;

    //--------------------<ImGui>--------------------//
    bool mOpenGui{};

};


