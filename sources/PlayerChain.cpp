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
	//if (/*�X�^�����ꂽ�G�����Ȃ���Βʏ�s���ɖ߂�*/) {}
	//else if (/*�X�^�����ꂽ�G�������*/) { transition_chain_lockon(); }
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

	// �Ō�̓_(�G�̈ʒu)�ɂ�����U���X�e�[�g��
	if (transit(elapsed_time, transit_index, position, speed, interpolated_way_points, play))
	{
		transition_chain_attack();
	}
	//else if (/*�L�����Z�������ꂪ������*/)
	//{

	//}
}

void Player::transition_chain_lockon()
{
	// �E�����x�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g���̊p�x���Z�o
	DirectX::XMVECTOR norm_vec = Math::calc_vector_AtoB_normalize(position, stun_enemy_position);
	DirectX::XMFLOAT3 right = { 1,0,0 };
	DirectX::XMVECTOR right_vec = DirectX::XMLoadFloat3(&right);
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(norm_vec, right_vec);
	float cos_theta = DirectX::XMVectorGetX(dot);
	// �E�����x�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g���̊p�x�ƔC�ӂ̊p�x�𑫂������̂����Ԓn�_�̃�
	const float theta = DirectX::XMConvertToRadians(acosf(cos_theta + FLT_EPSILON) + 30.0f);
	// �^�[�Q�b�g�ւ̃x�N�g���̒������Z�o
	float length_vec = Math::calc_vector_AtoB_length(position, stun_enemy_position);
	// ���Ԓn�_�̎Z�o
	DirectX::XMFLOAT3 midpoint = { position.x + cosf(theta) * length_vec * 0.5f, position.y, position.z + sinf(theta) * length_vec * 0.5f };
	// way_points�̐���
	way_points.emplace_back(position); // ���̎��_�ł̃v���C���[�̈ʒu
	way_points.emplace_back(midpoint); // �v���C���[�̈ʒu�ƓG�̈ʒu�̒��Ԓn�_
	way_points.emplace_back(stun_enemy_position); // �G�̈ʒu
	// way_points��ʂ�J�[�u���쐬
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



	// �U�����I���΍��G�X�e�[�g��
	//if (�U�����I����)
	//{
	//	transition_chain_search();
	//}
}

void Player::transition_chain_attack()
{
	player_chain_activity = &Player::chain_attack_update;
}