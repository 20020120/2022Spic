#pragma once

#include"practical_entities.h"
#include"graphics_pipeline.h"
#include"skinned_mesh.h"
#include"EnemyStructuer.h"
#include"Effects.h"
#include<memory>
#include<functional>

typedef std::function<void()> InitFunc;
typedef std::function<void(float, GraphicsPipeline&)> UpdateFunc;
typedef std::tuple<InitFunc, UpdateFunc > FunctionTuple;

class BaseEnemy :public PracticalEntities
{
public:
    BaseEnemy(GraphicsPipeline& Graphics_, 
              const char* FileName_,
              const EnemyParamPack& Param_,
              const DirectX::XMFLOAT3& EntryPosition_);
    BaseEnemy(GraphicsPipeline& Graphics_, const char* FileName_);
    ~BaseEnemy() override;
    void fBaseUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    virtual void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) = 0;
    void fRender(GraphicsPipeline& Graphics_);

    void fDamaged(int Damage_, float InvincibleTime_);
    virtual void fDie();

    void fUpdateVernierEffectPos();
    virtual void fUpdateAttackCapsule() = 0;

    //--------------------<セッター関数>--------------------//
    void fSetStun(bool Arg_);
    void fSetPlayerPosition(const DirectX::XMFLOAT3& PlayerPosition_);
    void fSetAttack(bool Arg_);

    //--------------------<ゲッター関数>--------------------//
    [[nodiscard]] bool fGetAttack() const;

protected:
    std::shared_ptr<SkinnedMesh> mpModel{ nullptr };

    DirectX::XMFLOAT3 mPosition{};
    DirectX::XMFLOAT3 mScale{1.0f,1.0f,1.0f};
    DirectX::XMFLOAT4 mOrientation{ 0.0f,0.0f,0.0f,1.0f };

    DirectX::XMFLOAT3 mPlayerPosition{};

    SkinnedMesh::anim_Parameters mAnimPara{};
    float mDissolve{};

    float mAnimationSpeed{1.0f};
private:
    bool mIsStun{}; // スタン状態かどうか
    int mCurrentHitPoint{};
    float mInvincibleTime{};

    bool mIsAttack{};
protected:
    // StateMachine
    std::map<std::string, FunctionTuple> mFunctionMap{};
private:
    FunctionTuple mCurrentTuple{};
protected:
    virtual void fRegisterFunctions() = 0;
    void fChangeState(const char* Tag_);

protected:
    // 攻撃の当たり判定
    Capsule mAttackCapsule{}; 
    // ダメージを食らう当たり判定
    Capsule mBodyCapsule{};
private:
    skeleton::bone mVernierBone{};

    //--------------------<エフェクト>--------------------//
    std::unique_ptr<Effect> mVernierEffect{ nullptr };
    inline static const char* mkVernierPath = "./resources/Effect/sluster_enemy2.efk";
};

