#pragma once

#include"EnemyManager.h"
#include "sprite_dissolve.h"
#include "sprite_batch.h"

//****************************************************************
//
// �E�F�[�u���Ǘ�����N���X
//
//****************************************************************
class WaveManager final
{
    //****************************************************************
    //
    // �\����
    //
    //****************************************************************
public:
    enum class WaveState
    {
        Start, // �Q�[���J�n�O
        Game,  // �Q�[����
        Clear, // �Q�[���N���A
    };

    //****************************************************************
    //
    //  �֐�
    //
    //****************************************************************
public:
    WaveManager() = default;
    ~WaveManager() = default;

    // ������
    void fInitialize(GraphicsPipeline& graphics_, AddBulletFunc Func_);
    // �X�V
    void fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_, AddBulletFunc Func_);
    //----------------------------------
    // TODO:�������������Ƃ���
    //----------------------------------
    void render(ID3D11DeviceContext* dc, float elapsed_time);

    //---�����܂�--//

    // �I������
    void fFinalize();

    // �N���A����
    void fWaveClear();
    // �E�F�[�u�J�n
    void fStartWave();


    //--------------------<�Q�b�^�[�֐�>--------------------//
    [[nodiscard]]  EnemyManager* fGetEnemyManager() ;

    //--------------------<�Z�b�^�[�֐�>--------------------//
    void fSetStartGame(bool Arg_);

    //--------------------<ImGui>--------------------//
    void fGuiMenu();

private:

    void fClearUpdate(float elapsedTime_);

    //****************************************************************
    //
    //  �ϐ�
    //
    //****************************************************************
    // ���݂̃E�F�[�u
    int mCurrentWave{};

    // �ŏ��̉��o���I���������ǂ���
    bool mStartGame{};

    EnemyManager mEnemyManager{};
    WaveState mWaveState;

    //--------------------<ImGui>--------------------//
    bool mOpenGui{};

    //----------------------------------
    // TODO:�������������Ƃ���1
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

    //---�����܂�--//


};


