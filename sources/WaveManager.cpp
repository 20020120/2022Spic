#include"WaveManager.h"
#include "Operators.h"

#define ProtoType
void WaveManager::fInitialize(GraphicsPipeline& graphics_,AddBulletFunc Func_)
{
    // 初期化
    mEnemyManager.fInitialize(graphics_,Func_);

    mWaveState = WaveState::Start;
    mStartGame = true;

    //----------------------------------
    // TODO:藤岡が書いたところ2
    //----------------------------------
    // map
    //map.sprite = std::make_unique<SpriteDissolve>(graphics_.get_device().Get(),
    //    L".\\resources\\Sprites\\clear_wave\\map.png", L".\\resources\\Sprites\\mask\\dot.png", 1);
    map.sprite = std::make_unique<SpriteBatch>(graphics_.get_device().Get(), L".\\resources\\Sprites\\clear_wave\\map.png", 1);
    map.arg.texsize = { (float)map.sprite->get_texture2d_desc().Width, (float)map.sprite->get_texture2d_desc().Height };
    // player_icon
    player_icon.sprite = std::make_unique<SpriteBatch>(graphics_.get_device().Get(), L".\\resources\\Sprites\\clear_wave\\player_icon.png", 1);
    player_icon.arg.texsize = { (float)player_icon.sprite->get_texture2d_desc().Width, (float)player_icon.sprite->get_texture2d_desc().Height };
    player_icon.arg.pivot = player_icon.arg.texsize * DirectX::XMFLOAT2{ 0.5f, 0.5f };


    clear_initialize();
    //---ここまで--//
}

void WaveManager::fUpdate(GraphicsPipeline& Graphics_ ,float elapsedTime_, AddBulletFunc Func_)
{
    switch (mWaveState)
    {
    case WaveState::Start:
        // ゲーム開始演出が終了したかどうか
        if (mStartGame)
        {
            mWaveState = WaveState::Game;
            mEnemyManager.fStartWave(0);
            mStartGame = false;
        }
        break;
    case WaveState::Game:
        mEnemyManager.fUpdate(Graphics_,elapsedTime_,Func_);

        // クリア状態に遷移
        if(mEnemyManager.fGetClearWave())
        {
            mWaveState = WaveState::Clear;
            //----------------------------------
            // TODO:藤岡が書いたところ6
            //----------------------------------
            clear_initialize();
            //---ここまで--//
        }

        break;
    case WaveState::Clear:
        fClearUpdate(elapsedTime_);
        break;
    default: ;
    }
    fGuiMenu();
}

void WaveManager::render(ID3D11DeviceContext* dc, float elapsed_time)
{
    //----------------------------------
    // TODO:藤岡が書いたところ3
    //----------------------------------

    if (mWaveState == WaveState::Clear)
    {
        //auto r_dissolve = [&](std::string g_name, SpriteDissolve* dissolve, SpriteArg& arg, float& threshold)
        auto r_dissolve = [&](std::string g_name, SpriteBatch* dissolve, SpriteArg& arg, float& threshold)
        {
#ifdef USE_IMGUI
            ImGui::Begin(g_name.c_str());
            ImGui::DragFloat2("pos", &arg.pos.x);
            ImGui::DragFloat2("scale", &arg.scale.x, 0.1f);
            ImGui::DragFloat("threshold", &threshold, 0.01f);
            ImGui::End();
#endif // USE_IMGUI
            dissolve->begin(dc);
            //dissolve->render(dc, arg.pos, arg.scale, arg.pivot, arg.color, arg.angle, arg.texpos, arg.texsize, threshold);
            dissolve->render(dc, arg.pos, arg.scale, arg.pivot, arg.color, arg.angle, arg.texpos, arg.texsize);
            dissolve->end(dc);
        };
        auto r_batch = [&](std::string g_name, SpriteBatch* batch, SpriteArg& arg)
        {
#ifdef USE_IMGUI
            ImGui::Begin(g_name.c_str());
            ImGui::DragFloat2("pos", &arg.pos.x);
            ImGui::DragFloat2("scale", &arg.scale.x, 0.1f);
            ImGui::End();
#endif // USE_IMGUI
            batch->begin(dc);
            batch->render(dc, arg.pos, arg.scale, arg.pivot, arg.color, arg.angle, arg.texpos, arg.texsize);
            batch->end(dc);
        };

        r_dissolve("Map", map.sprite.get(), map.arg, map.threshold);
        r_batch("player icon", player_icon.sprite.get(), player_icon.arg);
    }

    //---ここまで--//


}

