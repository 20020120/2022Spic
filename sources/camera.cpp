#include "framework.h"
#include "camera.h"
#include "user.h"

DirectX::XMFLOAT4X4 Camera::keep_view = {};
DirectX::XMFLOAT4X4 Camera::keep_projection = {};

Camera::Camera(GraphicsPipeline& graphics,Player* player)
{
	HRESULT hr{ S_OK };
	//----定数バッファ----//
	// カメラ関連
	scene_constants = std::make_unique<Constants<SceneConstants>>(graphics.get_device().Get());
	// orientationの初期化
	{
		DirectX::XMFLOAT3 n(0, 1, 0); // 軸（正規化）
		constexpr float angle = DirectX::XMConvertToRadians(0); //角度（ラジアン）
		orientation = {
			sinf(angle / 2) * n.x,
			sinf(angle / 2) * n.y,
			sinf(angle / 2) * n.z,
			cosf(angle / 2)
		};
	}
	using namespace DirectX;
	const DirectX::XMFLOAT3 playerPosition = player->GetPosition();
	const DirectX::XMFLOAT3 playerForward = player->GetForward();
	const DirectX::XMFLOAT3 playerUp = player->GetUp();

	const DirectX::XMVECTOR PlayerPosition = DirectX::XMLoadFloat3(&playerPosition);
	const DirectX::XMVECTOR PlayerForward = DirectX::XMLoadFloat3(&playerForward);
	const DirectX::XMVECTOR PlayerUp = DirectX::XMLoadFloat3(&playerUp);

	DirectX::XMVECTOR EyeVector = -PlayerForward * 10 + PlayerUp;
	const DirectX::XMVECTOR Radius = DirectX::XMVector3Length(EyeVector);
	DirectX::XMStoreFloat(&radius, Radius);
	EyeVector = DirectX::XMVector3Normalize(EyeVector);
	DirectX::XMStoreFloat3(&eyeVector, EyeVector);

	const DirectX::XMVECTOR Target = PlayerPosition + PlayerUp * 3;
	DirectX::XMStoreFloat3(&target, Target);

	const DirectX::XMVECTOR Eye = Target + EyeVector * radius;
	DirectX::XMStoreFloat3(&eye, Eye);

}

void Camera::update_with_euler_angles(float elapsed_time)
{
    using namespace DirectX;
	// マウス操作でカメラ操作
	mouse_operation_with_euler_angles(elapsed_time);
	// キー入力によって回転
#if 1
	keyboard_operation_with_euler_angles(elapsed_time);
#endif // 0
	// X軸のカメラ回転を制限
	angle.x = Math::clamp(angle.x, min_angle_x, max_angle_x);
	// Y軸の回転値を-3.14~3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI) { angle.y += DirectX::XM_2PI; }
	if (angle.y > DirectX::XM_PI) { angle.y -= DirectX::XM_2PI; }
	// rangeの範囲制御
	range = Math::clamp(range, 2.0f, 95.0f);
	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR forward_vec = transform.r[2];
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, forward_vec);
	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	eye.x = target.x - forward.x * range;
	eye.y = target.y - forward.y * range;
	eye.z = target.z - forward.z * range;
	// カメラの視点と注意点を設定
	DirectX::XMFLOAT3 up(0, 1, 0);
	XMVECTOR eye_vec   = XMLoadFloat3(&eye);
	XMVECTOR focus_vec = XMLoadFloat3(&target);
	XMVECTOR up_vec    = XMLoadFloat3(&up);
	XMMATRIX view_mat  = XMMatrixLookAtLH(eye_vec, focus_vec, up_vec); // V
	XMStoreFloat4x4(&view, view_mat);
	// プロジェクション行列を作成
	float width = static_cast<float>(SCREEN_WIDTH);
	float height = static_cast<float>(SCREEN_HEIGHT);
	float aspect_ratio{ width / height };
#ifdef USE_IMGUI
	imgui_menu_bar("objects", "camera", display_camera_imgui);
	if (display_camera_imgui)
	{
		ImGui::Begin("Camera", false);
		ImGui::DragFloat2("near_far", &near_far.x, 0.1f, 0.1f, 2000.0f);
		ImGui::End();
	}
#endif // USE_IMGUI

	XMMATRIX projection_mat = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), aspect_ratio, near_far.x, near_far.y); // P
	XMStoreFloat4x4(&projection, projection_mat);
}

