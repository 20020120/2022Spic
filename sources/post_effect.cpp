#include <iostream>

#include "post_effect.h"
#include "misc.h"
#include "shader.h"
#include "framework.h"

#include"user.h"

PostEffect::PostEffect(ID3D11Device* device)
{
	// デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };
	// ピクセルシェーダーの作成
	{
		create_ps_from_cso(device, "shaders/luminance_extraction_ps.cso", pixel_shaders[0].GetAddressOf());
		create_ps_from_cso(device, "shaders/blur_ps.cso", pixel_shaders[1].GetAddressOf());
		create_ps_from_cso(device, "shaders/RGB_shift_ps.cso", pixel_shaders[2].GetAddressOf());
		create_ps_from_cso(device, "shaders/white_noise_ps.cso", pixel_shaders[3].GetAddressOf());
		create_ps_from_cso(device, "shaders/low_resolution_ps.cso", pixel_shaders[4].GetAddressOf());
		create_ps_from_cso(device, "shaders/scan_line_ps.cso", pixel_shaders[5].GetAddressOf());
		create_ps_from_cso(device, "shaders/default_ps.cso", pixel_shaders[6].GetAddressOf());
		create_ps_from_cso(device, "shaders/game_boy_palette_ps.cso", pixel_shaders[7].GetAddressOf());
		create_ps_from_cso(device, "shaders/barrel_shaped_bend_ps.cso", pixel_shaders[8].GetAddressOf());
		create_ps_from_cso(device, "shaders/glitch_ps.cso", pixel_shaders[9].GetAddressOf());
		create_ps_from_cso(device, "shaders/vignetting_ps.cso", pixel_shaders[10].GetAddressOf());
		create_ps_from_cso(device, "shaders/dash_blur_ps.cso", pixel_shaders[11].GetAddressOf());
	}
	// 定数バッファ
	effect_constants = std::make_unique<Constants<PostEffectConstants>>(device);
	// FrameBufferの生成
	for (int i = 0; i < FRAMEBUFFERS_COUNT; ++i)
	{
		framebuffers[i] = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, i == 0 ? FB_FLAG::COLOR_DEPTH : FB_FLAG::COLOR);
	}
	// fullscreen_quadオブジェクト生成
	bit_block_transfer = std::make_unique<FullScreenQuad>(device);
}

PostEffect::~PostEffect()
{
}

void PostEffect::begin(ID3D11DeviceContext* dc)
{
	// オフスクリーンバッファ（framebuffer）の内容を画面に描画する
    // ここからdeactivate()で囲まれている部分がオフスクリーンレンダリングの対象になる
	framebuffers[static_cast<int>(FRAME_BUFER_SLOT::OFF_SCREEN)]->clear(dc);
	framebuffers[static_cast<int>(FRAME_BUFER_SLOT::OFF_SCREEN)]->activate(dc);
}

void PostEffect::end(ID3D11DeviceContext* dc)
{
	framebuffers[static_cast<int>(FRAME_BUFER_SLOT::OFF_SCREEN)]->deactivate(dc);
}