void WaveManager::fFinalize()
{
    mEnemyManager.fFinalize();
}

void WaveManager::fWaveClear()
{
    if(mEnemyManager.fGetClearWave())
    {
        if (mCurrentWave % 3 == 2)
        {
            // ３ウェーブごとにクリアシーンに遷移する
            mWaveState = WaveState::Clear;
            //----------------------------------
            // TODO:藤岡が書いたところ7
            //----------------------------------
            clear_initialize();
            //---ここまで--//
        }
        else
        {
            // クリアしたら
            mCurrentWave++;
            fStartWave();
        }
    }
}

void WaveManager::fStartWave()
{
    mEnemyManager.fStartWave(mCurrentWave);
}

 EnemyManager* WaveManager::fGetEnemyManager()
{
    return &mEnemyManager;
}

void WaveManager::fSetStartGame(bool Arg_)
{
    mStartGame = Arg_;
}

void WaveManager::fGuiMenu()
{
    imgui_menu_bar("System", "WaveManager", mOpenGui);

#ifdef USE_IMGUI
    if (mOpenGui)
    {
        ImGui::Begin("WaveManager");
        if (ImGui::Button("StartGame"))
        {
            fSetStartGame(true);
        }

        //----------------------------------
        // TODO:藤岡が書いたところ8
        //----------------------------------
        if (ImGui::Button("ClearGame"))
        {
            mWaveState = WaveState::Clear;
            clear_initialize();
        }
        //---ここまで--//

        ImGui::Text("State ");
        ImGui::SameLine();
        switch (mWaveState) {
        case WaveState::Start:
            ImGui::Text("Start");
            break;
        case WaveState::Game:
            ImGui::Text("Game");
            break;
        case WaveState::Clear:
            ImGui::Text("Clear");
            break;
        default:

            break;
        }

        ImGui::End();

    }
#endif


}

void WaveManager::fClearUpdate(float elapsedTime_)
{
    //----------------------------------
    // TODO:藤岡が書いたところ4
    //----------------------------------
    if (!close) { map.threshold = Math::lerp(map.threshold, 0.0f, 1.0f * elapsedTime_); }
    else { map.threshold = Math::lerp(map.threshold, 1.0f, 1.0f * elapsedTime_); }




    current_stage = next_stage;

#ifdef USE_IMGUI
    ImGui::Begin("ClearProto");
    ImGui::InputInt("current_stage", &current_stage);
    if (ImGui::Button("transition")) { clear_initialize(); }
    ImGui::DragFloat2("viewpoint", &viewpoint.x);
    ImGui::End();
#endif

    //---ここまで--//


#ifdef USE_IMGUI
    ImGui::Begin("ClearProto");
    if (ImGui::Button("NextWave"))
    {
        mWaveState = WaveState::Game;
        mCurrentWave++;
        fStartWave();
    }
    if(ImGui::Button("StartGame"))
    {
        fSetStartGame(true);
    }
    ImGui::End();
#endif
}


//----------------------------------
// TODO:藤岡が書いたところ5
//----------------------------------
void WaveManager::clear_initialize()
{
    close = false;
    viewpoint = { 640.0f, 500.0f };
    // map
    map.threshold = 1.0f;
    map.arg.pos = viewpoint - stage_points[current_stage] * map.arg.scale;
    map.arg.scale = { 1, 1 };
    // player_icon
    player_icon.arg.pos = viewpoint;
    player_icon.arg.scale = { 0.2f, 0.2f };
}
//---ここまで--//