void Camera::update_with_quaternion(float elapsed_time)
{
	using namespace DirectX;
	// マウス操作でカメラ操作
	mouse_operation_with_quaternion(elapsed_time);
	// キー入力によって回転
#if 1
	keyboard_operation_with_quaternion(elapsed_time);
#endif // 0
	// rangeの範囲制御
	range = Math::clamp(range, 0.5f, 95.0f);
	// カメラ回転値を回転行列に変換
	// XMVECTORクラスへ変換
	XMVECTOR orientation_vec = XMLoadFloat4(&orientation);
	//回転行列から各方向を出す
	XMVECTOR forward_vec, right_vec, up_vec;
	XMMATRIX m = XMMatrixRotationQuaternion(orientation_vec);
	XMFLOAT4X4 m4x4 = {};
	XMStoreFloat4x4(&m4x4, m);
	right_vec = { m4x4._11, m4x4._12, m4x4._13 };
	XMFLOAT3 up{ 0,1,0 };
	up_vec = XMLoadFloat3(&up);
	forward_vec = { m4x4._31, m4x4._32, m4x4._33 };
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, XMVector3Normalize(forward_vec));
	// 回転の制御
	XMVECTOR dot_vec = XMVector3Dot(XMVector3Normalize(forward_vec), up_vec);
	float dot;
	XMStoreFloat(&dot, dot_vec);
	float angle = XMConvertToDegrees(acosf(dot));
	if (angle > 170 && vertical_rotation_degree > 0)
	{
		vertical_rotation_degree = static_cast<float>((std::min)(0, (int)vertical_rotation_degree));
	}
	if (angle < 10 && vertical_rotation_degree < 0)
	{
		vertical_rotation_degree = static_cast<float>((std::max)(0, (int)vertical_rotation_degree));
	}
	// 上下
	{
		DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(right_vec, DirectX::XMConvertToRadians(vertical_rotation_degree));
		orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
		vertical_rotation_degree = 0;
	}
	// 左右
	{
		DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(up_vec, DirectX::XMConvertToRadians(horizon_rotation_degree));
		orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
		horizon_rotation_degree = 0;
	}
	// orientation更新
	XMStoreFloat4(&orientation, orientation_vec);
	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	eye.x = target.x - forward.x * range;
	eye.y = target.y - forward.y * range;
	eye.z = target.z - forward.z * range;
	// カメラの視点と注意点を設定
	XMVECTOR eye_vec = XMLoadFloat3(&eye);
	XMVECTOR focus_vec = XMLoadFloat3(&target);
	XMMATRIX view_mat = XMMatrixLookAtLH(eye_vec, focus_vec, XMLoadFloat3(&up)); // V
	XMStoreFloat4x4(&view, view_mat);
	//ビュー行列を逆行列かしワールド行列に戻す
	DirectX::XMMATRIX world_vec = DirectX::XMMatrixInverse(nullptr, view_mat);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, world_vec);
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->forward.x = world._31;
	this->forward.y = world._32;
	this->forward.z = world._33;

	// プロジェクション行列を作成
	float width = static_cast<float>(SCREEN_WIDTH);
	float height = static_cast<float>(SCREEN_HEIGHT);
	float aspect_ratio{ width / height };
#ifdef USE_IMGUI
	imgui_menu_bar("objects", "camera", display_camera_imgui);
	if (display_camera_imgui)
	{
		ImGui::Begin("Camera", false);
		ImGui::DragFloat2("near_far", &near_far.x, 0.1f, 0.1f, 2000.0f);
		ImGui::End();
	}
#endif // USE_IMGUI
	XMMATRIX projection_mat = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), aspect_ratio, near_far.x, near_far.y); // P
	XMStoreFloat4x4(&projection, projection_mat);
}

void Camera::calc_view_projection(GraphicsPipeline& graphics)
{
	// ビュー・プロジェクション変換行列を計算し、それを定数バッファにセットする
	using namespace DirectX;
#ifdef USE_IMGUI
	if (display_camera_imgui)
	{
		ImGui::Begin("Camera", false);
		ImGui::DragFloat4("LightDirection", &scene_constants->data.light_direction.x, 0.01f, -1, 1);
		ImGui::ColorEdit4("light_color", &scene_constants->data.light_color.x);
		ImGui::End();
		debug_gui();
	}
#endif
	// ビュー行列/プロジェクション行列を作成
	XMMATRIX V = XMLoadFloat4x4(&view);
	XMMATRIX P = XMLoadFloat4x4(&projection);
	keep_view = view;
	keep_projection = projection;
	// 定数バッファにフェッチする
	XMStoreFloat4x4(&scene_constants->data.view_projection, V * P);
	scene_constants->data.camera_position = { eye.x,eye.y,eye.z,0 };
	scene_constants->data.shake_matrix = camera_shake->get_shake_matrix();
	XMStoreFloat4x4(&scene_constants->data.inverse_view_projection, XMMatrixInverse(nullptr, V * P));

	scene_constants->bind(graphics.get_dc().Get(), 1, CB_FLAG::PS_VS_GS);
}

