#include"EnemiesEditor.h"

#include"imgui_include.h"
#include "user.h"


EnemyEditor::EnemyEditor()
{
    // “G‚Ìƒf[ƒ^‚ðŽæ“¾
    EnemyFileSystem::fLoadFromJson(mKindsMap, "./resources/Data/EnemiesParam");

    mFunction = [=](std::string ClassName_)->EnemyData
    {
        EnemyFileSystem::fLoadFromJson(mKindsMap, "./resources/Data/EnemiesParam");
        const auto it = mKindsMap.find(ClassName_);
        if (it != mKindsMap.end())
        {
            return it->second;
        }
        else
        {
            EnemyData data{};
            data.mDivideClass = ClassName_;
            data.mMaxHitPoint = 10;
            mKindsMap.insert(std::make_pair(ClassName_, data));
            return data;
        }
    };
}

EnemyEditor::~EnemyEditor()
{
    EnemyFileSystem::fSaveToJson(mKindsMap, "EnemiesParam");
}

void EnemyEditor::fGuiMenu()
{
#ifdef USE_IMGUI

    imgui_menu_bar("Game","EnemyEditor", mIsOpenEditor);

    if (mIsOpenEditor)
    {
        ImGui::Begin("WaveEditor");
        
        if (ImGui::CollapsingHeader("File"))
        {
            static char fileName[256]{};
            ImGui::InputText("FileName", fileName, sizeof(char) * 256);
            if (ImGui::Button("Save"))
            {
                EnemyFileSystem::fSaveToJson(mEditorSourceVec, fileName);
            }

        }

        if (ImGui::CollapsingHeader("Setting"))
        {
            static int emitterPoint = 0;
            static float time = 0.0f;
            int enemyType = 0;

            ImGui::InputInt("EmitterNumber", &emitterPoint);
            ImGui::DragFloat("SpawnTime", &time);
            ImGui::InputInt("Type", &enemyType);

            if (ImGui::Button("Add"))
            {
                EnemySource source;
                source.mEmitterNumber = emitterPoint;
                source.mSpawnTimer = time;
                source.mType = enemyType;
                mEditorSourceVec.emplace_back(source);
            }
        }

        if (ImGui::CollapsingHeader("List"))
        {
            for(const auto source:mEditorSourceVec)
            {
                ImGui::Text("Time");
                ImGui::SameLine();
                ImGui::Text(std::to_string(source.mSpawnTimer).c_str());

                ImGui::Text("EmitterNumber");
                ImGui::SameLine();
                ImGui::Text(std::to_string(source.mEmitterNumber).c_str());

                ImGui::Text("Type");
                ImGui::SameLine();
                ImGui::Text(std::to_string(source.mType).c_str());

            }
        }

        if(ImGui::Button("OpenParamEditor"))
        {
            mParamEditor = true;
        }

        if (ImGui::Button("Close"))
        {
            mIsOpenEditor = false;
        }
        ImGui::End();
    }
#endif

}

void EnemyEditor::fGui_ParamEditor()
{

#ifdef USE_IMGUI
    if (mParamEditor)
    {
        ImGui::Begin("ParamEditor");
        static char name[256]{};
        ImGui::InputText("Name", name,ARRAYSIZE(name));
        if (ImGui::Button("Add"))
        {
            EnemyData enemyData{};
            enemyData.mDivideClass = name;
            mKindsMap.insert(std::make_pair(name, enemyData));
        }

        if(ImGui::Button("CreateFile"))
        {
            EnemyFileSystem::fSaveToJson(mKindsMap, "EnemiesParam");
        }
        ImGui::End();
    }
#endif
}

std::function<EnemyData(std::string)> EnemyEditor::fGetFunction()const
{
    return mFunction;
}

void EnemyEditor::fControlParams()
{
    throw std::logic_error("Not implemented");
}
