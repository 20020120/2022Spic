#include"WaveManager.h"
#include "Operators.h"
#include "scene_title.h"
#include "scene_loading.h"
#include "scene_manager.h"

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
    // ステージ情報の登録
    DirectX::XMFLOAT2 stage_points[STAGE_IDENTIFIER::STAGE_COUNT] =
    {
        { 1128, 1310 },{ 784, 974 },{ 1518, 974 },{ 484, 564 },{ 1070, 564 },
        { 1520, 564 },{ 1070, 176 }
    };
    for (int i = 0; i < STAGE_IDENTIFIER::STAGE_COUNT; ++i) { stage_details[i].position = stage_points[i]; }
    /*1-1*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_1_1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::LEFT, STAGE_IDENTIFIER::S_2_1));
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::RIGHT, STAGE_IDENTIFIER::S_2_2));
    }
    /*2-1*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_2_1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::LEFT, STAGE_IDENTIFIER::S_3_1));
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::RIGHT, STAGE_IDENTIFIER::S_3_2));
    }
    /*2-2*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_2_2;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::S_3_3));
    }
    /*3-1*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_3_1;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::BOSS));
    }
    /*3-2*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_3_2;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::BOSS));
    }
    /*3-3*/
    {
        STAGE_IDENTIFIER index = STAGE_IDENTIFIER::S_3_3;
        stage_details[index].journeys.insert(std::make_pair(StageDetails::ROUTE::UP, STAGE_IDENTIFIER::BOSS));
    }
    /*BOSS*/
    {
        /* 次のルートなし */
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
    // arrows
    {
        arrow_sprite = std::make_unique<SpriteDissolve>(graphics_.get_device().Get(),
            L".\\resources\\Sprites\\clear_wave\\arrow.png", L".\\resources\\Sprites\\clear_wave\\player_icon_mask.png", 4);
        Arrow arrow;
        arrow.arg.texsize = { (float)arrow_sprite->get_texture2d_desc().Width, (float)arrow_sprite->get_texture2d_desc().Height };
        arrow.arg.pivot = arrow.arg.texsize * DirectX::XMFLOAT2{ 0.5f, 0.5f };
        /*LEFT*/
        {
            arrow.arg.angle = 180.0f;
            arrows.insert(std::make_pair(StageDetails::ROUTE::LEFT, arrow));
        }
        /*RIGHT*/
        {
            arrow.arg.angle = 0.0f;
            arrows.insert(std::make_pair(StageDetails::ROUTE::RIGHT, arrow));
        }
        /*UP*/
        {
            arrow.arg.angle = -90.0f;
            arrows.insert(std::make_pair(StageDetails::ROUTE::UP, arrow));
        }
        /*DOWN*/
        {
            arrow.arg.angle = 90.0f;
            arrows.insert(std::make_pair(StageDetails::ROUTE::DOWN, arrow));
        }
    }

    transition_reduction();
    //---ここまで--//
}