void Camera::mouse_operation_with_euler_angles(float elapsed_time)
{
	if (is_mouse_operation)
	{
		const float CURSOL_PLAY = 2.0f;
		// マウスホイールでカメラ寄り、引き
		{
			if (mouse->up_tilt()) {
				/*↑に回転（チルト）した */ range -= 1000.0f * elapsed_time;
			}
			else if (mouse->down_tilt()) {
				/*↓に回転（チルト）した*/ range += 1000.0f * elapsed_time;
			}
		}
		// 左クリック押しながらマウス移動で平行移動
		{
			if (mouse->common_button() & Mouse::BTN_LEFT_CLICK) {
				if (mouse->get_cursor_pos().x < mouse->get_old_cursor_pos().x - CURSOL_PLAY) { target.x -= 20.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().x > mouse->get_old_cursor_pos().x + CURSOL_PLAY) { target.x += 20.0f * elapsed_time; }
				if (mouse->get_cursor_pos().y < mouse->get_old_cursor_pos().y - CURSOL_PLAY) { target.y += 20.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().y > mouse->get_old_cursor_pos().y + CURSOL_PLAY) { target.y -= 20.0f * elapsed_time; }
			}
		}
		// 右クリック押しながらマウス移動で回転
		{
			if (mouse->common_button() & Mouse::BTN_RIGHT_CLICK) {
				if (mouse->get_cursor_pos().x < mouse->get_old_cursor_pos().x - CURSOL_PLAY) { angle.y -= 30.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().x > mouse->get_old_cursor_pos().x + CURSOL_PLAY) { angle.y += 30.0f * elapsed_time; }
				if (mouse->get_cursor_pos().y < mouse->get_old_cursor_pos().y - CURSOL_PLAY) { angle.x += 30.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().y > mouse->get_old_cursor_pos().y + CURSOL_PLAY) { angle.x -= 30.0f * elapsed_time; }
			}
		}
	}
}

void Camera::keyboard_operation_with_euler_angles(float elapsed_time)
{
	// カメラの回転速度
	float speed = roll_speed * elapsed_time;
	float ax{};
	float ay{};
	if (mouse->debug_button() & Mouse::BTN_A_LEFT) { ax = 1; }
	if (mouse->debug_button() & Mouse::BTN_D_RIGHT) { ax = -1; }
	if (mouse->debug_button() & Mouse::BTN_W_UP) { ay = 1; }
	if (mouse->debug_button() & Mouse::BTN_S_DOWN) { ay = -1; }
	angle.x += ay * speed;
	angle.y += ax * speed;
}

