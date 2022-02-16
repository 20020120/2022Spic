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


class BaseEnemy
{
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

protected:
    void fUpdateStateMachine(float elapsedTime_);

    //****************************************************************
    // 
    // 変数 
    // 
    //****************************************************************
protected:
    DirectX::XMFLOAT3 mPosition{};    // 位置
    DirectX::XMFLOAT4 mOrientation{}; // 回転
    DirectX::XMFLOAT3 mScale{};       // 大きさ
private:
    // モデル
    std::unique_ptr<SkinnedMesh> mpSkinnedMesh{ nullptr };

    //--------------------<ステートマシンに関連する変数>--------------------//

    int mState{}; // 状態
    bool mIsInitialize{ false }; // 初期化したかどうか

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