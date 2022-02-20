#pragma once
#include "EnemyFileSystem.h"
#include"BaseEnemy.h"
#include<vector>
//****************************************************************
// 
// 敵のエディター 
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
    //--------------------<敵のぱらめーたーの調整機能>--------------------//
    void fControlParams();


    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************
    
    
    //--------------------<エディタ用>--------------------//
    std::function<EnemyData(std::string)> mFunction{};
    std::map<std::string, EnemyData> mKindsMap{}; //敵の種類ごとのマップ
    std::vector<EnemySource> mEditorSourceVec;
    bool mIsOpenEditor{};
    bool mParamEditor{};
};

