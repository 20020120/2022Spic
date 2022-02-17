#pragma once
#include"BaseEnemy.h"
#include"EnemyFileSystem.h"
#include"EnemiesEditor.h"
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
    void Initialize(DirectX::XMFLOAT3 Position_)
    {
        mPosition = Position_;
    }

    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition()const
    {
        return mPosition;
    }
private:
    DirectX::XMFLOAT3 mPosition{};
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
    enum EnemyType
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
    void fGuiMenu();
    //--------------------<ウェーブ切り替え関数>--------------------//
    void fStartWave(int WaveIndex_);
private:
    //--------------------<敵と関連する処理>--------------------//
    void fSpawn(); // 敵の生成を管理
    void fSpawn(EnemySource Source_);
    void fEnemiesUpdate(float elapsedTime_); // 敵の更新処理
    void fEnemiesRender(ID3D11DeviceContext* pDeviceContext_); // 敵の描画処理

    //--------------------<敵を出す場所を作成する>--------------------//
    void fRegisterEmitter();

    //--------------------<外部ファイルに干渉する>--------------------//
    void fLoad(const char* FileName_);

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
    ID3D11Device* mpDevice{ nullptr };

    //--------------------<ウェーブに関する変数>--------------------//
    float mWaveTimer{}; // 各ウェーブ開始からの経過時間
    int mCurrentWave{}; // 現在のウェーブ
    // 現在のウェーブのデータ
    std::vector<EnemySource> mCurrentWaveVec{};

    EnemyEditor mEditor{};

    //--------------------<プレイヤーの位置>--------------------//
    DirectX::XMFLOAT3 mPlayerPosition{};

    //****************************************************************
    // 
    // 定数
    // 
    //****************************************************************

    const char* mWaveFileNameArray[1]
    {
        "./resources/Data/Test.json",
    };

    // ImGui
    bool mOpenGuiMenu{};
   
};
