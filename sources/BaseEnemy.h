#pragma once
#include"skinned_mesh.h"
#include "EnemyMove.h"
#include "Effects.h"
#include<d3d11.h>
#include<memory>

//****************************************************************
//
// 敵の基底クラス
//
//****************************************************************
typedef std::function<void()> InitFunc;
typedef std::function<void(float,GraphicsPipeline&)> UpdateFunc;
typedef std::tuple<InitFunc, UpdateFunc > FunctionTuple;

struct CubeData
{
    DirectX::XMFLOAT3 mHalfSize{};
};

struct CapsuleSaveData
{
    float mLengthFromPositionA{};
    float mLengthFromPositionB{};
    float mRadius{};
    // シリアライズ
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("LengthA", mLengthFromPositionA),
            cereal::make_nvp("LengthB", mLengthFromPositionB),
            cereal::make_nvp("Radius", mRadius));
    }
};
struct EnemyData
{
    std::string mDivideClass; // 派生クラスの名前かつデータのキー値（一応）
    int mMaxHitPoint{};      // 体力
    int mAttackPower{};   // 攻撃力
    float mMoveSpeed{};   // 移動速度
    float mAttackSpeed{}; // 攻撃間隔
    CapsuleSaveData mCapsule{}; //カプセルのデータ
    template<class Archive>
        void serialize(Archive& archive)
    {
            archive(
                cereal::make_nvp("Name", mDivideClass),
                cereal::make_nvp("MaxHp", mMaxHitPoint),
                cereal::make_nvp("AttackPower", mAttackPower),
                cereal::make_nvp("MoveSpeed", mMoveSpeed),
                cereal::make_nvp("AttackSpeed", mAttackSpeed),
                cereal::make_nvp("Capsule", mCapsule));
    }
};

class BaseEnemy :public EnemyMove
{
protected:
    //****************************************************************
    //
    // 構造体
    //
    //****************************************************************
    struct Param
    {
        int mHitPoint{};      // 体力
        int mAttackPower{};   // 攻撃力
        float mMoveSpeed{};   // 移動速度
        float mAttackSpeed{}; // 攻撃間隔
    };

    // ステートの名前を定義する
    struct StateTag
    {
        inline static const std::string Start = "Start";
        inline static const std::string Idle = "Idle";
        inline static const std::string Move = "Move";
        inline static const std::string Damaged = "Damaged";
    };

    //攻撃に関する情報
    struct AttackInformation
    {
        bool mIsAttack = false; //攻撃判定がある状態かのフラグ
        int mDamage{};//攻撃力
        float mInvincible_time{};//攻撃を与えた対象に付与する無敵時間

        DirectX::XMFLOAT3 mPosition{};//攻撃の基準点
        DirectX::XMFLOAT3 mUp{};//攻撃の上軸

        float mLengthFromPositionA{};//ポジションから上への高さ
        float mLengthFromPositionB{};//ポジションから下への高さ
        float mRadius{};//カプセルの半径
    };
public:
    struct CapsuleCollider
    {
        DirectX::XMFLOAT3 mPointA{};
        DirectX::XMFLOAT3 mPointB{};
        float mRadius{};
        // シリアライズ
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("PointA", mPointA),
                cereal::make_nvp("PointB", mPointB),
                cereal::make_nvp("Radius", mRadius));
        }
    };

    //****************************************************************
    //
    // 関数
    //
    //****************************************************************
