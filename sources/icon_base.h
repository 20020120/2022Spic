#pragma once

#include <memory>
#include <string>
#include "sprite_batch.h"

class IconBase
{
public:
    //--------<constructor/destructor>--------//
    IconBase(ID3D11Device* device, const wchar_t* filename, size_t max_sprites);
    virtual ~IconBase() {}
    //--------< ŠÖ” >--------//
    virtual void update(float elapsed_time) = 0;
    virtual void render(std::string gui, ID3D11DeviceContext* dc);
    //--------<getter/setter>--------//
    void reset_state() { state = 0; }
    const DirectX::XMFLOAT2& get_icon_position() const { return icon.position; }
protected:
    //--------< \‘¢‘Ì >--------//
    struct Element
    {
        DirectX::XMFLOAT2 position{};
        DirectX::XMFLOAT2 scale{ 1, 1 };
        DirectX::XMFLOAT2 pivot{};
        DirectX::XMFLOAT4 color{ 1,1,1,1 };
        float angle{};
        DirectX::XMFLOAT2 texpos{};
        DirectX::XMFLOAT2 texsize{};
    };
    struct FontElement
    {
        std::wstring s = L"";
        DirectX::XMFLOAT2 position{};
        DirectX::XMFLOAT2 scale{ 1, 1 };
        DirectX::XMFLOAT4 color{ 1,1,1,1 };
        float angle{};
        DirectX::XMFLOAT2 length{};
    };

    //--------< •Ï” >--------//
    Element icon;
    std::unique_ptr<SpriteBatch> sprite_icon = nullptr;
    Element selecterL;
    Element selecterR;
    DirectX::XMFLOAT2 selecterL_arrival_pos{};
    DirectX::XMFLOAT2 selecterR_arrival_pos{};
    std::unique_ptr<SpriteBatch> sprite_selecter{ nullptr };

    int state = 0;
};