void Camera::mouse_operation_with_quaternion(float elapsed_time)
{
	using namespace DirectX;
	// XMVECTORクラスへ変換
	XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&orientation);
	//回転行列から各方向を出す
	XMVECTOR forward_vec, right_vec, up_vec;
	XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
	XMFLOAT4X4 m4x4 = {};
	XMStoreFloat4x4(&m4x4, m);
	right_vec = { m4x4._11, m4x4._12, m4x4._13 };
	up_vec = { 0, 1, 0 };
	forward_vec = { m4x4._31, m4x4._32, m4x4._33 };

	if (is_mouse_operation)
	{
		const float CURSOL_PLAY = 2.0f;
		// マウスホイールでカメラ寄り、引き
		{
			if (mouse->up_tilt()) {
				/*↑に回転（チルト）した */ range -= 1000.0f * elapsed_time;
			}
			else if (mouse->down_tilt()) {
				/*↓に回転（チルト）した*/ range += 1000.0f * elapsed_time;
			}
		}
		// 左クリック押しながらマウス移動で平行移動
		{
			if (mouse->common_button() & Mouse::BTN_LEFT_CLICK) {
				if (mouse->get_cursor_pos().x < mouse->get_old_cursor_pos().x - CURSOL_PLAY) { target.x -= 20.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().x > mouse->get_old_cursor_pos().x + CURSOL_PLAY) { target.x += 20.0f * elapsed_time; }
				if (mouse->get_cursor_pos().y < mouse->get_old_cursor_pos().y - CURSOL_PLAY) { target.y += 20.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().y > mouse->get_old_cursor_pos().y + CURSOL_PLAY) { target.y -= 20.0f * elapsed_time; }
			}
		}
		// 右クリック押しながらマウス移動で回転
		{
			if (mouse->common_button() & Mouse::BTN_RIGHT_CLICK) {
				if (mouse->get_cursor_pos().x < mouse->get_old_cursor_pos().x - CURSOL_PLAY) {
					DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(up_vec, DirectX::XMConvertToRadians(200 * elapsed_time));
					orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
				}
				else if (mouse->get_cursor_pos().x > mouse->get_old_cursor_pos().x + CURSOL_PLAY) {
					DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(up_vec, DirectX::XMConvertToRadians(-200 * elapsed_time));
					orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
				}
				if (mouse->get_cursor_pos().y < mouse->get_old_cursor_pos().y - CURSOL_PLAY) {
					DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(right_vec, DirectX::XMConvertToRadians(200 * elapsed_time));
					orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
				}
				else if (mouse->get_cursor_pos().y > mouse->get_old_cursor_pos().y + CURSOL_PLAY) {
					DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(right_vec, DirectX::XMConvertToRadians(-200 * elapsed_time));
					orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
				}
			}
		}
	}
	// orientation_vecからorientationを更新
	XMStoreFloat4(&orientation, orientation_vec);
}

void Camera::keyboard_operation_with_quaternion(float elapsed_time)
{
	using namespace DirectX;
	// XMVECTORクラスへ変換
	XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&orientation);
	//回転行列から各方向を出す
	XMVECTOR forward_vec;
	XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
	XMFLOAT4X4 m4x4 = {};
	XMStoreFloat4x4(&m4x4, m);
	forward_vec = { m4x4._31, m4x4._32, m4x4._33 };


	// I K で前後移動
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, forward_vec);
	if (mouse->debug_button() & Mouse::BTN_I)
	{
		target.x += forward.x * 20 * elapsed_time;
		target.y += forward.y * 20 * elapsed_time;
		target.z += forward.z * 20 * elapsed_time;
	}
	if (mouse->debug_button() & Mouse::BTN_K)
	{
		target.x -= forward.x * 20 * elapsed_time;
		target.y -= forward.y * 20 * elapsed_time;
		target.z -= forward.z * 20 * elapsed_time;
	}
	// W A S D で回転
	{
		if (mouse->debug_button() & Mouse::BTN_D_RIGHT) { horizon_rotation_degree = 100 * elapsed_time; }
		else if (mouse->debug_button() & Mouse::BTN_A_LEFT) { horizon_rotation_degree = -100 * elapsed_time; }
		if (mouse->debug_button() & Mouse::BTN_S_DOWN) { vertical_rotation_degree = 100 * elapsed_time; }
		else if (mouse->debug_button() & Mouse::BTN_W_UP) { vertical_rotation_degree = -100 * elapsed_time; }
	}
}

void Camera::debug_gui()
{
#ifdef USE_IMGUI
	if (display_camera_imgui)
	{
		ImGui::Begin("Camera");
		ImGui::Checkbox("mouse_operation", &is_mouse_operation);
		ImGui::DragFloat("range", &range, 0.2f);
		ImGui::DragFloat2("angle", &angle.x, 0.1f);
		ImGui::DragFloat3("target", &target.x, 0.1f, -100, 100);
		ImGui::End();
	}
#endif
}