public:
    BaseEnemy(GraphicsPipeline& graphics_,int UniqueId_,const char* ModelName_);
    virtual ~BaseEnemy();

    virtual void fInitialize() = 0;
    virtual void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) = 0;
    virtual void fRender(GraphicsPipeline& graphics_);

    void fGetParam(BaseEnemy* This_,std::function<EnemyData(std::string)> Function_);
    //--------------------<ImGui>--------------------//
    virtual void fGuiMenu(){}
    void fDieEffect() const;
    //--------------------<プレイヤーからダメージを受ける>--------------------//
    virtual void fDamaged(int Damage_, float InvinsibleTime_);

    //再生中のエフェクトを停止する
    virtual void fStopEffect() = 0;
    // プレイヤー敵との距離を計算する
    void fCalcNearestEnemy(DirectX::XMFLOAT3 NearPosition_);

    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]] bool fGetIsFrustum()const;
    [[nodiscard]] float fGetLengthFromPlayer()const;
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition()const;
    [[nodiscard]] bool fGetIsAlive()const;
    [[nodiscard]] CapsuleCollider fGetCapsuleData()const;
    [[nodiscard]] int fGetUniqueId()const;
    [[nodiscard]] float fGetLengthFromNearEstEnemy()const;
    [[nodiscard]] const float fGetPercentHitPoint()const;
    [[nodiscard]] std::string fGetType()const;
    [[nodiscard]] CapsuleCollider fGetAttackCapsuleData() const;
    [[nodiscard]] AttackInformation fGetAttackInfo()const;
    [[nodiscard]] bool fGetAttacking()const;//攻撃中か

    //--------------------<セッター関数>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_);
    void fSetPosition(DirectX::XMFLOAT3 Position);
    void fSetAttackPower(int Damage_, float InvincibleTime_);
    void fSetAttackRange(DirectX::XMFLOAT3 Position_, DirectX::XMFLOAT3 Up_, float LengthFromPositionA_, float LengthFromPositionB_);
    void fAttackStart();//攻撃開始
    void fAttackEnd();//攻撃終了
    void fSetHitVector(DirectX::XMFLOAT3 Arg_);
protected:
    void fUpdateBase(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fUpdateStateMachine(float elapsedTime_,GraphicsPipeline& Graphics_);
    void fCalcFrustum();
    void fCalcLength();
    virtual  void fSetCapsulePoint();
    virtual  void fSetAttackCapsulePoint();
    void fUpdateInvicibleTimer(float elapsedTime_);
    //--------------------<移動処理関連>--------------------//
    //プレイヤーのほうを向く処理
    bool fTurnToTarget(float elapsedTime_, DirectX::XMFLOAT3 target_pos,float turn_speed = 10.0f);
    //方向取得
    void fGetDirections(DirectX::XMFLOAT4& o);

    // 引数のベクトルの方向に移動する
    void fMoveVelocity(float elapsedTime_, DirectX::XMFLOAT3 Velocity_, float Speed_);

    //****************************************************************
    //
    // 変数
    //
    //****************************************************************
protected:
    DirectX::XMFLOAT3 mPosition{};    // 位置
    DirectX::XMFLOAT4 mOrientation{0.0f,0.0f,0.0f,1.0f}; // 回転
    DirectX::XMFLOAT3 mScale{};       // 大きさ

    DirectX::XMFLOAT3 mPlayerPosition{}; // プレイヤーの位置
    float mLengthFromPlayer{};     // プレイヤーからの距離
    float mLengthFromTargetEnemy{}; // プレイヤーがロックオンしている敵との距離
    EnemyData mData{};
    Param mParam{};
    CapsuleCollider mCapsuleCollider{};
    CapsuleCollider mAttackCapsuleCollider{};
    AttackInformation mAttackInformation{};

    bool IsAttacked{};
    float mInvinsibleTimer = 0.0f;//無敵時間
    //エネミーの各方向
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.1f };
    
    int mUniqueId{};
    // モデル
    std::shared_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };
    SkinnedMesh::anim_Parameters mAnimPara;
    std::unique_ptr<Effect> mDieEffect{ nullptr };

    float mAnimationSpeed{1.0f}; // アニメーションの再生速度

   //--------------------<ほかの敵に当たった時のベクトル>--------------------//
    DirectX::XMFLOAT3 mHitVector{}; 


    private:
    //--------------------<ステートマシンに関連する変数>--------------------//
    int mState{}; // 状態
    bool mIsInitialize{ false }; // 初期化したかどうか

    bool mIsFrustum{}; // カリングされているか
    CubeData mCubeData{};
protected:
    float mDissolveThreshold{};
protected:
    //****************************************************************
    //
    // 簡易的なステートマシンを仮実装（実装速度が必要じゃないなら変えるかも）
    //
    //****************************************************************
    std::map<std::string, FunctionTuple> mFunctionMap{};
    FunctionTuple mCurrentTuple{};

    virtual void fRegisterFunctions() {};
    void fChangeState(std::string Tag_);

};