#include "mini_map.h"
#include "Operators.h"
#include "user.h"

MiniMap::MiniMap(GraphicsPipeline& graphics)
{
	icon = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprites\\ui\\icon.png", 256);
	 player_icon = 
		 {
		{128,128},{0.5f,0.5f},{0,0},
		{1,1,1,1},{0},{128,128},
	 	{128,128}
		 };
	 enemy_icon =
	 {
	{128,128},{0.3f,0.3f},{0,0},
	{1,1,1,1},{0},{384,128},
	{128,128}
	 };;
	 radar_icon =
	 {
	{128,128},{1,1},{0,0},
	{1,1,1,1},{0},{0,128},
	{128,128}
	 };;
}
void MiniMap::render(GraphicsPipeline& graphics,const DirectX::XMFLOAT2& player_pos,const DirectX::XMFLOAT2& player_forward, std::vector<BaseEnemy*> enemy_list)
{
	//レーダーあいこん　
	DirectX::XMFLOAT2 center_pos = { 1080.0f,128.0f };//アイコンの基準位置
	icon->begin(graphics.get_dc().Get());
	icon->render(graphics.get_dc().Get(), { center_pos.x - 36.0f,center_pos.y -36.0f }, { radar_icon.scale },
		{ radar_icon.pivot }, { radar_icon.color },
		{ radar_icon.angle }, { radar_icon.texpos }, { radar_icon.texsize });
	icon->end(graphics.get_dc().Get());

	player_icon.position = center_pos;
	//プレイヤーアイコン
	icon->begin(graphics.get_dc().Get());
	icon->render(graphics.get_dc().Get(), { player_icon.position },{player_icon.scale},
		{player_icon.pivot},{player_icon.color},
		{player_icon.angle},{player_icon.texpos},{player_icon.texsize});
	icon->end(graphics.get_dc().Get());

	DirectX::XMVECTOR P_Normal = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&player_forward));
	//エネミーアイコン
	for(auto& enemy : enemy_list)
	{
		const DirectX::XMFLOAT2 p_pos = player_pos;
		const DirectX::XMFLOAT2 e_pos = { enemy->fGetPosition().x,enemy->fGetPosition().z };
		//プレイヤーから敵へのベクトル
		const DirectX::XMVECTOR Normal_P_To_E_Vec = Math::calc_vector_AtoB_normalize(p_pos, e_pos);
		//プレイヤーの正面ベクトルと敵までのベクトルとの内積で間の角を出す
		const DirectX::XMVECTOR Dot = DirectX::XMVector2Dot(P_Normal, Normal_P_To_E_Vec);
		float dot = DirectX::XMVectorGetX(Dot);
		dot = acosf(dot);
		DirectX::XMFLOAT2 normal_p_to_e_vec{};
		DirectX::XMStoreFloat2(&normal_p_to_e_vec, Normal_P_To_E_Vec);
		const float length_p_to_e_vec = Math::calc_vector_AtoB_length(p_pos, e_pos);
		//DirectX::XMFLOAT2 enemy_icon_pos = center_pos + (normal_p_to_e_vec * (length_p_to_e_vec * 0.7f));
		DirectX::XMFLOAT2 enemy_icon_pos;
		float cross{ (normal_p_to_e_vec.x * player_forward.y) - (normal_p_to_e_vec.y * player_forward.x) };

		dot = cross < 0 ? -dot : dot;
		enemy_icon_pos.x =  center_pos.x + length_p_to_e_vec * 0.7f * sinf(dot);
		enemy_icon_pos.y =  center_pos.y - length_p_to_e_vec * 0.7f * cosf(dot);
		enemy_icon.position = enemy_icon_pos;
		if(length_p_to_e_vec < 50.0f)
		{
			icon->begin(graphics.get_dc().Get());
			icon->render(graphics.get_dc().Get(), { enemy_icon.position }, { enemy_icon.scale },
				{ enemy_icon.pivot }, { enemy_icon.color },
				{ enemy_icon.angle }, { enemy_icon.texpos }, { enemy_icon.texsize });
			icon->end(graphics.get_dc().Get());
		}
	}
}
