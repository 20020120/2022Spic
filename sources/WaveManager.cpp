#include"WaveManager.h"
#include "Operators.h"

#define ProtoType
void WaveManager::fInitialize(GraphicsPipeline& graphics_,AddBulletFunc Func_)
{
    // 初期化
    mEnemyManager.fInitialize(graphics_,Func_);

    mWaveState = WaveState::Start;


    //----------------------------------
    // TODO:藤岡が書いたところ2
    //----------------------------------
    // ステージ情報の登録
    DirectX::XMFLOAT2 stage_points[STAGE_IDENTIFIER::STAGE_COUNT] =
    {
        { 1500, 2700 },{ 900, 2100 },{ 2100, 2100 },{ 300, 1200 },{ 1500, 1500 },
        { 2700, 1500 },{ 1500, 900 },{ 2700, 900 },{ 2200, 300 },{ 1200, 300 }
    };
    for (int i = 0; i < STAGE_IDENTIFIER::STAGE_COUNT; ++i) { stage_details[i].position = stage_points[i]; }
    /*1-1*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_1_1;
        stage_details[index].route_count = 2;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::LEFT, STAGE_IDENTIFIER::S_2_1));
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::RIGHT, STAGE_IDENTIFIER::S_2_2));
    }
    /*2-1*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_2_1;
        stage_details[index].route_count = 1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::LEFT, STAGE_IDENTIFIER::S_3_1));
    }
    /*2-2*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_2_2;
        stage_details[index].route_count = 2;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::LEFT, STAGE_IDENTIFIER::S_3_2));
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::RIGHT, STAGE_IDENTIFIER::S_3_3));
    }
    /*3-1*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_3_1;
        stage_details[index].route_count = 1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::BOSS));
    }
    /*3-2*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_3_2;
        stage_details[index].route_count = 1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::S_4_1));
    }
    /*3-3*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_3_3;
        stage_details[index].route_count = 1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::S_4_2));
    }
    /*4-1*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_4_1;
        stage_details[index].route_count = 1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::BOSS));
    }
    /*4-2*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_4_2;
        stage_details[index].route_count = 1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::S_5_1));
    }
    /*5-1*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_5_1;
        stage_details[index].route_count = 1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::LEFT, STAGE_IDENTIFIER::BOSS));
    }
    /*BOSS*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::BOSS;
        stage_details[index].route_count = 0;
    }

    // map
    map.sprite = std::make_unique<SpriteDissolve>(graphics_.get_device().Get(),
        L".\\resources\\Sprites\\clear_wave\\map.png", L".\\resources\\Sprites\\clear_wave\\map_mask.png", 1);
    map.arg.texsize = { (float)map.sprite->get_texture2d_desc().Width, (float)map.sprite->get_texture2d_desc().Height };
    // player_icon
    player_icon.sprite = std::make_unique<SpriteDissolve>(graphics_.get_device().Get(),
        L".\\resources\\Sprites\\clear_wave\\player_icon.png", L".\\resources\\Sprites\\clear_wave\\player_icon_mask.png", 1);
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
        auto r_dissolve = [&](std::string g_name, SpriteDissolve* dissolve, SpriteArg& arg, float& threshold)
        {
#ifdef USE_IMGUI
            ImGui::Begin(g_name.c_str());
            ImGui::DragFloat2("pos", &arg.pos.x);
            ImGui::DragFloat2("scale", &arg.scale.x, 0.1f);
            ImGui::DragFloat("threshold", &threshold, 0.01f);
            ImGui::End();
#endif // USE_IMGUI
            dissolve->begin(dc);
            dissolve->render(dc, arg.pos, arg.scale, arg.pivot, arg.color, arg.angle, arg.texpos, arg.texsize, threshold);
            dissolve->end(dc);
        };
        r_dissolve("Map", map.sprite.get(), map.arg, map.threshold);
        r_dissolve("player icon", player_icon.sprite.get(), player_icon.arg, player_icon.threshold);
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
    if (!close)
    {
        map.threshold = Math::lerp(map.threshold, -0.5f, 2.0f * elapsedTime_);
        player_icon.threshold = Math::lerp(player_icon.threshold, -0.5f, 2.0f * elapsedTime_);
    }
    else
    {
        map.threshold = Math::lerp(map.threshold, 1.0f, 2.0f * elapsedTime_);
        player_icon.threshold = Math::lerp(player_icon.threshold, 1.0f, 2.0f * elapsedTime_);
    }

    switch (clear_state)
    {
    case CLEAR_STATE::REDUCTION:   // 縮小
        update_reduction(elapsedTime_);
        break;
    case CLEAR_STATE::SELECTION:   // 選択
        update_selection(elapsedTime_);
        break;
    case CLEAR_STATE::ENLARGEMENT: // 拡大
        update_enlargement(elapsedTime_);
        break;
    }



#ifdef USE_IMGUI
    ImGui::Begin("ClearProto");
    const char* elems_names[STAGE_IDENTIFIER::STAGE_COUNT] = { "1-1","2-1","2-2","3-1","3-2","3-3","4-1","4-2","5-1","BOSS" };
    {
        //static int elem = current_stage;
        //const char* elem_name = (elem >= 0 && elem < STAGE_IDENTIFIER::STAGE_COUNT) ? elems_names[elem] : "Unknown";
        //if (ImGui::SliderInt("current_stage", &elem, 0, STAGE_IDENTIFIER::STAGE_COUNT - 1, elem_name))
        //{
        //    current_stage = (STAGE_IDENTIFIER)elem;
        //}
        //ImGui::Separator();
    }
    if (ImGui::Button("transition")) { clear_initialize(); }
    ImGui::DragFloat2("viewpoint", &viewpoint.x);
    if (current_stage != STAGE_IDENTIFIER::BOSS)
    {
        ImGui::Separator();
        int elem = (int)stage_details[current_stage].journeys.at(route_state);
        const char* elem_name = (elem >= 0 && elem < STAGE_IDENTIFIER::STAGE_COUNT) ? elems_names[elem] : "Unknown";
        ImGui::SliderInt("next stage", &elem, 0, STAGE_IDENTIFIER::STAGE_COUNT - 1, elem_name);
        ImGui::Separator();
    }
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
void WaveManager::transition_reduction()
{
    clear_state = CLEAR_STATE::REDUCTION;

    wait_timer = 1.5f;

    float ratio = (stage_details[current_stage].position.y / map.arg.texsize.y);
    arrival_viewpoint = { 640.0f, ratio > 0.8f ? 600.0f * ratio : 360.0f };
    arrival_scale = { 0.4f,0.4f };
    // map
    map.threshold = 1.0f;
    map.arg.pos = viewpoint - stage_details[current_stage].position * map.arg.scale;
    map.arg.scale = { 10, 10 };
    // player_icon
    player_icon.threshold = 1.0f;
    player_icon.arg.pos = viewpoint;
    player_icon.arg.scale = DirectX::XMFLOAT2(0.2f, 0.2f) * map.arg.scale;
}

void WaveManager::update_reduction(float elapsed_time)
{
    wait_timer -= elapsed_time;
    wait_timer = (std::max)(wait_timer, 0.0f);
    if (wait_timer > 0.0f) return;
    // viewpointの移動
    float lerp_rate = 2.0f;
    viewpoint = Math::lerp(viewpoint, arrival_viewpoint, lerp_rate * elapsed_time);
    // positionの移動
    map.arg.pos = viewpoint - stage_details[current_stage].position * map.arg.scale;
    player_icon.arg.pos = viewpoint;
    // scaleの変更
    map.arg.scale = Math::lerp(map.arg.scale, arrival_scale, lerp_rate * elapsed_time);
    player_icon.arg.scale = DirectX::XMFLOAT2(0.2f, 0.2f) * map.arg.scale;

    // 選択状態に遷移
    if (Math::equal_check(map.arg.scale.x, arrival_scale.x, 0.1f))
    {
        if (current_stage != STAGE_IDENTIFIER::BOSS) { transition_selection(); }
        else {}
    }
}

void WaveManager::transition_selection()
{
    if (current_stage == STAGE_IDENTIFIER::BOSS)
    {
        bool a = true;
        assert(!a && "ボス部屋です");
    }

    clear_state = CLEAR_STATE::SELECTION;

    route_state = stage_details[current_stage].journeys.begin()->first;
}

void WaveManager::update_selection(float elapsed_time)
{
    auto r_transition_left = [&]()
    {
        if ((game_pad->get_button_down() & GamePad::BTN_LEFT) || game_pad->get_axis_LX() < -0.5f)
        {
            for (const auto& journey : stage_details[current_stage].journeys)
            {
                if (journey.first == StageDetails::ROUTE::LEFT) { route_state = StageDetails::ROUTE::LEFT; break; }
            }
        }
    };
    auto r_transition_right = [&]()
    {
        if ((game_pad->get_button_down() & GamePad::BTN_RIGHT) || game_pad->get_axis_LX() > 0.5f)
        {
            for (const auto& journey : stage_details[current_stage].journeys)
            {
                if (journey.first == StageDetails::ROUTE::RIGHT) { route_state = StageDetails::ROUTE::RIGHT; break; }
            }
        }
    };
    auto r_transition_up = [&]()
    {
        if ((game_pad->get_button_down() & GamePad::BTN_UP) || game_pad->get_axis_LY() > 0.5f)
        {
            for (const auto& journey : stage_details[current_stage].journeys)
            {
                if (journey.first == StageDetails::ROUTE::UP) { route_state = StageDetails::ROUTE::UP; break; }
            }
        }
    };
    auto r_transition_down = [&]()
    {
        if ((game_pad->get_button_down() & GamePad::BTN_DOWN) || game_pad->get_axis_LY() < -0.5f)
        {
            for (const auto& journey : stage_details[current_stage].journeys)
            {
                if (journey.first == StageDetails::ROUTE::DOWN) { route_state = StageDetails::ROUTE::DOWN; break; }
            }
        }
    };

    switch (route_state)
    {
    case StageDetails::ROUTE::LEFT:
        r_transition_right();
        r_transition_up();
        r_transition_down();
        break;
    case StageDetails::ROUTE::RIGHT:
        r_transition_left();
        r_transition_up();
        r_transition_down();
        break;
    case StageDetails::ROUTE::UP:
        r_transition_left();
        r_transition_right();
        r_transition_down();
        break;
    case StageDetails::ROUTE::DOWN:
        r_transition_left();
        r_transition_right();
        r_transition_up();
        break;
    }
    // next_stageのセット
    if (game_pad->get_button_down() & GamePad::BTN_B)
    {
        next_stage = stage_details[current_stage].journeys.at(route_state);
        transition_enlargement();
    }
}

void WaveManager::transition_enlargement()
{
    clear_state = CLEAR_STATE::ENLARGEMENT;

    arrival_viewpoint = { 640.0f, 360.0f };
    arrival_scale     = { 10.0f,10.0f };
}

void WaveManager::update_enlargement(float elapsed_time)
{
    // viewpointの移動
    float lerp_rate = 2.0f;
    viewpoint = Math::lerp(viewpoint, arrival_viewpoint, lerp_rate * elapsed_time);
    // positionの移動
    map.arg.pos = viewpoint - stage_details[current_stage].position * map.arg.scale;
    player_icon.arg.pos = viewpoint;
    // scaleの変更
    map.arg.scale = Math::lerp(map.arg.scale, arrival_scale, lerp_rate * elapsed_time);
    player_icon.arg.scale = DirectX::XMFLOAT2(0.2f, 0.2f) * map.arg.scale;

    if (Math::equal_check(map.arg.scale.x, arrival_scale.x, 0.1f))
    {
        current_stage = next_stage;
        // バグ防止
        if (current_stage != STAGE_IDENTIFIER::BOSS) route_state = stage_details[current_stage].journeys.begin()->first;
    }
}

void WaveManager::clear_initialize()
{
    close                 = false;
    viewpoint             = { 640.0f, 360.0f };

    transition_reduction();
}
//---ここまで--//