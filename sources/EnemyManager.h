#pragma once
#include"BaseEnemy.h"

#include<map>
#include<vector>
#include <d3d11.h>

//****************************************************************
// 
// 構造体
// 
//****************************************************************
class Emitter
{
public:
    void Initialize();

    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition();
private:
    DirectX::XMFLOAT3 mPosition{};
};

struct EnemySource
{
    float mSpawnTimer{};  // 出現タイミングを記録
    int mEmitterNumber{}; // 出現位置の番号
    int mType{};          // 敵の種類
};


//****************************************************************
// 
// 敵の管理クラス 
// 
//****************************************************************
class EnemyManager final 
{
    //****************************************************************
    // 
    //  構造体
    // 
    //****************************************************************
    // 敵の種類
    enum class EnemyType
    {
        Test, // テスト用（本番では使わない）

    };

    //****************************************************************
    // 
    // 関数
    //  
    //****************************************************************
public:
    EnemyManager() = default;
    ~EnemyManager() = default;

    void fInitialize(ID3D11Device* pDevice_);
    void fUpdate(float elapsedTime_);
    void fRender(ID3D11DeviceContext* pDeviceContext_);
    void fFinalize();

    //--------------------<ImGui>--------------------//
    void fGuiMenuItem();
    void fGuiMenu(); 
private:
    //--------------------<敵と関連する処理>--------------------//
    void fSpawn(); // 敵の生成を管理
    void fEnemiesUpdate(float elapsedTime_); // 敵の更新処理
    void fEnemiesRender(ID3D11DeviceContext* pDeviceContext_); // 敵の描画処理

    //--------------------<外部ファイルに干渉する>--------------------//
    void fLoad();

    //--------------------<管理クラス内で完結する処理>--------------------//
    void fAllClear(); // 敵を全削除する関数


    //****************************************************************
    // 
    // 変数 
    // 
    //****************************************************************
private:
    std::vector<BaseEnemy*> mEnemyVec;  // 敵を格納するコンテナ
    std::vector<BaseEnemy*> mRemoveVec; // 敵を格納するコンテナ
    std::map<int, Emitter> mEmitterMap{}; // 敵の出現口

    // 初期化用にポインタを保持
    const ID3D11Device* mpDevice{ nullptr };

    //--------------------<ウェーブに関する変数>--------------------//
    float mWaveTimer{}; // 各ウェーブ開始からの経過時間
    int mCurrentWave{}; // 現在のウェーブ

    // ImGui
    bool mOpenGuiMenu{};
};
