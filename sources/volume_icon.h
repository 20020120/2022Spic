#pragma once

#include "icon_base.h"
#include "practical_entities.h"
#include "number.h"

class VolumeIcon : public IconBase, PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    VolumeIcon(ID3D11Device* device);
    ~VolumeIcon() override;
    //--------< ŠÖ” >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void render(std::string gui, ID3D11DeviceContext* dc, const DirectX::XMFLOAT2& add_pos) override;
private:
    //--------< ’è” >--------//
    enum BarType
    {
        MASTER,
        BGM,
        SE,
    };
    static const int MAX_SCALE_COUNT = 20;
    static const int BAR_COUNT = 3;
    static constexpr float INTERVAL = 0.1f;
    //--------< •Ï” >--------//
    FontElement master;
    FontElement bgm;
    FontElement se;

    std::map<BarType, std::unique_ptr<Number>> volume_numbers;

    float interval_LX = 0;

    std::vector<Element> scales[BAR_COUNT];
    std::vector<Element> shell_scales[BAR_COUNT];
    std::unique_ptr<SpriteBatch> sprite_scale = nullptr;
};