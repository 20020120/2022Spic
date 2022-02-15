#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>
//****************************************************************
// 
// テストとサンプルを兼ねた敵の派生クラス 
// 
//****************************************************************

typedef std::function<void()> InitFunc;
typedef std::function<void(float)> UpdateFunc;
typedef std::tuple<InitFunc,UpdateFunc > FunctionTuple;

class TestEnemy final:public BaseEnemy
{
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
public:
    
    TestEnemy(ID3D11Device* pDevice_, 
        DirectX::XMFLOAT3 EmitterPoint_/*スポーン位置*/);

    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;
    //****************************************************************
    // 
    // 変数 
    // 
    //****************************************************************
private:
    int mState{}; // 状態
    bool mIsInitialize{ false }; // 初期化したかどうか

    //****************************************************************
    // 
    // 簡易的なステートマシンを仮実装（実装速度が必要じゃないなら変えるかも）
    // 
    //****************************************************************
    std::map<int, FunctionTuple> mFunctionMap{};
    FunctionTuple mCurrentTuple{};

    void fRegisterFunctions();
    void fIdleInit();   // 待機の初期化
    void fIdleUpdate(float elapsedTime_); // 待機の更新処理

    void fChangeState(int i);
};
