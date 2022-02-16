#pragma once
#include<vector>
#include "EnemyFileSystem.h"
//****************************************************************
// 
// �G�̃G�f�B�^�[ 
// 
//****************************************************************

class EnemyEditor final
{
public:
    EnemyEditor() = default;
    ~EnemyEditor() = default;

    void fGuiMenu();

private:

    //--------------------<�G�f�B�^�p>--------------------//
    std::vector<EnemySource> mEditorSourceVec;
    bool mIsOpenEditor{};
};

