#pragma once
#include"skinned_mesh.h"
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

class BaseEnemy
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
    };

    //****************************************************************
    // 
    // 関数
    // 
    //****************************************************************
public:
    BaseEnemy(ID3D11Device *pDevice_,const char* ModelName_);
    virtual ~BaseEnemy();

    virtual void fInitialize() = 0;
    virtual void fUpdate(float elapsedTime_) = 0;
    void fRender(ID3D11DeviceContext* pDeviceContext_) const;

    //--------------------<プレイヤーからダメージを受ける>--------------------//
    void fDamaged(int Damage_);

    // プレイヤー敵との距離を計算する
    void fCalcNearestEnemy(DirectX::XMFLOAT3 NearPosition_);

    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]] bool fGetIsFrustum()const;
    [[nodiscard]] float fGetLengthFromPlayer()const;
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition()const;
    [[nodiscard]] bool fGetIsAlive()const;
    [[nodiscard]] CapsuleCollider fGetCapsuleData()const;
    //--------------------<セッター関数>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_);

protected:
    void fUpdateBase(float elapsedTime_);

    void fUpdateStateMachine(float elapsedTime_);
    void fCalcFrustum();
    void fCalcLength();
    virtual  void fSetCapsulePoint() = 0;
    virtual void fTurnToThePlayer(){}
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
    Param mParam{};
    CapsuleCollider mCapsuleCollider{};
    
    //プレイヤーの各方向
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 up;
private:
    // モデル
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };

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
    void fGetEnemyDirections();

};