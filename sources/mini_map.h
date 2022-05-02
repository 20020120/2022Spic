#pragma once
#include "EnemyManager.h"
#include "sprite_batch.h"
class MiniMap 
{
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
	public:
	MiniMap(GraphicsPipeline& graphics);
	~MiniMap(){}

	void render(GraphicsPipeline& graphics,const DirectX::XMFLOAT2& player_pos,const DirectX::XMFLOAT2& player_forward, std::vector<BaseEnemy*> enemy_list);
	std::unique_ptr<SpriteBatch> icon;
	Element player_icon;
	Element enemy_icon;
	Element radar_icon;
};
