#pragma once
#include"BaseEnemy.h"
#include"LaserBeam.h"
#include"Common.h"
//****************************************************************
// 
// ���{�X
// 
//****************************************************************
class MiddleBoss final : public BaseEnemy
{
public:
    //****************************************************************
    // 
    // �\����
    // 
    //****************************************************************
    struct  State: public StateTag
    {
        inline static const std::string Start = "Start";
        inline static const std::string Tour = "Tour";
        inline static const std::string TourShot = "TourShot";
        inline static const std::string TourBeamReady = "TourBeamReady";
        inline static const std::string TourBeam = "TourBeam";
    };


    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    MiddleBoss(GraphicsPipeline& Graphics_, std::function<EnemyData(std::string)> Function_,AddBulletFunc Func_);
    ~MiddleBoss() override = default;
    
    void fInitialize() override;
    void fUpdate(GraphicsPipeline& Graphics_,float elapsedTime_) override;
    void fGuiMenu(GraphicsPipeline& Graphics_);
    void fDamaged(int Damage_, float InvinsibleTime_) override;
protected:
    void fSetCapsulePoint() override;
    void fRegisterFunctions() override;
    void fShotStraightBullet(GraphicsPipeline& Graphics_); // ���i����e���͂�����
private:
    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    float mTourRadian{};         // ���񒆂̉�]�p
    LaserBeam mLaserPointer{};   // �Ə��̃��[�U�[
    LaserBeam mLaserBeam{};      // ���[�U�[�r�[��

    float mLaserPointerLength{}; // �Ə��̒���
    float mLaserBeamLength{};    // �r�[���̒���
    float mBeamRadius{};         // �r�[���̑���
    float mLaserShotTimer{};     // �r�[�����ˏ������甭�˂ł̎���
    AddBulletFunc mfAddFunc;

    //****************************************************************
    // 
    // �萔
    // 
    //****************************************************************
    const float TourLength{ 150.0f };    // ���񒆂̌��_����̋���
    const float TourLaserTimer{ 6.0f }; // ���˂܂ł̎���

private:
    //****************************************************************
    // 
    // �X�e�[�g�}�V���̊֐�
    // 
    //****************************************************************
    //�ŏ��̓o��V�[��
    void fStartInit();  
    void fStartUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // ���`�ԃX�e�[�W�̏�����邮����
    void fTourInit();
    void fTourUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // ���`�ԃr�[������
    void fTourLaserReadyInit();
    void fTourLaserReadyUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);
    // ���`�ԃr�[��
    void fTourLaserInit();
    void fTourLaserUpdate(float elapsedTime_, GraphicsPipeline& Graphic_);
    // ���`�ԒP���ˌ�
    void fTourShotInit();
    void fTourShotUpdate(float elapsedTime_, GraphicsPipeline& Graphics_);


public:
    void fRender(GraphicsPipeline& graphics_) override;
};
