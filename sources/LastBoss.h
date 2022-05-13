#pragma once
#include <random>

#include"BaseEnemy.h"
#include "Common.h"
#include"LaserBeam.h"
//****************************************************************
// 
// ラストボス
// 
//****************************************************************

  // 前方宣言
class EnemyManager;

class LastBoss final : public BaseEnemy
{
public:
    enum class Mode // ボスのモード
    {
        None,          // 何もなし（存在しない）
        Ship,          // 戦艦（ダメージは受けない）
        Human,         // 人型（体力の100%~20%）
        HumanToDragon, // 人型からドラゴンに遷移している途中（ダメージは受けない）
        Dragon,        // ドラゴン（体力の20%~0%）
    };

private:
    struct DivideState
    {
        //--------------------<戦艦>--------------------//
        inline static const char* ShipStart  = "ShipStart";
        inline static const char* ShipIdle   = "ShipIdle";
        inline static const char* ShipAttack = "ShipAttack";
        inline static const char* ShipBeamStart   = "ShipBeamStart";
        inline static const char* ShipBeamCharge   = "ShipBeamCharge";
        inline static const char* ShipBeamShoot   = "ShipBeamShoot";
        inline static const char* ShipBeamEnd   = "ShipBeamEnd";
        inline static const char* ShipToHuman   = "ShipToHuman";

        //--------------------<人型>--------------------//
        inline static const char* HumanIdle = "HumanIdle"; // 待機
        inline static const char* HumanMove = "HumanMove"; // 移動
        inline static const char* HumanAllShot = "HumanAllShot"; // 乱射する
        inline static const char* HumanRotAttack = "HumanRotAttack"; // 回転攻撃
        inline static const char* HumanWithdrawal_Begin = "HumanWithdrawal_Begin"; // 一旦対比する
        inline static const char* HumanWithdrawal_Middle = "HumanWithdrawal_Middle"; // 範囲外で待つ
        // 一定時間内に手下を蹴散らせなければ必殺技
        inline static const char* HumanWithdrawal_End = "HumanWithdrawal_End";
        inline static const char* HumanBlowAttack = "HumanBlowAttack";

        inline static const char* HumanSpAway = "HumanSpAway"; // 遠くに飛ぶ
        inline static const char* HumanSpWait = "HumanSpWait"; // 待っている
        inline static const char* HumanSpOver = "HumanSpOver"; // 時間切れ
        inline static const char* HumanSpCharge = "HumanSpCharge"; // ため
        inline static const char* HumanSpShoot = "HumanSpShoot"; // 発射
        inline static const char* HumanSpDamage = "HumanSpDamage"; // ダメージ

        inline static const char* HumanRush = "HumanRush"; // ダメージ


        //--------------------<人型の死亡>--------------------//
        inline static const char* HumanDieStart = "HumanDieStart";   // 人型の死亡開始
        inline static const char* HumanDieMiddle = "HumanDieMiddle"; // 人型の死亡待機
        inline static const char* HumanToDragon = "HumanToDragon";   // 人型からドラゴン

        //--------------------<ドラゴン>--------------------//
        inline static const char* DragonIdle = "DragonIdle"; // 待機
        inline static const char* DragonDieStart = "DragonDieStart"; // 死亡
        inline static const char* DragonDieEnd = "DragonDieEnd"; // 死亡

        inline static const char* DragonHideStart= "DragonHideStart"; // 消える
        inline static const char* DragonAppear= "DragonAppear"; // 現れる
    };

    enum  AnimationName {
        ship_idle,
        ship_beam_charge_start,
        ship_beam_charge_idle,
        ship_beam_shot_start,
        ship_beam_shot_idle,
        ship_to_human,
        human_idle,
        human_move,
        human_beam_charge,
        human_beam_shoot,
        human_beam_end,
        human_bullet,
        human_shockwave,
        human_damage,
        human_die,
        human_die_idle,
        human_to_dragon,
        dragon_idle,
        dragon_dive_ready,
        dragon_dive,
        dragon_dive_end,
        dragon_hide,
        dragon_beam_ready,
        dragon_beam_charge,
        dragon_beam_start,
        dragon_beam_idle,
        dragon_beam_end,
        dragon_breath_ready,
        dragon_breath_start,
        dragon_breath_idle,
        dragon_breath_end,
        dragon_damage,
        dragon_die,
        ship_to_human_quick,
        human_to_ship_quick,
        human_to_dragon_quick,
        dragon_to_human_quick,
    };

    enum class AttackKind // 乱数から抽出する攻撃の種類
    {
        //--------------------<人型>--------------------//
        HumanRotAttack,     // 回転攻撃
        HumanAllShot,       // 乱射
        HumanSpecialAttack  // 必殺技
    };

