#include <assert.h>
#include "SwordTrail.h"
#include "Player.h"



bool Player::transit(float elapsed_time, int& index, DirectX::XMFLOAT3& position, float speed,
	const std::vector<DirectX::XMFLOAT3>& points, float play)
{
	assert(!points.empty() && "ポイントのサイズが0です");
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

		if (index != 0 && index % (STEPS * 2) == 0) { return true; } // 敵のポイントについた
	}

	position.x += velocity.x * speed * elapsed_time;
	position.y += velocity.y * speed * elapsed_time;
	position.z += velocity.z * speed * elapsed_time;

	return false;
}

void Player::rotate(float elapsed_time, int index, const std::vector<DirectX::XMFLOAT3>& points)
{
	using namespace DirectX;
	//ターゲットに向かって回転
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
				q = XMQuaternionRotationAxis(up, angle);//正の方向に動くクオータニオン
			}
			else
			{
				q = XMQuaternionRotationAxis(up, -angle);//正の方向に動くクオータニオン
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


#define CHAIN_DEBUG

#ifdef CHAIN_DEBUG
std::vector<DirectX::XMFLOAT3> debug_way_points;
std::vector<bool> debug_hit;
DirectX::XMFLOAT3 debug_point;
bool debug_lockon = false;
bool debug_transition_chain_lockon_flg = false;
bool attack_start = false;
#endif // CHAIN_DEBUG

void Player::transition_chain_search()
{
	if (!chain_lockon_enemy_indexes.empty()) chain_lockon_enemy_indexes.clear();
	if (!lockon_suggests.empty()) lockon_suggests.clear();

	// 初めの要素にはプレイヤーを入れる
	LockOnSuggest player_suggest;
	player_suggest.position = position;
	player_suggest.detection = true; // プレイヤーは検索には入れないので初めからtrue
	lockon_suggests.emplace_back(player_suggest);

	search_time = SEARCH_TIME;

#ifdef CHAIN_DEBUG
	debug_lockon = false;
	debug_transition_chain_lockon_flg = false;
	attack_start = false;
#endif // CHAIN_DEBUG

	player_chain_activity = &Player::chain_search_update;
}

void Player::chain_search_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain");
	ImGui::Text("search");
#ifdef CHAIN_DEBUG
	ImGui::Text("search_time:%f", search_time);
	if (ImGui::Button("lockon start")) { debug_lockon = true; }
	if (ImGui::Button("transition lockon")) { debug_transition_chain_lockon_flg = true; }
	if (ImGui::Button("points clear")) { debug_way_points.clear();	debug_hit.clear(); }
	if (ImGui::Button("reset"))
	{
		position = {};
		if (!lockon_suggests.empty()) lockon_suggests.clear();
		search_time = SEARCH_TIME;
	}
	ImGui::DragFloat3("debug_point", &debug_point.x, 0.1f);
	if (ImGui::Button("create")) { debug_way_points.emplace_back(debug_point); debug_hit.emplace_back(); }
#endif // CHAIN_DEBUG
	ImGui::End();
#endif // USE_IMGUI

#ifdef CHAIN_DEBUG
	for (auto pos : debug_way_points)
	{
		debug_figure->create_sphere(pos, 1.5f, { 0,1,0,1 });
	}
	//for (auto index : chain_lockon_enemy_indexes)
	//{
	//	debug_figure->create_sphere(enemies.at(index)->fGetPosition(), 2.0f, { 1,1,0,1 });
	//}
	debug_figure->create_sphere(debug_point, 1.5f, { 1,0,0,1 });
#endif // CHAIN_DEBUG


#ifdef CHAIN_DEBUG

	if(debug_lockon) // 決められた時間内に敵を索敵しロックオンステートへ
	{
		if (search_time > 0)
		{
			search_time -= elapsed_time;
			if (!lockon_suggests.empty())
			{
				lockon_suggests.clear();
				// 初めの要素にはプレイヤーを入れる
				LockOnSuggest player_suggest;
				player_suggest.position = position;
				player_suggest.detection = true; // プレイヤーは検索には入れないので初めからtrue
				lockon_suggests.emplace_back(player_suggest);
			}
			for (auto point : debug_way_points)
			{
				const DirectX::XMFLOAT3 minPoint{
					point.x - 1.5f * 0.5f,
					point.y - 1.5f * 0.5f,
					point.z - 1.5f * 0.5f
				};
				const DirectX::XMFLOAT3 maxPoint{
					point.x + 1.5f * 0.5f,
					point.y + 1.5f * 0.5f,
					point.z + 1.5f * 0.5f
				};

				if (Collision::frustum_vs_cuboid(minPoint, maxPoint)) // 索敵時間内に一度でも視錐台に映ればロックオン
				{
					debug_figure->create_sphere(point, 2.0f, { 0,0,1,1 });


					LockOnSuggest enemy_suggest; // サジェスト登録
					enemy_suggest.position = point;
					lockon_suggests.emplace_back(enemy_suggest);
				}
			}
		}
		else
		{
			if (debug_transition_chain_lockon_flg) { transition_chain_lockon_begin(); }
		}
	}
#else

	if (is_awakening) // 覚醒状態
	{
		// 敵がいなければ通常行動に戻る
		if (enemies.size() == 0) { transition_normal_behavior(); }

		// スタンしてなくてもロックオン
	    // 決められた時間内に敵を索敵しロックオンステートへ
		search_time -= elapsed_time;
		if (search_time > 0)
		{
			/*キャンセルがあれがここへ*/




			for (int i = 0; i < enemies.size(); ++i)
			{
				bool registered = false;
				for (auto index : chain_lockon_enemy_indexes)
				{
					if (index == i) // 一度登録したインデックスは登録しない
					{
						registered = true;
						break;
					}
				}
				if (!registered && enemies.at(i)->fComputeAndGetIntoCamera()) // 索敵時間内に一度でも視錐台に映ればロックオン(スタン関係なし)
				{
					chain_lockon_enemy_indexes.emplace_back(i); // 登録
					LockOnSuggest enemy_suggest; // サジェスト登録
					enemy_suggest.position = enemies.at(i)->fGetPosition();
					lockon_suggests.emplace_back(enemy_suggest);
				}
			}
		}
		else
		{
			transition_chain_lockon_begin();
		}
	}
	else // 非覚醒状態
	{
		bool is_stun = false;
		// スタンされた敵がいなければ通常行動に戻る
		for (const auto& enemy : enemies)
		{
			if (enemy->fGetStun())
			{
				is_stun = true;
				break;
			}
		}

		if (!is_stun) { transition_normal_behavior(); }
		else // 決められた時間内に敵を索敵しロックオンステートへ
		{
			search_time -= elapsed_time;
			if (search_time > 0)
			{
				/*キャンセルがあれがここへ*/




				for (int i = 0; i < enemies.size(); ++i)
				{
					bool registered = false;
					for (auto index : chain_lockon_enemy_indexes)
					{
						if (index == i) // 一度登録したインデックスは登録しない
						{
							registered = true;
							break;
						}
					}
					if (!registered && enemies.at(i)->fGetStun() && enemies.at(i)->fComputeAndGetIntoCamera()) // 索敵時間内に一度でも視錐台に映ればロックオン
					{
						chain_lockon_enemy_indexes.emplace_back(i); // 登録
						LockOnSuggest enemy_suggest; // サジェスト登録
						enemy_suggest.position = enemies.at(i)->fGetPosition();
						lockon_suggests.emplace_back(enemy_suggest);
					}
				}
			}
			else
			{
				transition_chain_lockon_begin();
			}
		}
	}
#endif // CHAIN_DEBUG

	//旋回処理
	RollTurn(position, orientation, elapsed_time);
}

