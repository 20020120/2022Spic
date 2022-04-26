#pragma once
#include"BaseEnemy.h"
#include"EnemyFileSystem.h"
#include"EnemiesEditor.h"
#include<vector>


#include "Common.h"

//****************************************************************
//
// 敵の管理クラス
//
//****************************************************************
class EnemyManager final
{
    //****************************************************************
    //
    // 関数
    //
    //****************************************************************
public:
    EnemyManager() = default;
    ~EnemyManager();

    void fInitialize(GraphicsPipeline& graphics_, AddBulletFunc Func_);
    void fUpdate(GraphicsPipeline & graphics_,float elapsedTime_, AddBulletFunc Func_);
    void fRender(GraphicsPipeline& graphics_);
    void fFinalize();

public:
    //--------------------<当たり判定>--------------------//

    // プレイヤーの攻撃と敵の当たり判定
    int fCalcPlayerAttackVsEnemies(           // 戻り値 ： 当たった敵の数
        DirectX::XMFLOAT3 PlayerCapsulePointA_,// プレイヤーのカプセルの情報
        DirectX::XMFLOAT3 PlayerCapsulePointB_,// プレイヤーのカプセルの情報
        float PlayerCapsuleRadius_,            // プレイヤーのカプセルの情報
        int PlayerAttackPower_                 // プレイヤーの攻撃力
    );
    // 敵の攻撃とプレイヤーの当たり判定
    bool fCalcEnemiesAttackVsPlayer(
        DirectX::XMFLOAT3 PlayerCapsulePointA_,// プレイヤーのカプセルの情報
        DirectX::XMFLOAT3 PlayerCapsulePointB_,// プレイヤーのカプセルの情報
        float PlayerCapsuleRadius_,             // プレイヤーのカプセルの情報
        AddDamageFunc Func_
    );
public:
    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]] const BaseEnemy* fGetNearestEnemyPosition();
    [[nodiscard]] const BaseEnemy* fGetSecondEnemyPosition();
    [[nodiscard]] bool fGetClearWave() const;
    //--------------------<セッター関数>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 Position_);

public:
    //--------------------<ImGui>--------------------//
    void fGuiMenu(GraphicsPipeline& Graphics_, AddBulletFunc Func_);
    //--------------------<ウェーブ切り替え関数>--------------------//
    void fStartWave(int WaveIndex_);
    //--------------------<ウェーブクリアしたかどうかを判定する>--------------------//
    [[nodiscard]] bool fWaveClear() const;

    //--------------------<敵の実体を削除する関数>--------------------//
    void fDeleteEnemies();

    //--------------------<キャッシュにモデルデータを登録>--------------------//
    void fRegisterCash(GraphicsPipeline& graphics_);
    void fDeleteCash(); // キャッシュを削除


private:
    //--------------------<敵と関連する処理>--------------------//
    void fSpawn(GraphicsPipeline& graphics); // 敵の生成を管理
    void fSpawn(EnemySource Source_, GraphicsPipeline& graphics_);
    void fEnemiesUpdate(GraphicsPipeline& Graphics_,float elapsedTime_); // 敵の更新処理
    void fEnemiesRender(GraphicsPipeline& graphics_); // 敵の描画処理

    //--------------------<敵をソートする>--------------------//
    void fSort(std::function<bool(const BaseEnemy* A_, const BaseEnemy* B_)> Function_);

    //--------------------<管理クラス内で完結する処理>--------------------//
    void fAllClear(); // 敵を全削除する関数

    //--------------------<敵同士の当たり判定>--------------------//
    void fCollisionEnemyVsEnemy();


    void fLoad(const char* FileName_);
    //****************************************************************
    //
    // 変数
    //
    //****************************************************************
private:
    std::vector<BaseEnemy*> mEnemyVec;  // 敵を格納するコンテナ
    std::vector<BaseEnemy*> mRemoveVec; // 敵を格納するコンテナ
    // リソースマネージャーが情報を保持できるようにキャッシュとしてモデルデーを登録する
    std::vector<BaseEnemy*> mCashEnemyVec{};

    //--------------------<ウェーブに関する変数>--------------------//
    float mWaveTimer{}; // 各ウェーブ開始からの経過時間
    int mCurrentWave{}; // 現在のウェーブ
    // 現在のウェーブのデータ
    std::vector<EnemySource> mCurrentWaveVec{};
    std::vector<EnemySource> mFirstTest{};
    float mLastTimer{};// 最後に出てくる敵の時間
    EnemyEditor mEditor{};

    bool mDebugMode{};

    //--------------------<プレイヤーの位置>--------------------//
    DirectX::XMFLOAT3 mPlayerPosition{};

    int mUniqueCount{};

    //****************************************************************
    //
    // 定数
    //
    //****************************************************************

    // 敵同士の間隔を調整する
    const float mAdjustmentEnemies{ 1.0f };
    
    const char* mWaveFileNameArray[6]
    {
        "./resources/Data/Wave1.json",
        "./resources/Data/Wave2.json",
        "./resources/Data/Wave3.json",
        "./resources/Data/Wave4.json",
        "./resources/Data/Wave5.json",
        "./resources/Data/Wave6.json",
    };

    // ImGui
    bool mOpenGuiMenu{};

};