void WaveManager::fUpdate(GraphicsPipeline& Graphics_ ,float elapsedTime_, AddBulletFunc Func_)
{
    // 3秒待ってクリア演出へ
    if (clear_flg)
    {
        clear_wait_timer -= elapsedTime_;
        if (clear_wait_timer < 0)
        {
            mWaveState = WaveState::Clear;
            //----------------------------------
            // TODO:藤岡が書いたところ6
            //----------------------------------
            transition_reduction();
            //---ここまで--//
        }
    }

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
        if (mEnemyManager.fGetClearWave()) { clear_flg = true; }

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
        for (auto& arrow : arrows)
        {
            for (const auto& journey : stage_details[current_stage].journeys)
            {
                if (arrow.first == journey.first && clear_state == CLEAR_STATE::SELECTION)
                {
                    std::string s = "arrow " + std::to_string((int)arrow.first);
                    r_dissolve(s, arrow_sprite.get(), arrow.second.arg, arrow.second.threshold);
                }
            }
        }
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
            clear_flg = true;
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
            transition_reduction();
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
        map.threshold = Math::lerp(map.threshold, 1.5f, 2.0f * elapsedTime_);
        player_icon.threshold = Math::lerp(player_icon.threshold, 1.5f, 2.0f * elapsedTime_);
        if (Math::equal_check(player_icon.threshold, 1.0f, 0.1f))
        {
            // クリア演出終了、次のステージへ
            mWaveState = WaveState::Game;
            mCurrentWave++;
            fStartWave();
        }
    }
    // state別の更新処理
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
    // 矢印の位置
    {
        DirectX::XMFLOAT2 offset = player_icon.arg.texsize * player_icon.arg.scale * 1.0f;
        if (arrows.count(StageDetails::ROUTE::LEFT)) { arrows.at(StageDetails::ROUTE::LEFT).arg.pos   = player_icon.arg.pos + DirectX::XMFLOAT2(-offset.x, 0); }
        if (arrows.count(StageDetails::ROUTE::RIGHT)) { arrows.at(StageDetails::ROUTE::RIGHT).arg.pos = player_icon.arg.pos + DirectX::XMFLOAT2(offset.x, 0); }
        if (arrows.count(StageDetails::ROUTE::UP)) { arrows.at(StageDetails::ROUTE::UP).arg.pos       = player_icon.arg.pos + DirectX::XMFLOAT2(0, -offset.y); }
        if (arrows.count(StageDetails::ROUTE::DOWN)) { arrows.at(StageDetails::ROUTE::DOWN).arg.pos   = player_icon.arg.pos + DirectX::XMFLOAT2(0, offset.y); }
    }
