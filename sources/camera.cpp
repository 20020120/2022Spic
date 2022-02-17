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
	const DirectX::XMVECTOR Orientation = DirectX::XMLoadFloat4(&orientation);

	DirectX::XMVECTOR Forward, Right, Up;
	const DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(Orientation);
	DirectX::XMFLOAT4X4 m4x4 = {};
	DirectX::XMStoreFloat4x4(&m4x4, m);

	Right = { m4x4._11, m4x4._12, m4x4._13 };
	Up = { 0, 1, 0 };
	Forward = { m4x4._31, m4x4._32, m4x4._33 };

	DirectX::XMFLOAT3 playerForward = player->GetForward();
	DirectX::XMFLOAT3 playerUp = player->GetUp();
	DirectX::XMFLOAT3 playerRight = player->GetRight();
	DirectX::XMFLOAT3 playerPosition = player->GetPosition();

	DirectX::XMVECTOR PlayerForward = DirectX::XMLoadFloat3(&playerForward);
	DirectX::XMVECTOR PlayerUp = DirectX::XMLoadFloat3(&playerUp);
	DirectX::XMVECTOR PlayerRight = DirectX::XMLoadFloat3(&playerRight);
	DirectX::XMVECTOR PlayerPosition = DirectX::XMLoadFloat3(&playerPosition);

	DirectX::XMVECTOR Eye = PlayerPosition + PlayerForward * -5;
	DirectX::XMStoreFloat3(&eye, Eye);

	DirectX::XMVECTOR Target = PlayerPosition + Forward * 5;
	DirectX::XMStoreFloat3(&target, Target);

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
	const DirectX::XMVECTOR Orientation = DirectX::XMLoadFloat4(&orientation);

	DirectX::XMVECTOR Forward, Right, Up;
	const DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(Orientation);
	DirectX::XMFLOAT4X4 m4x4 = {};
	DirectX::XMStoreFloat4x4(&m4x4, m);

	Right = { m4x4._11, m4x4._12, m4x4._13 };
	Up = { 0, 1, 0 };
	Forward = { m4x4._31, m4x4._32, m4x4._33 };

	DirectX::XMFLOAT3 playerForward = player->GetForward();
	DirectX::XMFLOAT3 playerUp = player->GetUp();
	DirectX::XMFLOAT3 playerRight = player->GetRight();
	DirectX::XMFLOAT3 playerPosition = player->GetPosition();
	DirectX::XMFLOAT3 playerVelocity = player->GetVelocity();

	DirectX::XMVECTOR PlayerForward = DirectX::XMLoadFloat3(&playerForward);
	DirectX::XMVECTOR PlayerUp = DirectX::XMLoadFloat3(&playerUp);
	DirectX::XMVECTOR PlayerRight = DirectX::XMLoadFloat3(&playerRight);
	DirectX::XMVECTOR PlayerPosition = DirectX::XMLoadFloat3(&playerPosition);
	DirectX::XMVECTOR PlayerVelocity = DirectX::XMLoadFloat3(&playerVelocity);

	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	Eye += PlayerVelocity * elapsedTime;
	DirectX::XMStoreFloat3(&eye, Eye);

	if (player->GetCameraReset())
	{
		DirectX::XMVECTOR Target = PlayerPosition + PlayerForward * 5;
		DirectX::XMStoreFloat3(&target, Target);
		player->FalseCameraReset();
	}

	ImGui::Begin("Camera", false);
	ImGui::InputFloat3("Position", &eye.x);
	ImGui::InputFloat3("target", &target.x);
	ImGui::End();

	XMFLOAT3 up{ 0,1,0 };
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

	float width = static_cast<float>(SCREEN_WIDTH);
	float height = static_cast<float>(SCREEN_HEIGHT);
	float aspect_ratio{ width / height };

	XMMATRIX projection_mat = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), aspect_ratio, near_far.x, near_far.y); // P
	XMStoreFloat4x4(&projection, projection_mat);

}