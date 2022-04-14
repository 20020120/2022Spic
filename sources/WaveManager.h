#pragma once

#include"EnemyManager.h"
#include "sprite_dissolve.h"
#include "sprite_batch.h"

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
    //----------------------------------
    // TODO:藤岡が書いたところ
    //----------------------------------
    void render(ID3D11DeviceContext* dc, float elapsed_time);

    //---ここまで--//

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

    //----------------------------------
    // TODO:藤岡が書いたところ1
    //----------------------------------
    bool close = false;
    static const int STAGE_COUNT = 10;
    DirectX::XMFLOAT2 stage_points[STAGE_COUNT] =
    {
        { 1500, 2700 },{ 900, 2100 },{ 2100, 2100 },{ 300, 1200 },{ 1500, 1500 },{ 2700, 1500 },{ 1500, 900 },{ 2700, 900 },{ 2200, 300 },{ 1200, 300 }
    };
    int current_stage = 0;
    int next_stage = 0;
    DirectX::XMFLOAT2 viewpoint = {};
    struct SpriteArg
    {
        DirectX::XMFLOAT2 pos = {};
        DirectX::XMFLOAT2 scale = { 1,1 };
        DirectX::XMFLOAT2 pivot = {};
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        float angle = 0;
        DirectX::XMFLOAT2 texpos = {};
        DirectX::XMFLOAT2 texsize = {};
    };
    struct Map
    {
        std::unique_ptr<SpriteBatch> sprite;
        //std::unique_ptr<SpriteDissolve> sprite;
        float threshold = 1.0f;
        SpriteArg arg;
    };
    Map map;
    struct Icon
    {
        std::unique_ptr<SpriteBatch> sprite;
        SpriteArg arg;
    };
    Icon player_icon;
    void clear_initialize();

    //---ここまで--//


};