void Player::transition_chain_lockon_begin()
{
	if (is_awakening) { model->play_animation(AwakingChargeInit); }
	else { model->play_animation(ChargeInit); }
	player_chain_activity = &Player::chain_lockon_begin_update;
}

void Player::chain_lockon_begin_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
	if (model->end_of_animation())
	{
		transition_chain_lockon();
	}
}

void Player::transition_chain_lockon()
{
	is_chain_attack = true;

	if (!sort_points.empty()) sort_points.clear();
	if (!way_points.empty()) way_points.clear();
	if (!interpolated_way_points.empty()) interpolated_way_points.clear();

	sort_points.emplace_back(position); // ソートされた位置の最初はプレイヤーの位置

	// プレイヤーとロックオンされた敵をプレイヤーと始点として基準距離以上の中で一番近い敵を次の要素に入れ、その敵を次の始点として同じ条件でソートする
	const float REFERENCE_DISTANCE = 10.0f; // 基準距離
	for (int start = 0; start < lockon_suggests.size() - 1; ++start) // 始点側
	{
		float champion_length{ FLT_MAX }; // 基準距離以上で最も始点に近い距離
		DirectX::XMFLOAT3 champion_pos{}; // 基準距離以上で最も始点に近い位置
		int champion_index = -1;          // 基準距離以上で最も始点に近い終点側のインデックス
		for (int end = 0; end < lockon_suggests.size(); ++end) // 終点側
		{
			float length = Math::calc_vector_AtoB_length(lockon_suggests.at(start).position, lockon_suggests.at(end).position);
			if (start != end && !lockon_suggests.at(end).detection && length >= REFERENCE_DISTANCE) // 検索条件：未検索かつ基準距離以上
			{
				if (champion_length > length)
				{
					champion_length = length;
					champion_pos    = lockon_suggests.at(end).position;
					champion_index  = end;
				}
			}
		}

		//-----< 始点側startから終点側endの総当たり終了 >-----//

		// 基準距離以上で最も始点に近い距離が見つかったので登録
		if (champion_index > -1)
		{
			lockon_suggests.at(champion_index).detection = true; // 検索済み
			sort_points.emplace_back(champion_pos);
		}
		else // 基準距離以上で最も始点に近い距離が見つからなかった
			 // パターン１：終点側が全て基準距離以内だった(未検索がまだある)→基準距離を0より大きく(始点側が未検索だった時対策)して再検索(検索済みははじく)
		 	 // パターン２：終点側の未検索がなかった→始点側を登録する
		{
			bool all_searching = true; // 終点側が全て検索されているかどうか
			for (int end = 0; end < lockon_suggests.size(); ++end) // 終点側
			{
				if (start != end && !lockon_suggests.at(end).detection) // 終点側( start != end )が未検索
				{
					all_searching = false; // 一つでも未検索があればパターン1確定
					break;
				}
			}
			// パターン1
			if (!all_searching) // 基準距離を0より大きくして再検索  検索条件：未検索かつ距離が0より大きい
			{
				// リセット
				champion_length = { FLT_MAX };
				champion_pos    = {};
				champion_index  = -1;
				for (int end = 0; end < lockon_suggests.size(); ++end) // 終点側
				{
					float length = Math::calc_vector_AtoB_length(lockon_suggests.at(start).position, lockon_suggests.at(end).position);
					if (start != end && !lockon_suggests.at(end).detection && length > 0) // 検索条件：未検索かつ距離が0より大きい
					{
						if (champion_length > length)
						{
							champion_length = length;
							champion_pos    = lockon_suggests.at(end).position;
							champion_index  = end;
						}
					}
					//--------<!! ここで距離が0以下ならまったく同じ位置にポイントがある !!>--------//
					else if (start != end && !lockon_suggests.at(end).detection && length <= 0)
					{
						champion_pos   = lockon_suggests.at(end).position;
						champion_index = end;
					}
				}
				assert(champion_index != -1 && "パターン1のchampion_indexが見つかっていません");
				assert(champion_index != start && "パターン1のchampion_indexが始点側になっています");
				lockon_suggests.at(champion_index).detection = true; // 検索済み
				sort_points.emplace_back(champion_pos); // 再検索終了
			}
			// パターン2
			else { sort_points.emplace_back(lockon_suggests.at(start).position); } // 始点側を登録
		}
	}

	assert(lockon_suggests.size() == sort_points.size() && "ソート前とソート後の要素数が異なっています");

	// ソートされたポイントからそれぞれの中間点を算出、そのすべてを通るように任意の分割数で分割したポイントを算出
	way_points.emplace_back(sort_points.at(0)); // プレイヤーの位置
	for (int index = 0; index < sort_points.size() - 1; ++index)
	{
		DirectX::XMFLOAT3 start = sort_points.at(index);
		DirectX::XMFLOAT3 end   = sort_points.at(index + 1);
		// 右方向ベクトルとターゲットへのベクトルの角度を算出
		DirectX::XMVECTOR norm_vec = Math::calc_vector_AtoB_normalize(start, end);
		DirectX::XMFLOAT3 right = { 1,0,0 };
		DirectX::XMVECTOR right_vec = DirectX::XMLoadFloat3(&right);
		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(norm_vec, right_vec);
		float cos_theta = DirectX::XMVectorGetX(dot);
		// 右方向ベクトルとターゲットへのベクトルの角度と任意の角度を足したものが中間地点のθ
		const float theta = acosf(cos_theta) + DirectX::XMConvertToRadians(30.0f);
		// ターゲットへのベクトルの長さを算出
		float length_vec = Math::calc_vector_AtoB_length(start, end);
		// 中間地点の算出
		DirectX::XMFLOAT3 midpoint;
		if (start.z <= end.z) { midpoint = { start.x + cosf(theta) * length_vec * 0.5f, start.y, start.z + sinf(theta) * length_vec * 0.5f }; }
		else { midpoint = { start.x + cosf(theta) * length_vec * 0.5f, start.y, start.z + -sinf(theta) * length_vec * 0.5f }; }
		// way_pointsの生成
		way_points.emplace_back(midpoint); // プレイヤーの位置と敵の位置の中間地点
		way_points.emplace_back(end); // 敵の位置
	}

	// way_pointsを通るカーブを作成
	CatmullRomSpline curve(way_points);
	curve.interpolate(interpolated_way_points, STEPS);

 	transit_index = 0;
	if (is_awakening) { model->play_animation(AwakingCharge, true); }
	else { model->play_animation(Charge, true); }
	player_chain_activity = &Player::chain_lockon_update;
}

