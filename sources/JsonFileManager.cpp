
#include "stdafx.h"

#include"JsonFileManager.h"
#include"Log.h"
#include"json.hpp"
#include <fstream>


//--------------------<�V�����t�@�C�����쐬>--------------------//
void JsonManager::CrateNewFile(const char* FileName)
{

    // �w�肳�ꂽ�t�@�C�������݂��邩�m�F
    std::ifstream ifs;
    ifs.open(FileName);
    if(ifs)
    {
        LoggerSystem::OutputLog_Str("JsonManager::CrateNewFile///Massage-->This_File_Already_Exists");
        ifs.close();
        return;
    }
    else
    {
        ifs.close();

        // �Ȃ���΍쐬
        std::ofstream ofs;
        ofs.open(FileName);
        if (ofs)
        {
            LoggerSystem::OutputLog_Str("JsonManager::CrateNewFile///Massage-->Complete_Create_File");
            LoggerSystem::OutputLog_Str(FileName);
            ofs.close();
        }
    }

   

}

void JsonManager::Write(const char* FileName)
{
    // �w�肳�ꂽ�t�@�C�����J��
    std::ofstream ofs;
    ofs.open(FileName);
    if (ofs)
    {
        nlohmann::json json;
        json["HitPoint"] = 10;
        ofs << json;

        LoggerSystem::OutputLog_Str("JsonManager::Write///Massage-->Complete");
        LoggerSystem::OutputLog_Str(FileName);
    }
    else
    {
        LoggerSystem::OutputLog_Str("JsonManager::Write///Massage-->This_File_Is_Not");
    }

}

void JsonManager::Read(const char* FileName)
{
    
}





