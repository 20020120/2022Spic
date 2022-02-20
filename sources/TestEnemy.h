#pragma once
#include"BaseEnemy.h"
#include<functional>
#include<tuple>
#include<map>

#include "EnemiesEditor.h"
//****************************************************************
// 
// テストとサンプルを兼ねた敵の派生クラス 
// 
//****************************************************************
class TestEnemy final:public BaseEnemy
{
    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
public:
    
    TestEnemy(ID3D11Device* pDevice_, 
        DirectX::XMFLOAT3 EmitterPoint_/*スポーン位置*/,
        int UniqueId_,
        ParamGetFunction Function_);

    void fInitialize() override;
    void fUpdate(float elapsedTime_) override;

    void fGuiMenu()override;
    //****************************************************************
    // 
    // 変数 
    // 
    //****************************************************************
private:
    // ステートマシンを追加する関数
    void fRegisterFunctions() override;

    //--------------------<各ステートの関数>--------------------//
    void fIdleInit();   // 待機の初期化
    void fIdleUpdate(float elapsedTime_); // 待機の更新処理

    
protected:
    void fSetCapsulePoint() override;
};
