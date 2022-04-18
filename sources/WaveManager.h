#pragma once

#include "EnemyManager.h"
#include "sprite_dissolve.h"
#include "sprite_batch.h"
#include "practical_entities.h"

//****************************************************************
//
// ウェーブを管理するクラス
//
//****************************************************************
class WaveManager final : public PracticalEntities
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
    bool during_clear_performance() { return mWaveState == WaveState::Clear; }
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
    enum class CLEAR_STATE
    {
        REDUCTION,    // 縮小
        SELECTION,    // ステージ選択
        ENLARGEMENT,  // 拡大
    };
    CLEAR_STATE clear_state = CLEAR_STATE::REDUCTION;
    // reduction
    void transition_reduction();
    void update_reduction(float elapsed_time);
    // selection
    void transition_selection();
    void update_selection(float elapsed_time);
    // enlargement
    void transition_enlargement();
    void update_enlargement(float elapsed_time);

    bool close = false;
    float wait_timer = 1.5f;

    enum STAGE_IDENTIFIER /*S_到達数_左からの数*/
    {
        S_1_1,
        S_2_1,
        S_2_2,
        S_3_1,
        S_3_2,
        S_3_3,
        S_4_1,
        S_4_2,
        S_5_1,
        BOSS,


        STAGE_COUNT,
    };

    struct StageDetails
    {
        enum class ROUTE
        {
            LEFT,
            RIGHT,
            UP,
            DOWN,
        };
        DirectX::XMFLOAT2 position = {}; // 各ステージの位置
        // 各ステージのルートとその行先
        std::map<ROUTE, STAGE_IDENTIFIER> journeys;
    };
    StageDetails stage_details[STAGE_IDENTIFIER::STAGE_COUNT];
    STAGE_IDENTIFIER current_stage      = STAGE_IDENTIFIER::S_1_1;
    STAGE_IDENTIFIER next_stage         = STAGE_IDENTIFIER::S_1_1;
    STAGE_IDENTIFIER candidate_stage    = STAGE_IDENTIFIER::S_1_1; // 次に選べるルート候補
    DirectX::XMFLOAT2 viewpoint         = {};
    DirectX::XMFLOAT2 arrival_viewpoint = {};
    DirectX::XMFLOAT2 arrival_scale     = {};

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
    struct Icon
    {
        std::unique_ptr<SpriteDissolve> sprite = nullptr;
        float threshold = 1.0f;
        SpriteArg arg = {};
    };
    std::unique_ptr<SpriteDissolve> arrow_sprite = nullptr;
    struct Arrow
    {
        float threshold = 1.0f;
        SpriteArg arg = {};
    };
    Icon map;
    Icon player_icon;
    std::map<StageDetails::ROUTE, Arrow> arrows;

    StageDetails::ROUTE route_state = StageDetails::ROUTE::LEFT;

    //---ここまで--//


};