void Camera::Update(float elapsedTime, Player* player)
{
	using namespace DirectX;

	const DirectX::XMFLOAT3 playerForward = player->GetForward();
	const DirectX::XMFLOAT3 playerUp = player->GetUp();
	const DirectX::XMFLOAT3 playerPosition = player->GetPosition();

	const DirectX::XMVECTOR PlayerForward = DirectX::XMLoadFloat3(&playerForward);
	const DirectX::XMVECTOR PlayerUp = DirectX::XMLoadFloat3(&playerUp);
	const DirectX::XMVECTOR PlayerPosition = DirectX::XMLoadFloat3(&playerPosition);

	if(player->GetEnemyLockOn())
	{
		//if (player->GetCameraReset())
		//{
		//	if (RockOnCameraReset(elapsedTime, PlayerForward, PlayerUp))
		//	{
		//		player->FalseCameraReset();
		//	}
		//}
		SetAngle(elapsedTime);
		const DirectX::XMFLOAT3 playerTarget = player->GetTarget();
		const DirectX::XMVECTOR PlayerTarget = DirectX::XMLoadFloat3(&playerTarget);
		RockOnCalculateEyeVector(PlayerPosition, PlayerTarget);
		if(RockOnUpdateEyeVector(elapsedTime, PlayerUp, player->GetCameraLockOn()))
		{
		    player->FalseCameraLockOn();
		}
	}
	else
	{
		if (player->GetCameraReset())
		{
			if (CameraReset(elapsedTime, PlayerForward, PlayerUp))
			{
				player->FalseCameraReset();
			}
		}
		SetAngle(elapsedTime);
		UpdateEyeVector(elapsedTime, PlayerUp);
	}
	UpdateTarget(PlayerPosition, PlayerUp);
	UpdateEye();
	AttitudeControl(elapsedTime);

#ifdef USE_IMGUI
	ImGui::Begin("Camera", false);
	ImGui::SliderFloat3("Position", &eye.x, -10.0f, 10.0f);
	ImGui::InputFloat3("target", &target.x);

	ImGui::InputFloat3("forward", &forward.x);

	ImGui::SliderFloat("range", &radius,3.0f,15.0f);
	ImGui::End();
#endif


	XMFLOAT3 up = { 0, 1, 0 };
	XMVECTOR eye_vec = XMLoadFloat3(&eye);
	XMVECTOR focus_vec = XMLoadFloat3(&target);
	XMMATRIX view_mat = XMMatrixLookAtLH(eye_vec, focus_vec, DirectX::XMLoadFloat3(&up)); // V
	XMStoreFloat4x4(&view, view_mat);
	//ビュー行列を逆行列かしワールド行列に戻す
	DirectX::XMMATRIX world_vec = DirectX::XMMatrixInverse(nullptr, view_mat);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, world_vec);
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->forward.x = world._31;
	this->forward.y = world._32;
	this->forward.z = world._33;

	float width = static_cast<float>(SCREEN_WIDTH);
	float height = static_cast<float>(SCREEN_HEIGHT);
	float aspect_ratio{ width / height };

	XMMATRIX projection_mat = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), aspect_ratio, near_far.x, near_far.y); // P
	XMStoreFloat4x4(&projection, projection_mat);

}

void Camera::SetAngle(float elapsedTime)
{
	float ax = game_pad->get_axis_RX();
	float ay = game_pad->get_axis_RY();

	if(ax > 0.1f || ax < 0.1f)
	{
		horizonDegree = 180 * ax * elapsedTime;
	}
	if(ay > 0.1f || ay < 0.1f)
	{
		verticalDegree = 180 * -ay * elapsedTime;
	}
}

void Camera::UpdateEyeVector(float elapsedTime, DirectX::XMVECTOR PlayerUp)
{
	//変化が無ければスルー
	if (horizonDegree < 0.1f && horizonDegree >-0.1f
		&& verticalDegree < 0.1f && verticalDegree > -0.1f)return;

	DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);
	DirectX::XMFLOAT3 up = { 0,1,0 };
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

	DirectX::XMVECTOR Right = DirectX::XMVector3Cross(Up, DirectX::XMVectorScale(EyeVector, -1));

	//プレイヤーに対して縦方向の回転
	//プレイヤーの真上と真下に近いときは回転しない
	if (verticalDegree > 0.1f || verticalDegree < -0.1f)
	{
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(PlayerUp), DirectX::XMVector3Normalize(EyeVector));
		float dot{};
		DirectX::XMStoreFloat(&dot, Dot);
		//真上に近い時
		if(dot > 0.9f)
		{
		    if(verticalDegree < 0.1f)
		    {
				DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(Right);
				DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(verticalDegree));
				EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
			}
			verticalDegree = 0;
		}
		//真下に近い時
		else if(dot < -0.9f)
		{
			if (verticalDegree > 0.1f)
			{
				DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(Right);
				DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(verticalDegree));
				EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
			}
		    verticalDegree = 0;
		}
		//通常時
		else
		{
			DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(Right);
			DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(verticalDegree));
			EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
			verticalDegree = 0;
		}
	}
	//プレイヤーに対して横の回転
	if (horizonDegree > 0.1f || horizonDegree < -0.1f)
	{
		DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(PlayerUp);
		DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(horizonDegree));
		EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
		horizonDegree = 0;
	}


	//最後に一回だけ行う
	DirectX::XMStoreFloat3(&eyeVector, EyeVector);
}

