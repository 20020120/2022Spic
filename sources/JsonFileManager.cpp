
#include"JsonFileManager.h"
#include"json.hpp"
#include <fstream>


//--------------------<�V�����t�@�C�����쐬>--------------------//
void JsonManager::CrateNewFile(const char* FileName)
{
    // �Ȃ���΍쐬
    std::ofstream ofs;
    ofs.open(FileName);
    if (ofs)
    {
        ofs.close();
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
    }
    else
    {
        throw std::runtime_error("JsonFile Not Found");
    }

}

void JsonManager::Read(const char* FileName)
{
    
}





