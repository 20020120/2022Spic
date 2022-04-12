#pragma once
#include"BaseEnemy.h"
#include"EnemyFileSystem.h"
#include"EnemiesEditor.h"
#include<map>
#include<vector>
#include <d3d11.h>

#include "Common.h"

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
        // ↓↓↓↓↓↓↓↓↓ここから下に増やす↓↓↓↓↓↓↓↓↓↓↓↓↓
        Normal,
        Chase,  // 攻撃なし＆追跡してくる
        Archer, //遠距離攻撃
        Shield,//盾持ち
        MiddleBoss_, // 中ボス
        Sword,// 刀の敵 
        Spear,
        // ↑↑↑↑↑↑↑↑↑ここから上に増やす↑↑↑↑↑↑↑↑↑↑↑↑↑
        Count, // 種類の総量
    };

    //****************************************************************
    //
    // 関数
    //
    //****************************************************************
public:
    EnemyManager() = default;
    ~EnemyManager() = default;

    void fInitialize();
    void fUpdate(GraphicsPipeline & graphics_,float elapsedTime_, AddBulletFunc Func_);
    void fRender(GraphicsPipeline& graphics_);
    void fFinalize();
    //--------------------<当たり判定>--------------------//

    // プレイヤーの攻撃と敵の当たり判定
    int fCalcPlayerCapsuleVsEnemies(           // 戻り値 ： 当たった敵の数
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

    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]] const BaseEnemy* fGetNearestEnemyPosition();
    [[nodiscard]] const BaseEnemy* fGetSecondEnemyPosition();
    [[nodiscard]] const bool fGetClearWave() const;
    //--------------------<セッター関数>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 Position_);

    //--------------------<ImGui>--------------------//
    void fGuiMenu(GraphicsPipeline& Graphics_, AddBulletFunc Func_);
    //--------------------<ウェーブ切り替え関数>--------------------//
    void fStartWave(int WaveIndex_);

    //--------------------<敵の実体を削除する関数>--------------------//
    void fDeleteEnemies();



private:
    //--------------------<敵と関連する処理>--------------------//
    void fSpawn(GraphicsPipeline& graphics, AddBulletFunc Func_); // 敵の生成を管理
    void fSpawn(EnemySource Source_, GraphicsPipeline& graphics_, AddBulletFunc Func_);
    void fEnemiesUpdate(GraphicsPipeline& Graphics_,float elapsedTime_); // 敵の更新処理
    void fEnemiesRender(GraphicsPipeline& graphics_); // 敵の描画処理

    //--------------------<敵をソートする>--------------------//
    void fSort(std::function<bool(const BaseEnemy* A_, const BaseEnemy* B_)> Function_);

    //--------------------<敵を出す場所を作成する>--------------------//
    void fRegisterEmitter();

    //--------------------<外部ファイルに干渉する>--------------------//
    void fLoad(const char* FileName_);
    void fReLoadEnemyParam();

    //--------------------<管理クラス内で完結する処理>--------------------//
    void fAllClear(); // 敵を全削除する関数

    //--------------------<敵同士の当たり判定>--------------------//
    void fCollisionEnemyVsEnemy();

    //--------------------<プロト>--------------------//
    void fProtoSpawn(GraphicsPipeline& graphics_, AddBulletFunc Func_);


    //****************************************************************
    //
    // 変数
    //
    //****************************************************************
private:
    std::vector<BaseEnemy*> mEnemyVec;  // 敵を格納するコンテナ
    std::vector<BaseEnemy*> mRemoveVec; // 敵を格納するコンテナ
    std::map<int, Emitter> mEmitterMap{}; // 敵の出現口

    //--------------------<ウェーブに関する変数>--------------------//
    float mWaveTimer{}; // 各ウェーブ開始からの経過時間
    int mCurrentWave{}; // 現在のウェーブ
    // 現在のウェーブのデータ
    std::vector<EnemySource> mCurrentWaveVec{};

    EnemyEditor mEditor{};

    //--------------------<プレイヤーの位置>--------------------//
    DirectX::XMFLOAT3 mPlayerPosition{};

    int mUniqueCount{};

    //--------------------<プロト>--------------------//
    bool mIsProtoSpawn{ true };
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
