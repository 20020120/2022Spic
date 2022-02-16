#pragma once
#include<vector>
#include "EnemyFileSystem.h"
//****************************************************************
// 
// 敵のエディター 
// 
//****************************************************************

class EnemyEditor final
{
public:
    EnemyEditor() = default;
    ~EnemyEditor() = default;

    void fGuiMenu();

private:

    //--------------------<エディタ用>--------------------//
    std::vector<EnemySource> mEditorSourceVec;
    bool mIsOpenEditor{};
};