#ifdef USE_IMGUI
    ImGui::Begin("ClearProto");
    const char* elems_names[STAGE_IDENTIFIER::STAGE_COUNT] = { "S_1_1", "S_2_1", "S_2_2", "S_3_1", "S_3_2", "S_3_3", "BOSS_BATTLESHIP" };
    {
        static int elem = current_stage;
        const char* elem_name = (elem >= 0 && elem < STAGE_IDENTIFIER::STAGE_COUNT) ? elems_names[elem] : "Unknown";
        if (ImGui::SliderInt("change stage", &elem, 0, STAGE_IDENTIFIER::STAGE_COUNT - 1, elem_name))
        {
            current_stage = (STAGE_IDENTIFIER)elem;
            transition_reduction();
        }
        ImGui::Separator();
    }
    if (ImGui::Button("transition")) { transition_reduction(); }
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
void WaveManager::transition_reduction()
{
    clear_state = CLEAR_STATE::REDUCTION;

    clear_flg = false;
    clear_wait_timer = 3.0f;

    close = false;
    viewpoint = { 640.0f, 360.0f };
    wait_timer = 2.5f;

    float ratio = (stage_details[current_stage].position.y / map.arg.texsize.y);
    arrival_viewpoint = { 640.0f, ratio > 0.8f ? 700.0f * ratio : 360.0f };
    arrival_scale = { 1.0f,1.0f };
    // map
    map.threshold = 10.0f;
    map.arg.pos = viewpoint - stage_details[current_stage].position * map.arg.scale;
    map.arg.scale = { 6.0f, 6.0f };
    // player_icon
    player_icon.threshold = 10.0f; // プレイヤーのアニメーションを待つ
    player_icon.arg.pos = viewpoint;
    player_icon.arg.scale = DirectX::XMFLOAT2(0.3f, 0.3f) * map.arg.scale;
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
    player_icon.arg.scale = DirectX::XMFLOAT2(0.3f, 0.3f) * map.arg.scale;

    // 選択状態に遷移
    if (Math::equal_check(map.arg.scale.x, arrival_scale.x, 0.001f))
    {
        if (current_stage != STAGE_IDENTIFIER::BOSS) { transition_selection(); }
        else // ゲームクリア
        {
            SceneManager::scene_switching(new SceneLoading(new SceneTitle()), DISSOLVE_TYPE::DOT, 2.0f);
            return;
        }
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

    // 矢印のスケール
    for (auto& arrow : arrows)
    {
        arrow.second.arg.scale = player_icon.arg.scale;
        arrow.second.threshold = 1.0f;
    }
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
    // 選択している矢印は大きく
    for (auto& arrow : arrows)
    {
        arrow.second.threshold = Math::lerp(arrow.second.threshold, -0.5f, 2.0f * elapsed_time);

        if (arrow.first == route_state) { arrow.second.arg.scale = player_icon.arg.scale * 0.5f; }
        else { arrow.second.arg.scale = player_icon.arg.scale * 0.3f; }
    }
    // next_stageのセット
    if (game_pad->get_button_down() & GamePad::BTN_B)
    {
        next_stage = stage_details[current_stage].journeys.at(route_state);
        transition_enlargement();
    }

    DirectX::XMFLOAT2 min_point = map.arg.pos;
    DirectX::XMFLOAT2 max_point = { min_point + map.arg.texsize * map.arg.scale };

    // 右スティックでマップ移動
    if (game_pad->get_axis_RX() < 0)
    {
        if (min_point.x < 0)
        {
            viewpoint.x -= game_pad->get_axis_RX() * 600.0f * elapsed_time;
        }
    }
    else
    {
        if (max_point.x > 1280)
        {
            viewpoint.x -= game_pad->get_axis_RX() * 600.0f * elapsed_time;
        }
    }

    if (game_pad->get_axis_RY() < 0)
    {
        if (max_point.y > 720)
        {
            viewpoint.y += game_pad->get_axis_RY() * 600.0f * elapsed_time;
        }
    }
    else
    {
        if (min_point.y < 0)
        {
            viewpoint.y += game_pad->get_axis_RY() * 600.0f * elapsed_time;
        }
    }

    // 移動
    map.arg.pos = viewpoint - stage_details[current_stage].position * map.arg.scale;
    player_icon.arg.pos = viewpoint;

#ifdef USE_IMGUI
    ImGui::Begin("ClearProto");
    {
        ImGui::Separator();
        int elem = stage_details[current_stage].journeys.at(route_state);
        const char* elems_names[STAGE_IDENTIFIER::STAGE_COUNT] = { "S_1_1", "S_2_1", "S_2_2", "S_3_1", "S_3_2", "S_3_3", "BOSS_BATTLESHIP"};
        const char* elem_name = (elem >= 0 && elem < STAGE_IDENTIFIER::STAGE_COUNT) ? elems_names[elem] : "Unknown";
        ImGui::SliderInt("candidate stage", &elem, 0, STAGE_IDENTIFIER::STAGE_COUNT - 1, elem_name);
        ImGui::Separator();
    }
    ImGui::End();
#endif // USE_IMGUI
}

void WaveManager::transition_enlargement()
{
    clear_state = CLEAR_STATE::ENLARGEMENT;

    wait_timer = 3.0f;
    arrival_viewpoint = { 640.0f, 360.0f };
    arrival_scale     = { 6.0f, 6.0f };
}

void WaveManager::update_enlargement(float elapsed_time)
{
    current_stage = next_stage;

    wait_timer -= elapsed_time;
    wait_timer = (std::max)(wait_timer, 0.0f);

    // viewpointの移動
    float lerp_rate = 1.5f;
    viewpoint = Math::lerp(viewpoint, arrival_viewpoint, lerp_rate * elapsed_time);
    // positionの移動
    map.arg.pos = viewpoint - stage_details[current_stage].position * map.arg.scale;
    player_icon.arg.pos = viewpoint;

    if (wait_timer > 0.0f) return;
    // scaleの変更
    map.arg.scale = Math::lerp(map.arg.scale, arrival_scale, lerp_rate * elapsed_time);
    player_icon.arg.scale = DirectX::XMFLOAT2(0.4f, 0.4f) * map.arg.scale;

    if (Math::equal_check(map.arg.scale.x, arrival_scale.x, 0.1f)) { close = true; }
}

//---ここまで--//