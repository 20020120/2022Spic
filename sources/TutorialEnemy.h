#pragma once
#include"BaseEnemy.h"

class TutorialEnemy_NoMove final :public BaseEnemy
{
    struct DivideState
    {
        inline static const char* Start = "Start";
        inline static const char* Idle = "Idle";
        inline static const char* Stun = "Stun";
    };
    enum  AnimationName {
        idle,
        walk,
        damage,
        stun,
        attack_ready,
        attack,
    };

public:
    TutorialEnemy_NoMove(GraphicsPipeline& graphics, const char* file_name, const EnemyParamPack& param,
        const DirectX::XMFLOAT3& entry_position, const wchar_t* icon_file_name);

    TutorialEnemy_NoMove(GraphicsPipeline& graphics, const char* file_name);

    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
    void fUpdateAttackCapsule() override;
protected:
    void fRegisterFunctions() override;


private:
    float mTimer{};
private:
    //  NoMoveは待機・スタン
    void fStartInit();
    void fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fIdleInit();
    void fIdleUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);

    void fStunInit();
    void fStunUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
};


