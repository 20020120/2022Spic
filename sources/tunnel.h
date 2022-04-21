#pragma once
#include <d3d11.h>
#include <wrl.h>

class Tunnel
{
public:
    //--------<constructor/destructor>--------//
    Tunnel(ID3D11Device* device);
    ~Tunnel() = default;
    //--------< ŠÖ” >--------//


private:
    //--------< •Ï” >--------//
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_map;
};