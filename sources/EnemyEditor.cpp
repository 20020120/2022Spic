#include"EnemiesEditor.h"

#include"imgui_include.h"
#include "user.h"


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

        if (ImGui::Button("Close"))
        {
            mIsOpenEditor = false;
        }
        ImGui::End();
    }
#endif

}