void PostEffect::apply_an_effect(ID3D11DeviceContext* dc, float elapsed_time)
{
	{
		const char* effects[] = { "NONE", "BLUR", "RGB_SHIFT", "WHITE_NOISE", "LOW_RESOLUTION", "SCAN_LINE", "GAME_BOY",
			"BARREL_SHAPED", "GLITCH", "VIGNETTING", "DASH_BLUR"};
		static int effect_type[FRAMEBUFFERS_COUNT - 2] = { static_cast<int>(POST_EFFECT_TYPE::NONE) };
		static int post_effect_count = 1;
		static bool display_effect_imgui = false;
#ifdef USE_IMGUI
		imgui_menu_bar("contents", "post effect", display_effect_imgui);
		if (display_effect_imgui)
		{
			ImGui::Begin("PostEffect");
			ImGui::InputInt("post_efc_count", &post_effect_count);
			post_effect_count = Math::clamp(post_effect_count, 1, FRAMEBUFFERS_COUNT - 2);
			for (int i = 0; i < post_effect_count; ++i)
			{
				std::string ss = "post efc " + std::to_string(i + 1);
				ImGui::Combo(ss.c_str(), &effect_type[i], effects, IM_ARRAYSIZE(effects));
			}
			ImGui::End();
		}
#endif // USE_IMGUI
		auto r_set_framebuffer_pstefc = [&](int i, int last_minute_framebuffer_slot, int ps_slot)
		{
			framebuffers[i + 2]->clear(dc, FB_FLAG::COLOR);
			framebuffers[i + 2]->activate(dc, FB_FLAG::COLOR);
			bit_block_transfer->blit(dc,
				framebuffers[last_minute_framebuffer_slot]->get_color_map().GetAddressOf(), 0, 1, pixel_shaders[ps_slot].Get());
			framebuffers[i + 2]->deactivate(dc);
		};
		for (int i = 0; i < post_effect_count; ++i)
		{
			int last_minute_framebuffer_slot = i == 0 ? 0 : i + 1;

			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::NONE))
			{
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 6);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::BLUR))
			{
				// framebuffers[0]から高輝度成分を抽出しframebuffers[1]に転送する
				{
#ifdef USE_IMGUI
					std::string ss = "luminance_extraction " + std::to_string(i + 1);
					if (display_effect_imgui)
					{
						ImGui::Begin("pst efc para");
						if (ImGui::TreeNode(ss.c_str()))
						{

							ImGui::DragFloat("min", &effect_constants->data.luminance_min_max.x, 0.01f);
							ImGui::DragFloat("max", &effect_constants->data.luminance_min_max.y, 0.01f);
							ImGui::DragFloat3("dot_rgb", &effect_constants->data.luminance_dot_rgb.x, 0.01f);
							ImGui::TreePop();
						}
						ImGui::End();
					}
#endif
					effect_constants->bind(dc, 5);
					framebuffers[static_cast<int>(FRAME_BUFER_SLOT::LUMINANCE)]->clear(dc, FB_FLAG::COLOR);
					framebuffers[static_cast<int>(FRAME_BUFER_SLOT::LUMINANCE)]->activate(dc, FB_FLAG::COLOR);
					bit_block_transfer->blit(dc,
						framebuffers[static_cast<int>(FRAME_BUFER_SLOT::OFF_SCREEN)]->get_color_map().GetAddressOf(), 0, 1, pixel_shaders[0].Get());
					framebuffers[static_cast<int>(FRAME_BUFER_SLOT::LUMINANCE)]->deactivate(dc);
				}
				// シーン画像とブラーをかけた高輝度成分画像を合成し、framebuffers[2]に転送する
				{
#ifdef USE_IMGUI
					std::string ss = "blur_effect " + std::to_string(i + 1);
					if (display_effect_imgui)
					{
						ImGui::Begin("pst efc para");
						if (ImGui::TreeNode(ss.c_str()))
						{
							ImGui::DragFloat("gaussian_sigma", &effect_constants->data.blur_gaussian_sigma, 0.01f);
							ImGui::DragFloat("bloom_intensity", &effect_constants->data.blur_bloom_intensity, 0.01f);
							ImGui::TreePop();
						}
						ImGui::End();
					}
#endif
					effect_constants->bind(dc, 5);
					framebuffers[i + 2]->clear(dc, FB_FLAG::COLOR);
					framebuffers[i + 2]->activate(dc, FB_FLAG::COLOR);
					ID3D11ShaderResourceView* shader_resource_views[]
					{ framebuffers[last_minute_framebuffer_slot]->get_color_map().Get(),
					  framebuffers[static_cast<int>(FRAME_BUFER_SLOT::LUMINANCE)]->get_color_map().Get() };
					bit_block_transfer->blit(dc, shader_resource_views, 0, _countof(shader_resource_views), pixel_shaders[1].Get());
					framebuffers[i + 2]->deactivate(dc);
				}
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::RGB_SHIFT))
			{
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 2);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::WHITE_NOISE))
			{
				effect_constants->data.white_noise_time >= 20.0f
					? effect_constants->data.white_noise_time = 10.0f : effect_constants->data.white_noise_time += elapsed_time;
#ifdef USE_IMGUI
				std::string ss = "white noise " + std::to_string(i + 1);
				if (display_effect_imgui)
				{
					ImGui::Begin("pst efc para");
					if (ImGui::TreeNode(ss.c_str()))
					{
						ImGui::DragFloat("time", &effect_constants->data.white_noise_time, 0.1f);
						ImGui::TreePop();
					}
					ImGui::End();
				}
#endif
				effect_constants->bind(dc, 5);
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 3);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::LOW_RESOLUTION))
			{
#ifdef USE_IMGUI
				std::string ss = "low resolution " + std::to_string(i + 1);
				if (display_effect_imgui)
				{
					ImGui::Begin("pst efc para");
					if (ImGui::TreeNode(ss.c_str()))
					{
						ImGui::DragFloat("Resolution", &effect_constants->data.low_resolution, 0.1f);
						ImGui::DragFloat("number_of_divisions", &effect_constants->data.low_resolution_number_of_divisions, 1.0f);
						ImGui::TreePop();
					}
					ImGui::End();
				}
#endif
				effect_constants->bind(dc, 5);
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 4);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::SCAN_LINE))
			{
				effect_constants->data.scan_line_time <= -20.0f ?
					effect_constants->data.scan_line_time = 10.0f : effect_constants->data.scan_line_time -= elapsed_time * 5;
#ifdef USE_IMGUI
				std::string ss = "scan line " + std::to_string(i + 1);
				if (display_effect_imgui)
				{
					ImGui::Begin("pst efc para");
					if (ImGui::TreeNode(ss.c_str()))
					{
						ImGui::DragFloat("time", &effect_constants->data.scan_line_time, 0.1f);
						ImGui::TreePop();
					}
					ImGui::End();
				}
#endif
				effect_constants->bind(dc, 5);
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 5);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::GAME_BOY))
			{
#ifdef USE_IMGUI
				std::string ss = "game boy " + std::to_string(i + 1);
				if (display_effect_imgui)
				{
					ImGui::Begin("pst efc para");
					if (ImGui::TreeNode(ss.c_str()))
					{
						ImGui::DragFloat3("border_color1", &effect_constants->data.border_color1.x, 0.001f);
						ImGui::DragFloat3("border_color2", &effect_constants->data.border_color2.x, 0.001f);
						ImGui::DragFloat3("border_color3", &effect_constants->data.border_color3.x, 0.001f);
						ImGui::DragFloat3("border_color4", &effect_constants->data.border_color4.x, 0.001f);
						ImGui::TreePop();
					}
					ImGui::End();
				}
#endif
				effect_constants->bind(dc, 5);
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 7);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::BARREL_SHAPED))
			{
#ifdef USE_IMGUI
				std::string ss = "barrel shaped " + std::to_string(i + 1);
				if (display_effect_imgui)
				{
					ImGui::Begin("pst efc para");
					if (ImGui::TreeNode(ss.c_str()))
					{
						ImGui::DragFloat("distortion", &effect_constants->data.barrel_shaped_distortion, 0.001f);
						ImGui::TreePop();
					}
					ImGui::End();
				}
#endif
				effect_constants->bind(dc, 5);
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 8);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::GLITCH))
			{
				effect_constants->data.glitch_time >= 20.0f
					? effect_constants->data.glitch_time = 10.0f : effect_constants->data.glitch_time += 0.5f * elapsed_time;
#ifdef USE_IMGUI
				std::string ss = "glitch " + std::to_string(i + 1);
				if (display_effect_imgui)
				{
					ImGui::Begin("pst efc para");
					if (ImGui::TreeNode(ss.c_str()))
					{
						ImGui::DragFloat2("glitch_pase", &effect_constants->data.glitch_pase.x, 0.001f);
						ImGui::DragFloat("glitch_step_value", &effect_constants->data.glitch_step_value, 0.001f);
						ImGui::DragFloat("glitch_time", &effect_constants->data.glitch_time);
						ImGui::TreePop();
					}
					ImGui::End();
				}
#endif
				effect_constants->bind(dc, 5);
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 9);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::VIGNETTING))
			{
#ifdef USE_IMGUI
				std::string ss = "vignetting " + std::to_string(i + 1);
				if (display_effect_imgui)
				{
					ImGui::Begin("pst efc para");
					if (ImGui::TreeNode(ss.c_str()))
					{
						ImGui::DragFloat2("vignetting_pos", &effect_constants->data.vignetting_pos.x, 0.001f);
						ImGui::DragFloat("vignetting_scope", &effect_constants->data.vignetting_scope, 0.001f);
						ImGui::TreePop();
					}
					ImGui::End();
				}
#endif
				effect_constants->bind(dc, 5);
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 10);
			}
			if (effect_type[i] == static_cast<int>(POST_EFFECT_TYPE::DASH_BLUR))
			{
#ifdef USE_IMGUI
				std::string ss = "dash_blur " + std::to_string(i + 1);
				if (display_effect_imgui)
				{
					ImGui::Begin("pst efc para");
					if (ImGui::TreeNode(ss.c_str()))
					{
						ImGui::DragFloat2("reference_pos", &effect_constants->data.reference_position.x, 0.01f);
						ImGui::DragFloat("zoom_power", &effect_constants->data.zoom_power, 0.01f);
						ImGui::DragInt("focus_detail", &effect_constants->data.focus_detail);
						ImGui::TreePop();
					}
					ImGui::End();
				}
#endif
				effect_constants->bind(dc, 5);
				r_set_framebuffer_pstefc(i, last_minute_framebuffer_slot, 11);
			}
		}
		//----セットしたポストエフェクトを描画する---//
		{
			// 一番後ろのframebufferを描画する
			last_pst_efc_index = post_effect_count - 1;
			bit_block_transfer->blit(dc,
				framebuffers[last_pst_efc_index + 2]->get_color_map().GetAddressOf(),
				0, 1, pixel_shaders[6].Get());
		}
	}
}

void PostEffect::scene_preview()
{
	float ratio_percentage = 0.2f;
	static bool display_preview_imgui = false;
#ifdef USE_IMGUI
	imgui_menu_bar("contents", "scene preview", display_preview_imgui);
	if (display_preview_imgui)
	{
		ImGui::Begin("scene_preview");
		// 画像の描画
		ImGui::Image((void*)framebuffers[static_cast<int>(FRAME_BUFER_SLOT::OFF_SCREEN)]->get_color_map().Get(),
			ImVec2(1280.0f * ratio_percentage, 720.0f * ratio_percentage), ImVec2(0, 0), ImVec2(1, 1), { 1,1,1,1 });
		ImGui::Image((void*)framebuffers[static_cast<int>(FRAME_BUFER_SLOT::OFF_SCREEN)]->get_depth_map().Get(),
			ImVec2(1280.0f * ratio_percentage, 720.0f * ratio_percentage), ImVec2(0, 0), ImVec2(1, 1), { 1,1,1,1 });
		ImGui::End();
	}
#endif // USE_IMGUI
}