void Camera::AttitudeControl(float elapsedTime)
{
	using namespace DirectX;
	DirectX::XMVECTOR Orientation = DirectX::XMLoadFloat4(&orientation);
	DirectX::XMVECTOR Forward, Right,Up;
	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(Orientation);
	DirectX::XMFLOAT4X4 m4x4 = {};
	DirectX::XMStoreFloat4x4(&m4x4, m);
	Right = { m4x4._11, m4x4._12, m4x4._13 };
	Up = { 0,1,0 };
	Forward = { m4x4._31, m4x4._32, m4x4._33 };

	DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);
	EyeVector *= -1;

	DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(EyeVector), DirectX::XMVector3Normalize(Forward));
	float dot{};
	DirectX::XMStoreFloat(&dot, Dot);
	const float angle = acosf(dot);

	if (dot > 0.98f)
	{
		//DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(EyeVector, Forward);
		DirectX::XMVECTOR Axis = Up;
		DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, angle);
		Orientation = DirectX::XMQuaternionMultiply(Orientation, Quaternion);
	}



	DirectX::XMStoreFloat4(&orientation, Orientation);
}

void Camera::UpdateEye()
{
	using namespace DirectX;

	const DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
	const DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);
	const DirectX::XMVECTOR Eye = Target + EyeVector * radius;
	DirectX::XMStoreFloat3(&eye, Eye);
}

void Camera::UpdateTarget(DirectX::XMVECTOR PlayerPosition, DirectX::XMVECTOR PlayerUp)
{
	using namespace DirectX;

	DirectX::XMVECTOR Target = PlayerPosition + PlayerUp * 3;
	DirectX::XMStoreFloat3(&target, Target);
}

bool Camera::CameraReset(float elapsedTime, DirectX::XMVECTOR PlayerForward, DirectX::XMVECTOR PlayerUp)
{
	static float rotateTimer{ 0 };
	using namespace DirectX;

	if (rotateTimer < 0.1f)
	{
		rotateTimer += elapsedTime;
		DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);
		DirectX::XMVECTOR RotateEyeVector = -PlayerForward * 10 + PlayerUp;
		RotateEyeVector = DirectX::XMVector3Normalize(RotateEyeVector);

		const DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(EyeVector, RotateEyeVector);
		float dot{};
		DirectX::XMStoreFloat(&dot, Dot);
		const float angle = acosf(dot);

		const DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(EyeVector, RotateEyeVector);

		const DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, angle * elapsedTime / 0.1f);
		EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
		DirectX::XMStoreFloat3(&eyeVector, EyeVector);
	}
	else
	{
		DirectX::XMVECTOR EyeVector = -PlayerForward * 10 + PlayerUp;
		EyeVector = DirectX::XMVector3Normalize(EyeVector);
		DirectX::XMStoreFloat3(&eyeVector, EyeVector);
		rotateTimer = 0;
		return true;
	}
	return false;
}

bool Camera::RockOnCameraReset(float elapsedTime, DirectX::XMVECTOR PlayerForward, DirectX::XMVECTOR PlayerUp)
{
	static float rotateTimer{ 0 };
	using namespace DirectX;

	if (rotateTimer < 0.1f)
	{
		rotateTimer += elapsedTime;
		DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);
		DirectX::XMVECTOR RotateEyeVector = DirectX::XMLoadFloat3(&rockOnEyeVector);
		RotateEyeVector = DirectX::XMVector3Normalize(RotateEyeVector);

		const DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(EyeVector, RotateEyeVector);
		float dot{};
		DirectX::XMStoreFloat(&dot, Dot);
		const float angle = acosf(dot);

		const DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(EyeVector, RotateEyeVector);

		const DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, angle * elapsedTime / 0.1f);
		EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
		DirectX::XMStoreFloat3(&eyeVector, EyeVector);
	}
	else
	{
		DirectX::XMVECTOR EyeVector = -PlayerForward * 10 + PlayerUp;
		EyeVector = DirectX::XMVector3Normalize(EyeVector);
		DirectX::XMStoreFloat3(&eyeVector, EyeVector);
		rotateTimer = 0;
		return true;
	}
	return false;

}

