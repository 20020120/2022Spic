#pragma once
#include "EnemyFileSystem.h"
#include"BaseEnemy.h"
#include<vector>
//****************************************************************
// 
// 敵のエディター 
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
    //--------------------<敵のぱらめーたーの調整機能>--------------------//
    void fControlParams();


    //****************************************************************
    // 
    // 変数
    // 
    //****************************************************************

    // 何回もJsonを開くと重いので初回だけ開く
    bool mFirstOpenJson{}; // Jsonを開くのは最初かどうかの変数 

    //--------------------<エディタ用>--------------------//
    ParamGetFunction mFunction{};
    std::map<std::string, EnemyData> mKindsMap{}; //敵の種類ごとのマップ
    std::vector<EnemySource> mEditorSourceVec;
    bool mIsOpenEditor{};
    bool mParamEditor{};
};

