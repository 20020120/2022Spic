#include "SwordTrail.h"
#include "Player.h"


bool transit(float elapsed_time, int& index, DirectX::XMFLOAT3& position, float speed, const std::vector<DirectX::XMFLOAT3>& points, float play = 0.01f)
{
	assert(points.empty());
	if (index >= points.size() - 1) return true;

	using namespace DirectX;
	XMFLOAT3 velocity{};

	XMVECTOR vec = XMLoadFloat3(&points.at(index + 1)) - XMLoadFloat3(&position);
	XMVECTOR norm_vec = XMVector3Normalize(vec);
	XMStoreFloat3(&velocity, norm_vec);

	XMVECTOR length_vec = DirectX::XMVector3Length(vec);
	float length = DirectX::XMVectorGetX(length_vec);
	if (length <= play)
	{
		++index;
		position = points.at(index);
	}

	position.x += velocity.x * speed * elapsed_time;
	position.y += velocity.y * speed * elapsed_time;
	position.z += velocity.z * speed * elapsed_time;

	return false;
}

DirectX::XMFLOAT3 stun_enemy_position{};
int transit_index = 0;
std::vector<DirectX::XMFLOAT3> way_points;
std::vector<DirectX::XMFLOAT3> interpolated_way_points;

void Player::ChainLockOn()
{


}

void Player::chain_search_update(float elapsed_time)
{
	//if (/*スタンされた敵がいなければ通常行動に戻る*/) {}
	//else if (/*スタンされた敵がいれば*/) { transition_chain_lockon(); }
}

void Player::transition_chain_search()
{
	player_chain_activity = &Player::chain_search_update;
}

void Player::chain_lockon_update(float elapsed_time)
{
	static float speed = 5.0f;
	static float play  = 0.1f;
#ifdef USE_IMGUI
	ImGui::Begin("transit");
	ImGui::DragFloat3("position", &position.x, 0.1f);
	ImGui::DragFloat("speed", &speed, 0.1f);
	ImGui::DragFloat("play", &play, 0.01f);
	if (ImGui::Button("restart")) { transit_index = 0; }
	ImGui::End();
#endif // USE_IMGUI

	// 最後の点(敵の位置)についたら攻撃ステートへ
	if (transit(elapsed_time, transit_index, position, speed, interpolated_way_points, play))
	{
		transition_chain_attack();
	}
	//else if (/*キャンセルがあれがここへ*/)
	//{

	//}
}

void Player::transition_chain_lockon()
{
	// 右方向ベクトルとターゲットへのベクトルの角度を算出
	DirectX::XMVECTOR norm_vec = Math::calc_vector_AtoB_normalize(position, stun_enemy_position);
	DirectX::XMFLOAT3 right = { 1,0,0 };
	DirectX::XMVECTOR right_vec = DirectX::XMLoadFloat3(&right);
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(norm_vec, right_vec);
	float cos_theta = DirectX::XMVectorGetX(dot);
	// 右方向ベクトルとターゲットへのベクトルの角度と任意の角度を足したものが中間地点のθ
	const float theta = DirectX::XMConvertToRadians(acosf(cos_theta + FLT_EPSILON) + 30.0f);
	// ターゲットへのベクトルの長さを算出
	float length_vec = Math::calc_vector_AtoB_length(position, stun_enemy_position);
	// 中間地点の算出
	DirectX::XMFLOAT3 midpoint = { position.x + cosf(theta) * length_vec * 0.5f, position.y, position.z + sinf(theta) * length_vec * 0.5f };
	// way_pointsの生成
	way_points.emplace_back(position); // この時点でのプレイヤーの位置
	way_points.emplace_back(midpoint); // プレイヤーの位置と敵の位置の中間地点
	way_points.emplace_back(stun_enemy_position); // 敵の位置
	// way_pointsを通るカーブを作成
	CatmullRomSpline curve(way_points);
	curve.interpolate(interpolated_way_points, 5);

	transit_index = 0;

	player_chain_activity = &Player::chain_lockon_update;
}

void Player::chain_move_update(float elapsed_time)
{

}

void Player::transition_chain_move()
{

	player_chain_activity = &Player::chain_move_update;
}

void Player::chain_attack_update(float elapsed_time)
{



	// 攻撃が終われば索敵ステートへ
	//if (攻撃が終われば)
	//{
	//	transition_chain_search();
	//}
}

void Player::transition_chain_attack()
{
	player_chain_activity = &Player::chain_attack_update;
}