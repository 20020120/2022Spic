
#include "stdafx.h"

#include"JsonFileManager.h"
#include"Log.h"
#include"json.hpp"
#include <fstream>


//--------------------<新しいファイルを作成>--------------------//
void JsonManager::CrateNewFile(const char* FileName)
{

    // 指定されたファイルが存在するか確認
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

        // なければ作成
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
    // 指定されたファイルを開く
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