    //--------------------<砲身のクラス>--------------------//
    class Turret final
    {
    public:
        Turret(GraphicsPipeline& Graphics_);
        void fUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
        void fRender(GraphicsPipeline& graphics_, 
                     const DirectX::XMFLOAT4X4& ParentWorld_,
                     const DirectX::XMFLOAT3& Position_);
        void fSetDissolve(float Dissolve_);

        DirectX::XMFLOAT3 fGetPosition()const;
    private:
        DirectX::XMFLOAT4 mOrientation{0.0f,0.0f,0.0f,1.0f}; // 角度
        std::unique_ptr<SkinnedMesh> mpModel{ nullptr };
        SkinnedMesh::anim_Parameters mAnimPara{};
        DirectX::XMFLOAT3 mPosition;
        float mDissolve{};
    };
  
    //--------------------<第二砲身のクラス>--------------------//
    class SecondGun final
    {
    public :
        SecondGun(GraphicsPipeline& Graphics_);
        void fUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
        void fRender(GraphicsPipeline& graphics_,
            const DirectX::XMFLOAT4X4& ParentWorld_,
            const DirectX::XMFLOAT3& Position_);

        void fSetDissolve(float Dissolve_);
    private:
        std::unique_ptr<SkinnedMesh> mpModel{ nullptr };
        SkinnedMesh::anim_Parameters mAnimPara{};
        float mDissolve;
    };


public:
    LastBoss(GraphicsPipeline& Graphics_, 
             const DirectX::XMFLOAT3& EmitterPoint_,
             const EnemyParamPack& ParamPack_,
             EnemyManager* pEnemyManager_);

    LastBoss(GraphicsPipeline& Graphics_);
    ~LastBoss() override;
    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fUpdateAttackCapsule() override;
    
    void fSetStun(bool Arg_) override;
     
    //--------------------<タレットの関数>--------------------//
    skeleton::bone mShipFace{}; // タレットのボーンの位置

protected:
    void fRegisterFunctions() override;

private:
    void fGuiMenu();

    void fResetLaser();

    [[nodiscard]] float fComputePercentHp() const; // 最大体力に対する現在の体力の割合を0.0f~1.0fで返す
    void fChangeHumanToDragon();

    void fSpawnChildUnit(GraphicsPipeline& Graphics_, int Amounts_) const;

    // 移動制限
    bool fLimitStageHuman(float elapsedTime_);
private:
    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************
    float mTimer{}; // 汎用タイマー

    // ビーム
    LaserBeam mBeam{};
    DirectX::XMFLOAT3 mLaserBegin{};   // ビームの始点
    DirectX::XMFLOAT3 mLaserEnd{};     // ビームの終点
    float mLaserThreshold{};           // ビームの長さ0.0f~1.0f
    float mLaserAlpha{};
    float mLaserRadius{};              // ビームの太さ

    // 人型のビーム
    LaserBeam mRightBeam{};
    LaserBeam mLeftBeam{};
    float mRightBeamThreshold{};
    float mLeftBeamThreshold{};

    std::mt19937 mt{ std::random_device{}() };

    //--------------------<ボスのタレット>--------------------//
    std::unique_ptr<Turret> mpTurretRight{ nullptr };
    std::unique_ptr<Turret> mpTurretLeft{ nullptr };
    skeleton::bone mTurretBoneRight{};  // タレットのボーン
    skeleton::bone mTurretBoneLeft{};   // タレットのボーン

    //--------------------<ボスのセカンドガン>--------------------//
    std::unique_ptr<SecondGun> mpSecondGunRight{ nullptr };
    std::unique_ptr<SecondGun> mpSecondGunLeft{ nullptr };
    skeleton::bone mSecondGunBoneRight{};  // タレットのボーン
    skeleton::bone mSecondGunBoneLeft{};   // タレットのボーン


    LaserBeam mLaserPointer{};
    DirectX::XMFLOAT3 mPointerBegin{}; // レーザーポインターの始点
    DirectX::XMFLOAT3 mPointerEnd{};   // レーザーポインターの終点
    float mPointerThreshold{};         // レーザーポインターの長さ0.0f~1.0f
    float mPointerAlpha{};
    float mRgbColorPower{};
    float mRgbColorSpeed{ 10.0f };
    float mHeartTimer{};

    //--------------------<人型の必殺技>--------------------//
    DirectX::XMFLOAT3 mAwayBegin{}; // 飛びのき地点の始点
    const DirectX::XMFLOAT3 mAwayEnd{0.0f,40.0f,200.0f}; // 飛びのき地点の終点
    float mAwayLerp{}; // Lerp係数
    DirectX::XMFLOAT4 mBeginOrientation{};
    DirectX::XMFLOAT4 mEndOrientation{ 0.0f,0.0f,0.0f,1.0f };

