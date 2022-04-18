#pragma once

#include "EnemyManager.h"
#include "sprite_dissolve.h"
#include "sprite_batch.h"
#include "practical_entities.h"

//****************************************************************
//
// �E�F�[�u���Ǘ�����N���X
//
//****************************************************************
class WaveManager final : public PracticalEntities
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
    bool during_clear_performance() { return mWaveState == WaveState::Clear; }
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
    enum class CLEAR_STATE
    {
        REDUCTION,    // �k��
        SELECTION,    // �X�e�[�W�I��
        ENLARGEMENT,  // �g��
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

    enum STAGE_IDENTIFIER /*S_���B��_������̐�*/
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
        DirectX::XMFLOAT2 position = {}; // �e�X�e�[�W�̈ʒu
        // �e�X�e�[�W�̃��[�g�Ƃ��̍s��
        std::map<ROUTE, STAGE_IDENTIFIER> journeys;
    };
    StageDetails stage_details[STAGE_IDENTIFIER::STAGE_COUNT];
    STAGE_IDENTIFIER current_stage      = STAGE_IDENTIFIER::S_1_1;
    STAGE_IDENTIFIER next_stage         = STAGE_IDENTIFIER::S_1_1;
    STAGE_IDENTIFIER candidate_stage    = STAGE_IDENTIFIER::S_1_1; // ���ɑI�ׂ郋�[�g���
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

    //---�����܂�--//


};


