#include "counter.h"
#include "Operators.h"

Counter::Counter(GraphicsPipeline& graphics, const wchar_t* filename, size_t max_sprites)
{
    number = std::make_unique<Number>(graphics);
    sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), filename, max_sprites);
    //--element--//
    element.texsize = { static_cast<float>(sprite->get_texture2d_desc().Width), static_cast<float>(sprite->get_texture2d_desc().Height) };
    element.pivot   = element.texsize * DirectX::XMFLOAT2(0.0, 0.5f);
}

void Counter::update(GraphicsPipeline& graphics, float elapsed_time)
{
    number->set_offset_scale(element.scale);
    number->set_offset_pos(element.position + DirectX::XMFLOAT2(element.texsize.x * element.scale.x, 0) + offset);
	number->update(graphics, elapsed_time);
}

void Counter::render(GraphicsPipeline& graphics, float elapsed_time)
{
    //--number--//
    sprite->begin(graphics.get_dc().Get());
    sprite->render(graphics.get_dc().Get(), element.position, element.scale, element.pivot, element.color, element.angle, element.texpos, element.texsize);
    sprite->end(graphics.get_dc().Get());

    number->render(graphics, elapsed_time);
}