#pragma once
#include "EnemyFileSystem.h"
#include"BaseEnemy.h"
#include<vector>
//****************************************************************
// 
// �G�̃G�f�B�^�[ 
// 
//****************************************************************


class EnemyEditor final
{
public:
    EnemyEditor();
    ~EnemyEditor();
    void fGuiMenu();
    void fGui_ParamEditor();

    [[nodiscard]] std::function<EnemyData(std::string)> fGetFunction()const;

private:
    //--------------------<�G�̂ς�߁[���[�̒����@�\>--------------------//
    void fControlParams();


    //****************************************************************
    // 
    // �ϐ�
    // 
    //****************************************************************
    
    
    //--------------------<�G�f�B�^�p>--------------------//
    std::function<EnemyData(std::string)> mFunction{};
    std::map<std::string, EnemyData> mKindsMap{}; //�G�̎�ނ��Ƃ̃}�b�v
    std::vector<EnemySource> mEditorSourceVec;
    bool mIsOpenEditor{};
    bool mParamEditor{};
};

