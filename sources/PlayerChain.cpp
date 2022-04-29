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

	rotate(elapsed_time, index, points);

	XMVECTOR length_vec = DirectX::XMVector3Length(vec);
	float length = DirectX::XMVectorGetX(length_vec);
	if (length <= play)
	{
		++index;
		position = points.at(index);

		return true; // �|�C���g�ɂ���
	}

	position.x += velocity.x * speed * elapsed_time;
	position.y += velocity.y * speed * elapsed_time;
	position.z += velocity.z * speed * elapsed_time;

	return false;
}

void Player::rotate(float elapsed_time, int index, const std::vector<DirectX::XMFLOAT3>& points)
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

	XMFLOAT3 d{};
	XMVECTOR d_vec = XMLoadFloat3(&points.at(index + 1)) - XMLoadFloat3(&points.at(index));
	XMVECTOR norm_vec = XMVector3Normalize(d_vec);
	XMStoreFloat3(&d, norm_vec);
	{
		XMVECTOR dot = XMVector3Dot(forward, norm_vec);
		float angle = acosf(DirectX::XMVectorGetX(dot));
		if (fabs(angle + FLT_EPSILON) > FLT_EPSILON)
		{
			XMVECTOR q;
			float cross{ (d.x * front.z) - (d.z * front.x) };
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


//#define CHAIN_DEBUG

#ifdef CHAIN_DEBUG
std::vector<DirectX::XMFLOAT3> debug_way_points;
int num = 0;
bool lockon = false;
#endif // CHAIN_DEBUG


void Player::chain_search_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain");
	ImGui::Text("search");
#ifdef CHAIN_DEBUG
	if (ImGui::Button("lockon")) { lockon = true; }
	if (ImGui::Button("points clear"))
	{
		debug_way_points.clear();
	}
	if (ImGui::Button("reset"))
	{
		num = 0;
		position = {};
	}
	ImGui::DragFloat3("stun_enemy_position", &stun_enemy_position.x, 0.1f);
	if (ImGui::Button("create"))
	{
		debug_way_points.emplace_back(stun_enemy_position);
	}
#endif // CHAIN_DEBUG
	ImGui::End();
#endif // USE_IMGUI

#ifdef CHAIN_DEBUG
	for (auto pos : debug_way_points)
	{
		debug_figure->create_sphere(pos, 1.5f, { 0,1,0,1 });
	}
	debug_figure->create_sphere(stun_enemy_position, 1.5f, { 1,0,0,1 });
#endif // CHAIN_DEBUG

	bool is_stun = false;
	// �X�^�����ꂽ�G�����Ȃ���Βʏ�s���ɖ߂�
	for (const auto& enemy : enemies)
	{
		if (enemy->fGetStun())
		{
			is_stun = true;
			break;
		}
	}

	if (!is_stun) { transition_normal_behavior(); }
	else // ���߂�ꂽ���ԓ��ɓG�����G�����b�N�I���X�e�[�g��
	{
		search_time -= elapsed_time;
		if (search_time > 0)
		{
            /*�L�����Z�������ꂪ������*/




			for (int i = 0; i < enemies.size(); ++i)
			{
				bool registered = false;
				for (auto index : chain_lockon_enemy_indexes)
				{
					if (index == i) // ��x�o�^�����C���f�b�N�X�͓o�^���Ȃ�
					{
						registered = true;
						break;
					}
				}
				if (!registered && enemies.at(i)->fGetStun() && enemies.at(i)->fComputeAndGetIntoCamera()) // ���G���ԓ��Ɉ�x�ł�������ɉf��΃��b�N�I��
				{
					chain_lockon_enemy_indexes.emplace_back(i); // �o�^
					LockOnSuggest enemy_suggest; // �T�W�F�X�g�o�^
					enemy_suggest.position = enemies.at(i)->fGetPosition();
					lockon_suggests.emplace_back(enemy_suggest);
				}
			}
		}
		else
		{
			transition_chain_lockon();
		}
	}

#ifdef CHAIN_DEBUG
	if (debug_way_points.size() <= num) { lockon = false; }
	else if (lockon)
	{
		stun_enemy_position = debug_way_points.at(num);
		transition_chain_lockon(); lockon = false;
	}
#endif // CHAIN_DEBUG
}

void Player::transition_chain_search()
{
	if (!chain_lockon_enemy_indexes.empty()) chain_lockon_enemy_indexes.clear();
	if (!lockon_suggests.empty()) lockon_suggests.clear();

	// ���߂̗v�f�ɂ̓v���C���[������
	LockOnSuggest player_suggest;
	player_suggest.position = position;
	player_suggest.detection = true; // �v���C���[�͌����ɂ͓���Ȃ��̂ŏ��߂���true
	lockon_suggests.emplace_back(player_suggest);

	player_chain_activity = &Player::chain_search_update;
}

void Player::chain_lockon_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("lockon"); ImGui::End();
#endif // USE_IMGUI

	static float speed = 200.0f;
	static float play  = 0.2f;
#ifdef CHAIN_DEBUG
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
#endif // CHAIN_DEBUG

#ifdef CHAIN_DEBUG
	if (start && transit(elapsed_time, transit_index, position, speed, interpolated_way_points, play))
	{
		lockon = true;
		++num;
		if (num >= debug_way_points.size())
		{
			start = false;
		}

		transition_chain_search();
	}
#else
	// �|�C���g�ɂ������U���X�e�[�g��
	if (transit(elapsed_time, transit_index, position, speed, interpolated_way_points, play))
	{
		assert(transit_index != 0 && "�Ӑ}���Ă��Ȃ������ɂȂ��Ă��܂�");
		transition_chain_attack(); // �U���X�e�[�g��
	}
#endif // CHAIN_DEBUG
}

