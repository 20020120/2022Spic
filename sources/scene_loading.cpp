#include <thread>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#include "scene_loading.h"
#include "scene_manager.h"
#include "scene_game.h"

void SceneLoading::initialize(GraphicsPipeline& graphics)
{
    // sprite_batch
    sprite_batch = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\load_back.png", 1);
    //�X���b�h�J�n
    std::thread thread(loading_thread, &graphics, this);
    //�X���b�h�̊Ǘ������
    thread.detach();
}

void SceneLoading::uninitialize()
{
}

void SceneLoading::update(GraphicsPipeline& graphics, float elapsed_time)
{
    // �V�[����؂�ւ���
    if (next_scene->is_ready())
    {
        SceneManager::scene_switching(next_scene);
        //next_scene->set_ready(false);
    }
}

void SceneLoading::render(GraphicsPipeline& graphics, float elapsed_time)
{
    graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
    sprite_batch->begin(graphics.get_dc().Get());
    sprite_batch->render(graphics.get_dc().Get(), { 0,0 }, { 1, 1 });
    sprite_batch->end(graphics.get_dc().Get());
}

void SceneLoading::loading_thread(GraphicsPipeline* graphics, SceneLoading* scene)
{
    // ���̃V�[���̏��������s��
    scene->next_scene->initialize(*graphics);
    // ���̃V�[���̏��������ݒ�
    scene->next_scene->set_ready(true);
}