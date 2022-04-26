#include <assert.h>
#include "SwordTrail.h"
#include "Player.h"



bool Player::transit(float elapsed_time, int& index, DirectX::XMFLOAT3& position, float speed,
	const std::vector<DirectX::XMFLOAT3>& points, float play)
{
	assert(!points.empty() && "�|�C���g�̃T�C�Y��0�ł�");
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

	rotate(elapsed_time, velocity);

	return false;
}

void Player::rotate(float elapsed_time, const DirectX::XMFLOAT3& velocity)
{
	using namespace DirectX;
	//�^�[�Q�b�g�Ɍ������ĉ�]
	XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&orientation);
	DirectX::XMVECTOR forward, up;
	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
	DirectX::XMFLOAT4X4 m4x4 = {};
	DirectX::XMStoreFloat4x4(&m4x4, m);
	up = { 0, 1, 0 };
	forward = { m4x4._31, m4x4._32, m4x4._33 };
	DirectX::XMFLOAT3 front{};
	DirectX::XMStoreFloat3(&front, forward);

	{
		XMVECTOR velocity_vec = XMVector3Normalize(DirectX::XMLoadFloat3(&velocity));
		XMVECTOR dot = XMVector3Dot(forward, velocity_vec);
		float angle = acosf(DirectX::XMVectorGetX(dot));
		if (fabs(angle + FLT_EPSILON) > DirectX::XMConvertToRadians(0.1f))
		{
			XMVECTOR q;
			float cross{ (velocity.x * front.z) - (velocity.z * front.x) };
			if (cross > 0)
			{
				q = XMQuaternionRotationAxis(up, angle);//���̕����ɓ����N�I�[�^�j�I��
			}
			else
			{
				q = XMQuaternionRotationAxis(up, -angle);//���̕����ɓ����N�I�[�^�j�I��
			}
#if 0
			XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
			orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsed_time);
#else
			orientation_vec = XMQuaternionMultiply(orientation_vec, q);
#endif
		}
	}

	DirectX::XMStoreFloat4(&orientation, orientation_vec);
}

void Player::ChainLockOn()
{


}

void Player::chain_search_update(float elapsed_time, BaseEnemy* stun_enemy)
{
	static bool lockon = false;
#ifdef USE_IMGUI
	ImGui::Begin("chain");
	ImGui::Text("search");
	if (ImGui::Button("lockon")) { lockon = true; }
	if (ImGui::Button("reset"))
	{
		stun_enemy_position = {};
		position = {};
	}
	ImGui::DragFloat3("stun_enemy_position", &stun_enemy_position.x, 0.1f);
	ImGui::End();
#endif // USE_IMGUI

	debug_figure->create_sphere(stun_enemy_position, 1.5f, { 1,0,0,1 });

	// �X�^�����ꂽ�G�����Ȃ���Βʏ�s���ɖ߂�
	//if (stun_enemy == nullptr) { transition_normal_behavior(); }
	//else // ���b�N�I���X�e�[�g��
	//{
	//	stun_enemy_position = stun_enemy->fGetPosition();
	//	transition_chain_lockon();
	//}

	if (lockon) { transition_chain_lockon(); lockon = false; }
}

void Player::transition_chain_search()
{
	player_chain_activity = &Player::chain_search_update;
}

void Player::chain_lockon_update(float elapsed_time, BaseEnemy* stun_enemy)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("lockon"); ImGui::End();
#endif // USE_IMGUI

	static float speed = 200.0f;
	static float play  = 0.2f;
	static bool start = false;
#ifdef USE_IMGUI
	ImGui::Begin("transit");
	ImGui::DragFloat3("position", &position.x, 0.1f);
	ImGui::DragFloat("speed", &speed, 0.1f);
	ImGui::DragFloat("play", &play, 0.01f);
	if (ImGui::Button("start")) { start = true; }
	if (ImGui::Button("restart")) { transit_index = 0; }
	ImGui::End();
#endif // USE_IMGUI

	for (const auto& point : interpolated_way_points)
	{
		debug_figure->create_sphere(point, 1.0f, { 1,1,1,1 });
	}
	for (const auto& point : way_points)
	{
		debug_figure->create_sphere(point, 1.5f, { 1,0,0,1 });
	}

	// �Ō�̓_(�G�̈ʒu)�ɂ�����U���X�e�[�g��
	if (start && transit(elapsed_time, transit_index, position, speed, interpolated_way_points, play))
	{
		start = false;
		//transition_chain_attack();
		transition_chain_search();
	}
	//else if (/*�L�����Z�������ꂪ������*/)
	//{

	//}
}

void Player::transition_chain_lockon()
{
	if (!way_points.empty()) way_points.clear();
	if (!interpolated_way_points.empty()) interpolated_way_points.clear();

	// �E�����x�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g���̊p�x���Z�o
	DirectX::XMVECTOR norm_vec = Math::calc_vector_AtoB_normalize(position, stun_enemy_position);
	DirectX::XMFLOAT3 right = { 1,0,0 };
	DirectX::XMVECTOR right_vec = DirectX::XMLoadFloat3(&right);
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(norm_vec, right_vec);
	float cos_theta = DirectX::XMVectorGetX(dot);
	// �E�����x�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g���̊p�x�ƔC�ӂ̊p�x�𑫂������̂����Ԓn�_�̃�
	const float theta = acosf(cos_theta) + DirectX::XMConvertToRadians(30.0f);
	// �^�[�Q�b�g�ւ̃x�N�g���̒������Z�o
	float length_vec = Math::calc_vector_AtoB_length(position, stun_enemy_position);
	// ���Ԓn�_�̎Z�o
	DirectX::XMFLOAT3 midpoint;
	if (position.z <= stun_enemy_position.z){ midpoint = { position.x + cosf(theta) * length_vec * 0.5f, position.y, position.z + sinf(theta) * length_vec * 0.5f }; }
	else { midpoint = { position.x + cosf(theta) * length_vec * 0.5f, position.y, position.z + -sinf(theta) * length_vec * 0.5f }; }
	// way_points�̐���
	way_points.emplace_back(position); // ���̎��_�ł̃v���C���[�̈ʒu
	way_points.emplace_back(midpoint); // �v���C���[�̈ʒu�ƓG�̈ʒu�̒��Ԓn�_
	way_points.emplace_back(stun_enemy_position); // �G�̈ʒu
	// way_points��ʂ�J�[�u���쐬
	CatmullRomSpline curve(way_points);
	curve.interpolate(interpolated_way_points, 3);

 	transit_index = 0;
	//model->;
	player_chain_activity = &Player::chain_lockon_update;
}

void Player::chain_move_update(float elapsed_time, BaseEnemy* stun_enemy)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("move"); ImGui::End();
#endif // USE_IMGUI

}

void Player::transition_chain_move()
{

	player_chain_activity = &Player::chain_move_update;
}

void Player::chain_attack_update(float elapsed_time, BaseEnemy* stun_enemy)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("attack"); ImGui::End();
#endif // USE_IMGUI



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