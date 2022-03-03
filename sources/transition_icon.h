#pragma once

#include "icon_base.h"
#include "practical_entities.h"

class TransitionIcon : public IconBase, PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    TransitionIcon(ID3D11Device* device, const wchar_t* filename, size_t max_sprites);
    ~TransitionIcon() override {}
    //--------< ŠÖ” >--------//
    void update(float elapsed_time) override;
    void render(std::string gui, ID3D11DeviceContext* dc) override;
private:
    //--------< •Ï” >--------//
    FontElement game;
    FontElement title;
};