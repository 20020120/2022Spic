#pragma once 
#include <stdexcept>
//****************************************************************
// 
// ウェーブを管理するクラス
// 
//****************************************************************
class WaveManager final 
{
public:
    WaveManager() = default;
    ~WaveManager() = default;

    // 初期化
    void fInitialize();
    // 更新
    void fUpdate(float elapsedTime_);
    // 終了処理
    void fFinalize();

    // クリア処理
    void fClear();
    // ウェーブ開始


private:
    // 現在のウェーブ
    int mCurrentWave{};
};


