#include "point_light.h"
#include "imgui_include.h"
#include "user.h"

PointLights::PointLights(GraphicsPipeline& graphics)
{
	//定数バッファ
	plig_constants = std::make_unique<Constants<PointLightConstants>>(graphics.get_device().Get());
	// モデルの生成
    for (int i = 0; i < POINT_LIGHT_COUNT; ++i)
    {
        light_sources[i] = std::make_unique<SkinnedMesh>(graphics.get_device().Get(), ".\\resources\\Models\\point_light\\point_light.fbx");
    }
	// パラメータ設定
	set_all_parameters(0, { 8,1,25,0 },  { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,(float)rand() / RAND_MAX,1 }, 5);
	set_all_parameters(1, { 6,1,25,0 },  { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,(float)rand() / RAND_MAX,1 }, 5);
	set_all_parameters(2, { 4,1,25,0 },  { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,(float)rand() / RAND_MAX,1 }, 5);
	set_all_parameters(3, { 2,1,25,0 },  { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,(float)rand() / RAND_MAX,1 }, 5);
	set_all_parameters(4, { 1,1,30,0 },  { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,(float)rand() / RAND_MAX,1 }, 5);
	set_all_parameters(5, { 3,1,30,0 },  { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,(float)rand() / RAND_MAX,1 }, 5);
	set_all_parameters(6, { 5,1,30,0 },  { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,(float)rand() / RAND_MAX,1 }, 5);
	set_all_parameters(7, { 7,1,30,0 },  { (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,(float)rand() / RAND_MAX,1 }, 5);
}

PointLights::~PointLights() {}

void PointLights::render(GraphicsPipeline& graphics, float elapsed_time)
{
	// モデルの描画
	static DirectX::XMFLOAT3  angle = { 0,0,0 };
	static DirectX::XMFLOAT3  scale = { 0.1f,0.1f,0.1f };
#ifdef USE_IMGUI
	ImGui::Begin("point light");
	if (ImGui::TreeNode("transform"))
	{
		ImGui::DragFloat3("angle", &angle.x, 0.1f);
		ImGui::DragFloat3("scale", &scale.x, 0.1f, -10, 10);
		ImGui::TreePop();
	}
	ImGui::End();
#endif
	for (int i = 0; i < POINT_LIGHT_COUNT; ++i)
	{
#ifdef USE_IMGUI
		ImGui::Begin("point light");
		std::string s_transform = "light" + std::to_string(i);
		if (ImGui::TreeNode(s_transform.c_str()))
		{
			std::string s = "pos" + std::to_string(i);
			ImGui::DragFloat3(s.c_str(), &plig_constants->data.point_lights[i].position.x, 0.1f);
			std::string s2 = "range" + std::to_string(i);
			ImGui::DragFloat(s2.c_str(), &plig_constants->data.point_lights[i].range, 0.1f);
			std::string s3 = "luminous_intensity" + std::to_string(i);
			ImGui::DragFloat(s3.c_str(), &plig_constants->data.point_lights[i].luminous_intensity, 0.01f);
			std::string s4 = "color" + std::to_string(i);
			ImGui::ColorEdit4(s4.c_str(), &plig_constants->data.point_lights[i].color.x);
			ImGui::TreePop();
		}
		ImGui::End();
#endif
		plig_constants->data.unique_id = i;
		// 定数バッファ
		plig_constants->bind(graphics.get_dc().Get(), 3);
		// 描画ステート設定
		graphics.set_pipeline_preset(RASTERIZER_STATE::SOLID_COUNTERCLOCKWISE, DEPTH_STENCIL::DEON_DWON, SHADER_TYPES::POINT_LIGHT);
		// スタティックメッシュ
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT3 position = { plig_constants->data.point_lights[i].position.x, plig_constants->data.point_lights[i].position.y, plig_constants->data.point_lights[i].position.z };
		light_sources[i]->render(graphics.get_dc().Get(), Math::calc_world_matrix(scale, angle, position), plig_constants->data.point_lights[i].color);
	}
}
