#pragma once
//#include"BaseEnemy.h"
//#include "EnemiesEditor.h"
////****************************************************************
//// 
//// ���̓G�̒��{�X 
//// 
////****************************************************************
//class SwordEnemyAce final :public BaseEnemy
//{
//public:
//    struct DivideState:public StateTag
//    {
//        inline static const std::string CounterBegin     = "CounterBegin";      // �J�E���^�[�͂���
//        inline static const std::string CounterMiddle    = "CounterMiddle";     // �J�E���^�[��t��
//        inline static const std::string CounterEnd       = "CounterEnd";        // �J�E���^�[��t�I��
//        inline static const std::string CounterActivate  = "CounterActivate";   // �J�E���^�[�U���n��
//        inline static const std::string CounterAttack    = "CounterAttack";     // �J�E���^�[�U��
//        inline static const std::string CounterAttackEnd = "CounterAttackEnd";  // �J�E���^�[�U���I��
//    };
//    //****************************************************************
//    // 
//    // �֐�
//    // 
//    //****************************************************************
//    SwordEnemyAce(GraphicsPipeline& graphics_, int UniqueId_,
//        DirectX::XMFLOAT3 EmitterPoint_/*�X�|�[���ʒu*/,
//        ParamGetFunction Func_);
//    ~SwordEnemyAce() override = default;
//
//    void fUpdate(GraphicsPipeline& Graphics_, float elapsedTime_) override;
//    void fRegisterFunctions() override; // �X�e�[�g��o�^
//
//    void fSetAttackCapsuleCollider();
//
//    void fSetVernierEffectPos();
//
//    void fStopEffect() override;
//    //****************************************************************
//    // 
//    // �ϐ�
//    // 
//    //****************************************************************
//
//    //****************************************************************
//    // 
//    // �X�e�[�g�}�V��
//    // 
//    //****************************************************************
//    //--------------------<�o��>--------------------//
//    void fStartInit();
//    void fStartUpdate(GraphicsPipeline& Graphics_, float elapsedTime_);
//    //--------------------<�ҋ@>--------------------//
//
//
//    //--------------------<�ړ�>--------------------//
//
//};
//