bool Camera::RockOnUpdateEyeVector(float elapsedTime, DirectX::XMVECTOR PlayerUp,bool rockOnStart)
{
	if (rockOnStart)
	{
		if (rockOnTimer < 0.1f)
		{
			rockOnTimer += elapsedTime;
			DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);
			DirectX::XMVECTOR RotateEyeVector = DirectX::XMLoadFloat3(&rockOnEyeVector);
			RotateEyeVector = DirectX::XMVector3Normalize(RotateEyeVector);

			const DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(EyeVector, RotateEyeVector);
			float dot{};
			DirectX::XMStoreFloat(&dot, Dot);
			const float angle = acosf(dot);

			const DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(EyeVector, RotateEyeVector);

			const DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, angle * elapsedTime / 0.1f);
			EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
			DirectX::XMStoreFloat3(&eyeVector, EyeVector);
			return false;
		}
		else
		{
			eyeVector = rockOnEyeVector;
			rockOnTimer = 0;
			return true;
		}
	}
	else
	{
		//左右のずれ修正
		DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);

		const DirectX::XMVECTOR PlayerToRockOn = DirectX::XMLoadFloat3(&playerToRockOn);
		DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(PlayerUp, PlayerToRockOn);
		Cross = DirectX::XMVector3Normalize(Cross);
		const DirectX::XMVECTOR ProjectionLength = DirectX::XMVector3Dot(Cross, EyeVector);
		float projectionLength{};
		DirectX::XMStoreFloat(&projectionLength, ProjectionLength);

		EyeVector = DirectX::XMVectorSubtract(EyeVector, DirectX::XMVectorScale(Cross, projectionLength));
		EyeVector = DirectX::XMVector3Normalize(EyeVector);

		//変化が無ければスルー
		if (horizonDegree < 0.1f && horizonDegree >-0.1f
			&& verticalDegree < 0.1f && verticalDegree > -0.1f){}
		else
		{
			DirectX::XMFLOAT3 up = { 0,1,0 };
			DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

			DirectX::XMVECTOR Right = DirectX::XMVector3Cross(Up, DirectX::XMVectorScale(EyeVector, -1));

			//プレイヤーに対して縦方向の回転
			//プレイヤーの真上と真下に近いときは回転しない
			if (verticalDegree > 0.1f || verticalDegree < -0.1f)
			{
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(PlayerUp), DirectX::XMVector3Normalize(EyeVector));
				float dot{};
				DirectX::XMStoreFloat(&dot, Dot);
				//真上に近い時
				if (dot > 0.9f)
				{
					if (verticalDegree < 0.1f)
					{
						DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(Right);
						DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(verticalDegree));
						EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
					}
					verticalDegree = 0;
				}
				//真下に近い時
				else if (dot < -0.9f)
				{
					if (verticalDegree > 0.1f)
					{
						DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(Right);
						DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(verticalDegree));
						EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
					}
					verticalDegree = 0;
				}
				//通常時
				else
				{
					DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(Right);
					DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(verticalDegree));
					EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
					verticalDegree = 0;
				}
			}
		}
		//最後に一回だけ行う
		DirectX::XMStoreFloat3(&eyeVector, EyeVector);
		return false;
	}
}

void Camera::RockOnCalculateEyeVector(DirectX::XMVECTOR PlayerPosition, DirectX::XMVECTOR RockOnPosition)
{
	using namespace DirectX;

	DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
	Target -= PlayerPosition;
	const DirectX::XMVECTOR PlayerToRockOn = RockOnPosition - PlayerPosition;
	const DirectX::XMVECTOR Distance = DirectX::XMVector3Length(PlayerToRockOn);
	float distance{};
	DirectX::XMStoreFloat(&distance, Distance);

	const DirectX::XMVECTOR CameraRockOnPosition = PlayerToRockOn * 1.0f;

	DirectX::XMVECTOR RockOnEyeVector = CameraRockOnPosition - Target;
	RockOnEyeVector = DirectX::XMVector3Normalize(RockOnEyeVector);
	RockOnEyeVector *= -1;

	DirectX::XMStoreFloat3(&playerToRockOn, PlayerToRockOn);
	DirectX::XMStoreFloat3(&rockOnEyeVector, RockOnEyeVector);
}
