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
        Game,
        Clear,
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
    void fInitialize(GraphicsPipeline& graphics_);
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


private:

    void fClearUpdate(float elapsedTime_);

    //****************************************************************
    // 
    //  変数
    // 
    //****************************************************************
    // 現在のウェーブ
    int mCurrentWave{};

    EnemyManager mEnemyManager{};
    WaveState mWaveState;
};


