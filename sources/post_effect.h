#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include "framebuffer.h"
#include "fullscreen_quad.h"
#include "constants.h"

class PostEffect
{
public:
    //--------<constructor/destructor>--------//
    PostEffect(ID3D11Device* device);
    ~PostEffect();
    //--------< �֐� >--------//
    void begin(ID3D11DeviceContext* dc);
    void end(ID3D11DeviceContext* dc);
    void apply_an_effect(ID3D11DeviceContext* dc, float elapsed_time);
	void scene_preview();
	//--------<getter/setter>--------//
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& get_color_map() { return framebuffers[static_cast<int>(FRAME_BUFER_SLOT::OFF_SCREEN)]->get_color_map(); }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& get_depth_map() { return framebuffers[static_cast<int>(FRAME_BUFER_SLOT::OFF_SCREEN)]->get_depth_map(); }

private:
    //--------< �萔 >--------//
    enum class POST_EFFECT_TYPE {
		NONE,                       // �Ȃ�
		BLUR,						// �u���[
		RGB_SHIFT,					// RGB�V�t�g
		WHITE_NOISE,				// �z���C�g�m�C�Y
		LOW_RESOLUTION,				// ���U�C�N(��𑜓x)
		SCAN_LINE,					// ������
		GAME_BOY,					// �Q�[���{�[�C��
		BARREL_SHAPED,				// �M��p��
		GLITCH,                     // �O���b�`
		VIGNETTING,                 // �r�l�b�g

		PST_EFC_COUNT               //���̒萔���Ō��
	};
    enum class FRAME_BUFER_SLOT { OFF_SCREEN, LUMINANCE };
    //--------< �\���� >--------//
	struct PostEffectConstants
	{
		// luminance
		DirectX::XMFLOAT4 luminance_min_max{ 0.6f, 0.8f, 0, 0 };
		DirectX::XMFLOAT4 luminance_dot_rgb{ 0.299f, 0.587f, 0.114f, 0 };
		// blur
		float blur_gaussian_sigma{ 1.0f };
		DirectX::XMFLOAT3 pad1;
		float blur_bloom_intensity{ 1.0f };
		DirectX::XMFLOAT3 pad2;
		// white_noise
		float white_noise_time{ 10.0f };
		DirectX::XMFLOAT3 pad3;
		// low_resolution
		float low_resolution{ 1.2f };
		float low_resolution_number_of_divisions{ 55.0f };
		DirectX::XMFLOAT2 pad4;
		// scan_line
		float scan_line_time{};
		DirectX::XMFLOAT3 pad5;
		// game_boy
		DirectX::XMFLOAT4 border_color1{ 0.612f, 0.725f, 0.086f, 0 };
		DirectX::XMFLOAT4 border_color2{ 0.549f, 0.667f, 0.078f, 0 };
		DirectX::XMFLOAT4 border_color3{ 0.188f, 0.392f, 0.188f, 0 };
		DirectX::XMFLOAT4 border_color4{ 0.063f, 0.247f, 0.063f, 0 };
		// barrel_shaped
		float barrel_shaped_distortion{ 0.2f };
		DirectX::XMFLOAT3 pad6;
		// glitch
		DirectX::XMFLOAT2 glitch_pase{ 0.5f, -0.5f };
		float glitch_step_value{ 400.0f };
		float glitch_time{};
		// vignette
		DirectX::XMFLOAT2 vignetting_pos{ 0.5f, 0.5f };
		float vignetting_scope{ 0.2f };
		float pad7;
	};
	std::unique_ptr<Constants<PostEffectConstants>> effect_constants;
    //--------< �֐� >--------//


    //--------< �ϐ� >--------//
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[16];
	// framebuffer�̎���
	static const int FRAMEBUFFERS_COUNT = 16;
	std::unique_ptr<FrameBuffer> framebuffers[FRAMEBUFFERS_COUNT];
	// FullScreenQuad�̎���
	std::unique_ptr<FullScreenQuad> bit_block_transfer;

	int last_pst_efc_index;
};