    EnemyManager* mpEnemyManager{nullptr};

public:
    // 現在のモード
    Mode mCurrentMode{ Mode::Ship };

private:
    // 弾を
    AddBulletFunc mfAddBullet{};
    float mShotTimer{ 0.0f };

    // プレイヤーから離れる
    DirectX::XMFLOAT3 mMoveBegin{};
    DirectX::XMFLOAT3 mMoveEnd{};
    float mMoveThreshold{};
    bool mFirstSp{ false };
    bool mSecondSp{ false };

    //****************************************************************
    // 
    // 定数
    // 
    //****************************************************************
    const float mkRotSpeed { 20.0f }; // 回転速度
    const float  mkShipBeamChargeSec{ 5.0f }; // 戦艦のビームをチャージする長さ
    const float  mkShipBeamShootSec{ 5.0f }; // 戦艦のビームを発射している長さ
    const float  mkHumanDieIdleSec{ 5.0f };  // 人型の死亡時間

    const float mkPercentToDragon{ 0.2f }; // ドラゴン形態に遷移する体力の割合

    const float mkWaitHeartEffect = 0.5f;
    const float mkHumanAllShotDelay{ 0.1f };
    const float mkHumanAllShotEnd{ 1.3f };
    const float mkHumanAllShotBegin{ 0.3f };
   
    const float mkHumanSpWaitTime{ 15.0f };
    const float mkHumanSpBeamTime{ 5.0f };
    const float mkSpChargeTime = 7.0f;
    const float mkDistanceToPlayer{ 80.0f };
    const float mkLimitStage{ 300.0f };
    const float mkTimerBlow{ 0.8f };
    const float mkDragonHideTime{ 0.5f };
private:

    //****************************************************************
    // 
    // ステートマシン
    // 
    //****************************************************************
    // 戦艦の最初の動き（演出）
    void fShipStartInit();
    void fShipStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // 戦艦の待機状態
    void fShipIdleInit();
    void fShipIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // 戦艦の攻撃
    void fShipAttackInit();
    void fShipAttackUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    // 戦艦の最後の攻撃（これを耐えれば次へ）
    void fShipBeamStartInit();
    void fShipBeamStartUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fShipBeamChargeInit();
    void fShipBeamChargeUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fShipBeamShootInit();
    void fShipBeamShootUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fShipBeamEndInit();
    void fShipBeamEndUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fChangeShipToHumanInit();
    void fChangeShipToHumanUpdate(float elapsedTime_,
        GraphicsPipeline& Graphics_);

    //--------------------<人型>--------------------//
    void fHumanIdleInit();
    void fHumanIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fHumanMoveInit();
    void fHumanMoveUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanAllShotInit();
    void fHumanAllShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanBlowAttackInit();
    void fHumanBlowAttackUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fMoveAwayInit();
    void fMoveAwayUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanRushInit();
    void fHumanRushUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //--------------------<人型必殺技>--------------------//
    void fHumanSpAttackAwayInit(); // 飛びのく
    void fHumanSpAttackAwayUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanSpAttackSummonInit(); // 敵を召喚
    void fHumanSpAttackSummonUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanSpAttackWaitInit(); // 一定時間待機
    void fHumanSpAttackWaitUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanSpAttackCancelInit(); // 全敵を倒されたらイベント
    void fHumanSpAttackCancelUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fHumanSpAttackTimeOverInit(); // 制限時間を超えたら攻撃
    void fHumanSpAttackTimeOverUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);

    void fHumanSpAttackChargeInit();
    void fHumanSpAttackChargeUpdate(float elapsedTime_,GraphicsPipeline& Graphics_);

    void fHumanSpBeamShootInit();
    void fHumanSpBeamShootUpdate(float elapsedTime_, 
        GraphicsPipeline& Graphics_);
    
    void fHumanSpDamageInit();
    void fHumanSpDamageUpdate(float elapsedTime_,
        GraphicsPipeline& Graphics_);



    //--------------------<人型の死亡エフェクト>--------------------//
    void fHumanDieStartInit();
    void fHumanDieStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fHumanDieMiddleInit();
    void fHumanDieMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fHumanToDragonInit();
    void fHumanToDragonUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //--------------------<ドラゴン>--------------------//
    void fDragonIdleInit();
    void fDragonIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    // ディゾルブで消える
    void fDragonFastBreathStartInit();
    void fDragonFastBreathStartUpdate(float elapsedTime_,GraphicsPipeline& Graphics_);

    // 現れる
    void fDragonBreathAppearInit();
    void fDragonBreathAppearUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    //--------------------<ドラゴン死亡エフェクト>--------------------//
    void fDragonDieStartInit();
    void fDragonDieStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    void fDragonDieMiddleInit();
    void fDragonDieMiddleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    

public:
    void fRender(GraphicsPipeline& Graphics_) override;
};
