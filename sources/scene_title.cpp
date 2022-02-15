#include "scene_title.h"
#include "scene_game.h"
#include "scene_loading.h"
#include "scene_manager.h"

void SceneTitle::initialize(GraphicsPipeline& graphics)
{
	// sprite_batch
	sprite_batch = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\Title.png", 1);
}

void SceneTitle::uninitialize() {}

void SceneTitle::update(GraphicsPipeline& graphics, float elapsed_time)
{
	// gamepad
	if (game_pad->get_button_down() & GamePad::BTN_A)
	{
		// ‰Ÿ‚µ‚½uŠÔ
		SceneManager::scene_switching(new SceneLoading(new SceneGame()));
		return;
	}
}

void SceneTitle::render(GraphicsPipeline& graphics, float elapsed_time)
{
	graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID, DEPTH_STENCIL::DEOFF_DWOFF);
	sprite_batch->begin(graphics.get_dc().Get());
	sprite_batch->render(graphics.get_dc().Get(), { 0,0 }, { 1, 1 });
	sprite_batch->end(graphics.get_dc().Get());
}