void Player::chain_lockon_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("lockon"); ImGui::End();
#endif // USE_IMGUI

	static float base_speed = 100.0f;
	static float play  = 0.2f;
#ifdef CHAIN_DEBUG
#ifdef USE_IMGUI
	ImGui::Begin("transit");
	ImGui::Text("position x : %.1f  y : %.1f  z : %.1f", position.x, position.y, position.z);
	ImGui::DragFloat("base_speed", &base_speed, 0.1f);
	ImGui::DragFloat("play", &play, 0.01f);
	if (ImGui::Button("start")) { attack_start = true; }
	if (ImGui::Button("restart")) { transit_index = 0; }
	ImGui::Separator();
	static int elem = 0;
	constexpr int count = 4;
	const char* elems_names[count] = { "suggests","sort_points","way_points","interpolated" };

	const char* elem_name = (elem >= 0 && elem < count) ? elems_names[elem] : "Unknown";
	ImGui::SliderInt("sphere type", &elem, 0, count - 1, elem_name);

	ImGui::End();
#endif // USE_IMGUI
	switch (elem)
	{
	case 0: // lockon_suggestsデバッグ表示
		for (int i = 0; i < lockon_suggests.size(); ++i)
		{
			debug_figure->create_sphere(lockon_suggests.at(i).position, 1.5f, { 1,0,0,1 });
		}
		break;
	case 1: // sort_pointsデバッグ表示
		for (int i = 0; i < sort_points.size(); ++i)
		{
			if (i >= sort_points.size() - 1)
			{
				debug_figure->create_sphere(sort_points.at(i), 1.5f, { 0,0,1,1 });
			}
			else
			{
				if (debug_hit.at(i)) debug_figure->create_sphere(sort_points.at(i), 1.5f, { 0,1,0,1 });
				else debug_figure->create_sphere(sort_points.at(i), 1.5f, { 0,0,1,1 });
			}
		}
		break;
	case 2: // way_pointsデバッグ表示
		for (const auto& point : way_points)
		{
			debug_figure->create_sphere(point, 1.5f, { 0,0,1,1 });
		}
		break;
	case 3: // interpolated_way_pointsデバッグ表示
		for (const auto& point : interpolated_way_points)
		{
			debug_figure->create_sphere(point, 1.0f, { 1,1,1,1 });
		}
		break;
	}