void Player::transition_chain_lockon()
{
	if (!sort_points.empty()) sort_points.clear();
	if (!way_points.empty()) way_points.clear();
	if (!interpolated_way_points.empty()) interpolated_way_points.clear();

	sort_points.emplace_back(position); // �\�[�g���ꂽ�ʒu�̍ŏ��̓v���C���[�̈ʒu

	// �v���C���[�ƃ��b�N�I�����ꂽ�G���v���C���[�Ǝn�_�Ƃ��Ċ�����ȏ�̒��ň�ԋ߂��G�����̗v�f�ɓ���A���̓G�����̎n�_�Ƃ��ē��������Ń\�[�g����
	const float REFERENCE_DISTANCE = 10.0f; // �����
	for (int start = 0; start < lockon_suggests.size() - 1; ++start) // �n�_��
	{
		float champion_length{ FLT_MAX }; // ������ȏ�ōł��n�_�ɋ߂�����
		DirectX::XMFLOAT3 champion_pos{}; // ������ȏ�ōł��n�_�ɋ߂��ʒu
		int champion_index = -1;          // ������ȏ�ōł��n�_�ɋ߂��I�_���̃C���f�b�N�X
		for (int end = 0; end < lockon_suggests.size(); ++end) // �I�_��
		{
			float length = Math::calc_vector_AtoB_length(lockon_suggests.at(start).position, lockon_suggests.at(end).position);
			if (start != end && !lockon_suggests.at(end).detection && length >= REFERENCE_DISTANCE) // ���������F��������������ȏ�
			{
				if (champion_length > length)
				{
					champion_length = length;
					champion_pos    = lockon_suggests.at(end).position;
					champion_index  = end;
				}
			}
		}

		//-----< �n�_��start����I�_��end�̑�������I�� >-----//

		// ������ȏ�ōł��n�_�ɋ߂����������������̂œo�^
		if (champion_index > -1)
		{
			lockon_suggests.at(champion_index).detection = true; // �����ς�
			sort_points.emplace_back(champion_pos);
		}
		else // ������ȏ�ōł��n�_�ɋ߂�������������Ȃ�����
			 // �p�^�[���P�F�I�_�����S�Ċ�����ȓ�������(���������܂�����)���������0���傫��(�n�_�������������������΍�)���čČ���(�����ς݂͂͂���)
		 	 // �p�^�[���Q�F�I�_���̖��������Ȃ��������n�_����o�^����
		{
			bool all_searching = true; // �I�_�����S�Č�������Ă��邩�ǂ���
			for (int end = 0; end < lockon_suggests.size(); ++end) // �I�_��
			{
				if (start != end && !lockon_suggests.at(end).detection) // �I�_��( start != end )��������
				{
					all_searching = false; // ��ł�������������΃p�^�[��1�m��
					break;
				}
			}
			// �p�^�[��1
			if (!all_searching) // �������0���傫�����čČ���  ���������F��������������0�ȏ�
			{
				// ���Z�b�g
				champion_length = { FLT_MAX };
				champion_pos    = {};
				champion_index  = -1;
				for (int end = 0; end < lockon_suggests.size(); ++end) // �I�_��
				{
					float length = Math::calc_vector_AtoB_length(lockon_suggests.at(start).position, lockon_suggests.at(end).position);
					if (start != end && !lockon_suggests.at(end).detection && length > 0) // ���������F��������������0�ȏ�
					{
						if (champion_length > length)
						{
							champion_length = length;
							champion_pos    = lockon_suggests.at(end).position;
							champion_index  = end;
						}
					}
				}
				assert(champion_index != -1 && "�p�^�[��1��champion_index���������Ă��܂���");
				assert(champion_index != start && "�p�^�[��1��champion_index���n�_���ɂȂ��Ă��܂�");
				lockon_suggests.at(champion_index).detection = true; // �����ς�
				sort_points.emplace_back(champion_pos); // �Č����I��
			}
			// �p�^�[��2
			else { sort_points.emplace_back(lockon_suggests.at(start).position); } // �n�_����o�^
		}
	}

	assert(lockon_suggests.size() == sort_points.size() && "�\�[�g�O�ƃ\�[�g��̗v�f�����قȂ��Ă��܂�");


	// �\�[�g���ꂽ�|�C���g���炻�ꂼ��̒��ԓ_���Z�o�A���̂��ׂĂ�ʂ�悤�ɔC�ӂ̕������ŕ��������|�C���g���Z�o
	for (int index = 0; index < sort_points.size() - 1; ++index)
	{
		DirectX::XMFLOAT3 start = sort_points.at(index);
		DirectX::XMFLOAT3 end   = sort_points.at(index + 1);
		// �E�����x�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g���̊p�x���Z�o
		DirectX::XMVECTOR norm_vec = Math::calc_vector_AtoB_normalize(start, end);
		DirectX::XMFLOAT3 right = { 1,0,0 };
		DirectX::XMVECTOR right_vec = DirectX::XMLoadFloat3(&right);
		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(norm_vec, right_vec);
		float cos_theta = DirectX::XMVectorGetX(dot);
		// �E�����x�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g���̊p�x�ƔC�ӂ̊p�x�𑫂������̂����Ԓn�_�̃�
		const float theta = acosf(cos_theta) + DirectX::XMConvertToRadians(30.0f);
		// �^�[�Q�b�g�ւ̃x�N�g���̒������Z�o
		float length_vec = Math::calc_vector_AtoB_length(start, end);
		// ���Ԓn�_�̎Z�o
		DirectX::XMFLOAT3 midpoint;
		if (start.z <= end.z) { midpoint = { start.x + cosf(theta) * length_vec * 0.5f, start.y, start.z + sinf(theta) * length_vec * 0.5f }; }
		else { midpoint = { start.x + cosf(theta) * length_vec * 0.5f, start.y, start.z + -sinf(theta) * length_vec * 0.5f }; }
		// way_points�̐���
		way_points.emplace_back(start); // ���̎��_�ł̃v���C���[�̈ʒu
		way_points.emplace_back(midpoint); // �v���C���[�̈ʒu�ƓG�̈ʒu�̒��Ԓn�_
		way_points.emplace_back(end); // �G�̈ʒu
	}

	// way_points��ʂ�J�[�u���쐬
	CatmullRomSpline curve(way_points);
	curve.interpolate(interpolated_way_points, 3);

 	transit_index = 0;
	//model->;
	player_chain_activity = &Player::chain_lockon_update;
}

void Player::chain_move_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("move"); ImGui::End();
#endif // USE_IMGUI

}

void Player::transition_chain_move()
{

	player_chain_activity = &Player::chain_move_update;
}

void Player::chain_attack_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("attack"); ImGui::End();
#endif // USE_IMGUI



	// �U�����I�����
	//if (�U�����I����)
	{
		// ���������|�C���g�̍Ō�Ȃ�ʏ�s����
		if (transit_index >= interpolated_way_points.size()) { transition_normal_behavior(); }
		else // ���b�N�I���X�e�[�g�̏�������ʂ炸�X�V������
		{
			player_chain_activity = &Player::chain_lockon_update;
		}
	}
}

void Player::transition_chain_attack()
{
	player_chain_activity = &Player::chain_attack_update;
}