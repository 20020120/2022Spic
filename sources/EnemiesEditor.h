#pragma once
#include "EnemyFileSystem.h"
#include"BaseEnemy.h"
#include<vector>
//****************************************************************
// 
// �G�̃G�f�B�^�[ 
// 
//****************************************************************

typedef std::function<EnemyData(std::string)> ParamGetFunction;
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

    // �����Json���J���Əd���̂ŏ��񂾂��J��
    bool mFirstOpenJson{}; // Json���J���͍̂ŏ����ǂ����̕ϐ� 

    //--------------------<�G�f�B�^�p>--------------------//
    ParamGetFunction mFunction{};
    std::map<std::string, EnemyData> mKindsMap{}; //�G�̎�ނ��Ƃ̃}�b�v
    std::vector<EnemySource> mEditorSourceVec;
    bool mIsOpenEditor{};
    bool mParamEditor{};
};