#endif // CHAIN_DEBUG

	// speed増減
	float camera_to_player_length = Math::calc_vector_AtoB_length(camera_position, position);
	camera_to_player_length       = (std::min)(camera_to_player_length, 100.0f);
	float magnification           = camera_to_player_length / 100.0f * 10.0f;
	float speed = magnification * magnification + base_speed;

#ifdef USE_IMGUI
	ImGui::Begin("magnification");
	ImGui::Text("length:%f", camera_to_player_length);
	ImGui::Text("magnification:%f", magnification * magnification);
	ImGui::Text("speed:%f", speed);
	ImGui::End();
#endif // USE_IMGUI

#ifdef CHAIN_DEBUG
	if (attack_start && transit(elapsed_time, transit_index, position, speed, interpolated_way_points, play))
	{
		assert(transit_index != 0 && "意図していない挙動になっています");
		transition_chain_attack(); // 攻撃ステートへ

		for (int i = 0; i < sort_points.size() - 1; ++i)
		{
			float leng = Math::calc_vector_AtoB_length(position, sort_points.at(i));
			if (leng <= play) { debug_hit.at(i) = true; }
		}
	}
#else
	// ポイントについた時攻撃ステートへ
	if (transit(elapsed_time, transit_index, position, speed, interpolated_way_points, play))
	{
		assert(transit_index != 0 && "意図していない挙動になっています");
		transition_chain_attack(); // 攻撃ステートへ
	}
