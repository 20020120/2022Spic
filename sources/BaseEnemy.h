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


    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]] bool fGetIsFrustum()const;
    [[nodiscard]] float fGetLengthFromPlayer()const;
    [[nodiscard]] DirectX::XMFLOAT3 fGetPosition()const;
    
    //--------------------<セッター関数>--------------------//
    void fSetPlayerPosition(DirectX::XMFLOAT3 PlayerPosition_);

protected:
    void fUpdateBase(float elapsedTime_);

    void fUpdateStateMachine(float elapsedTime_);
    void fCalcFrustum();
    void fCalcLength();
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
};