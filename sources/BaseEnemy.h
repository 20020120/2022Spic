#pragma once
#include"skinned_mesh.h"
#include "EnemyMove.h"

#include<d3d11.h>
#include<memory>

//****************************************************************
// 
// 敵の基底クラス 
// 
//****************************************************************
typedef std::function<void()> InitFunc;
typedef std::function<void(float)> UpdateFunc;
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
    BaseEnemy(ID3D11Device *pDevice_,int UniqueId_,const char* ModelName_);
    virtual ~BaseEnemy();

    virtual void fInitialize() = 0;
    virtual void fUpdate(float elapsedTime_) = 0;
    void fRender(ID3D11DeviceContext* pDeviceContext_) const;

    void fGetParam(BaseEnemy* This_,std::function<EnemyData(std::string)> Function_);
    //--------------------<ImGui>--------------------//
    virtual void fGuiMenu(){}
    
    //--------------------<プレイヤーからダメージを受ける>--------------------//
    virtual void fDamaged(int Damage_, float InvinsibleTime_);

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
    //--------------------<セッター関数>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_);
    void fSetPosition(DirectX::XMFLOAT3 Position);
protected:
    void fUpdateBase(float elapsedTime_);

    void fUpdateStateMachine(float elapsedTime_);
    void fCalcFrustum();
    void fCalcLength();
    virtual  void fSetCapsulePoint();
    void fUpdateInvicibleTimer(float elapsedTime_);
    //--------------------<移動処理関連>--------------------//
    //プレイヤーのほうを向く処理
    bool fTurnToPlayer(float elapsedTime_);
    //方向取得
    void fGetDirections(DirectX::XMFLOAT4& o);

    //****************************************************************
    // 
    // 変数 
    // 
    //****************************************************************
protected:
    DirectX::XMFLOAT3 mPosition{};    // 位置
    DirectX::XMFLOAT4 mOrientation{}; // 回転
    DirectX::XMFLOAT3 mScale{};       // 大きさ

    DirectX::XMFLOAT3 mPlayerPosition{}; // プレイヤーの位置
    float mLengthFromPlayer{};     // プレイヤーからの距離
    float mLengthFromTargetEnemy{}; // プレイヤーがロックオンしている敵との距離
    EnemyData mData{}; 
    Param mParam{};
    CapsuleCollider mCapsuleCollider{};
    bool IsAttacked{};
    float mInvinsibleTimer = 0.0f;//無敵時間
    //エネミーの各方向
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.1f };

    int mUniqueId{};
    // モデル
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };
private:

    //--------------------<ステートマシンに関連する変数>--------------------//
    int mState{}; // 状態
    bool mIsInitialize{ false }; // 初期化したかどうか

    bool mIsFrustum{}; // カリングされているか
    CubeData mCubeData{};



protected:
    //****************************************************************
    // 
    // 簡易的なステートマシンを仮実装（実装速度が必要じゃないなら変えるかも）
    // 
    //****************************************************************
    std::map<int, FunctionTuple> mFunctionMap{};
    FunctionTuple mCurrentTuple{};

    virtual void fRegisterFunctions() {}; 
    void fChangeState(int i);

};