#endif // CHAIN_DEBUG
}

void Player::transition_chain_move()
{

	player_chain_activity = &Player::chain_move_update;
}

void Player::chain_move_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("move"); ImGui::End();
#endif // USE_IMGUI

}

void Player::transition_chain_attack()
{
	switch (attack_type)
	{
	case ATTACK_TYPE::FIRST:
		if (is_awakening) { model->play_animation(AwakingAttackType1, false, true, 0.3f, 2.0f); }
		else { model->play_animation(AttackType1, false, true, 0.3f, 2.0f); }
		attack_type = ATTACK_TYPE::SECOND;
		break;

	case ATTACK_TYPE::SECOND:
		if (is_awakening) { model->play_animation(AwakingAttackType2, false, true, 0.3f, 2.0f); }
		else { model->play_animation(AttackType2, false, true, 0.3f, 2.0f); }
		attack_type = ATTACK_TYPE::THIRD;
		break;

	case ATTACK_TYPE::THIRD:
		if (is_awakening) { model->play_animation(AwakingAttackType3, false, true, 0.3f, 2.0f); }
		else { model->play_animation(AttackType3, false, true, 0.3f, 2.0f); }
		attack_type = ATTACK_TYPE::FIRST;
		break;
	}

	player_chain_activity = &Player::chain_attack_update;
}

void Player::chain_attack_update(float elapsed_time, std::vector<BaseEnemy*> enemies)
{
#ifdef USE_IMGUI
	ImGui::Begin("chain"); ImGui::Text("attack"); ImGui::End();
#endif // USE_IMGUI



	// 攻撃が終わった
	if (model->end_of_animation())
	{
		// 分割したポイントの最後なら通常行動へ
		if (transit_index >= interpolated_way_points.size() - 1)
		{
			is_chain_attack = false;
			transition_normal_behavior();
		}
		else // ロックオンステートの初期化を通らず更新処理へ
		{
			player_chain_activity = &Player::chain_lockon_update;
			// 初期化通らないのでここでもアニメーション再生
			if (is_awakening) { model->play_animation(AwakingCharge, true); }
			else { model->play_animation(Charge, true); }
		}
	}
}