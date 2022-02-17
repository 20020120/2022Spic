#include "framework.h"
#include "camera.h"
#include "user.h"

DirectX::XMFLOAT4X4 Camera::keep_view = {};
DirectX::XMFLOAT4X4 Camera::keep_projection = {};

Camera::Camera(GraphicsPipeline& graphics,Player* player)
{
	HRESULT hr{ S_OK };
	//----�萔�o�b�t�@----//
	// �J�����֘A
	scene_constants = std::make_unique<Constants<SceneConstants>>(graphics.get_device().Get());
	// orientation�̏�����
	{
		DirectX::XMFLOAT3 n(0, 1, 0); // ���i���K���j
		constexpr float angle = DirectX::XMConvertToRadians(0); //�p�x�i���W�A���j
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
	// �}�E�X����ŃJ��������
	mouse_operation_with_euler_angles(elapsed_time);
	// �L�[���͂ɂ���ĉ�]
#if 1
	keyboard_operation_with_euler_angles(elapsed_time);
#endif // 0
	// X���̃J������]�𐧌�
	angle.x = Math::clamp(angle.x, min_angle_x, max_angle_x);
	// Y���̉�]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
	if (angle.y < -DirectX::XM_PI) { angle.y += DirectX::XM_2PI; }
	if (angle.y > DirectX::XM_PI) { angle.y -= DirectX::XM_2PI; }
	// range�͈̔͐���
	range = Math::clamp(range, 2.0f, 95.0f);
	// �J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// ��]�s�񂩂�O�����x�N�g�������o��
	DirectX::XMVECTOR forward_vec = transform.r[2];
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, forward_vec);
	// �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	eye.x = target.x - forward.x * range;
	eye.y = target.y - forward.y * range;
	eye.z = target.z - forward.z * range;
	// �J�����̎��_�ƒ��ӓ_��ݒ�
	DirectX::XMFLOAT3 up(0, 1, 0);
	XMVECTOR eye_vec   = XMLoadFloat3(&eye);
	XMVECTOR focus_vec = XMLoadFloat3(&target);
	XMVECTOR up_vec    = XMLoadFloat3(&up);
	XMMATRIX view_mat  = XMMatrixLookAtLH(eye_vec, focus_vec, up_vec); // V
	XMStoreFloat4x4(&view, view_mat);
	// �v���W�F�N�V�����s����쐬
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
	// �}�E�X����ŃJ��������
	mouse_operation_with_quaternion(elapsed_time);
	// �L�[���͂ɂ���ĉ�]
#if 1
	keyboard_operation_with_quaternion(elapsed_time);
#endif // 0
	// range�͈̔͐���
	range = Math::clamp(range, 0.5f, 95.0f);
	// �J������]�l����]�s��ɕϊ�
	// XMVECTOR�N���X�֕ϊ�
	XMVECTOR orientation_vec = XMLoadFloat4(&orientation);
	//��]�s�񂩂�e�������o��
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
	// ��]�̐���
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
	// �㉺
	{
		DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(right_vec, DirectX::XMConvertToRadians(vertical_rotation_degree));
		orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
		vertical_rotation_degree = 0;
	}
	// ���E
	{
		DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(up_vec, DirectX::XMConvertToRadians(horizon_rotation_degree));
		orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
		horizon_rotation_degree = 0;
	}
	// orientation�X�V
	XMStoreFloat4(&orientation, orientation_vec);
	// �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
	eye.x = target.x - forward.x * range;
	eye.y = target.y - forward.y * range;
	eye.z = target.z - forward.z * range;
	// �J�����̎��_�ƒ��ӓ_��ݒ�
	XMVECTOR eye_vec = XMLoadFloat3(&eye);
	XMVECTOR focus_vec = XMLoadFloat3(&target);
	XMMATRIX view_mat = XMMatrixLookAtLH(eye_vec, focus_vec, XMLoadFloat3(&up)); // V
	XMStoreFloat4x4(&view, view_mat);
	//�r���[�s����t�s�񂩂����[���h�s��ɖ߂�
	DirectX::XMMATRIX world_vec = DirectX::XMMatrixInverse(nullptr, view_mat);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, world_vec);
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	this->forward.x = world._31;
	this->forward.y = world._32;
	this->forward.z = world._33;

	// �v���W�F�N�V�����s����쐬
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
	// �r���[�E�v���W�F�N�V�����ϊ��s����v�Z���A�����萔�o�b�t�@�ɃZ�b�g����
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
	// �r���[�s��/�v���W�F�N�V�����s����쐬
	XMMATRIX V = XMLoadFloat4x4(&view);
	XMMATRIX P = XMLoadFloat4x4(&projection);
	keep_view = view;
	keep_projection = projection;
	// �萔�o�b�t�@�Ƀt�F�b�`����
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
		// �}�E�X�z�C�[���ŃJ�������A����
		{
			if (mouse->up_tilt()) {
				/*���ɉ�]�i�`���g�j���� */ range -= 1000.0f * elapsed_time;
			}
			else if (mouse->down_tilt()) {
				/*���ɉ�]�i�`���g�j����*/ range += 1000.0f * elapsed_time;
			}
		}
		// ���N���b�N�����Ȃ���}�E�X�ړ��ŕ��s�ړ�
		{
			if (mouse->common_button() & Mouse::BTN_LEFT_CLICK) {
				if (mouse->get_cursor_pos().x < mouse->get_old_cursor_pos().x - CURSOL_PLAY) { target.x -= 20.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().x > mouse->get_old_cursor_pos().x + CURSOL_PLAY) { target.x += 20.0f * elapsed_time; }
				if (mouse->get_cursor_pos().y < mouse->get_old_cursor_pos().y - CURSOL_PLAY) { target.y += 20.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().y > mouse->get_old_cursor_pos().y + CURSOL_PLAY) { target.y -= 20.0f * elapsed_time; }
			}
		}
		// �E�N���b�N�����Ȃ���}�E�X�ړ��ŉ�]
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
	// �J�����̉�]���x
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
	// XMVECTOR�N���X�֕ϊ�
	XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&orientation);
	//��]�s�񂩂�e�������o��
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
		// �}�E�X�z�C�[���ŃJ�������A����
		{
			if (mouse->up_tilt()) {
				/*���ɉ�]�i�`���g�j���� */ range -= 1000.0f * elapsed_time;
			}
			else if (mouse->down_tilt()) {
				/*���ɉ�]�i�`���g�j����*/ range += 1000.0f * elapsed_time;
			}
		}
		// ���N���b�N�����Ȃ���}�E�X�ړ��ŕ��s�ړ�
		{
			if (mouse->common_button() & Mouse::BTN_LEFT_CLICK) {
				if (mouse->get_cursor_pos().x < mouse->get_old_cursor_pos().x - CURSOL_PLAY) { target.x -= 20.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().x > mouse->get_old_cursor_pos().x + CURSOL_PLAY) { target.x += 20.0f * elapsed_time; }
				if (mouse->get_cursor_pos().y < mouse->get_old_cursor_pos().y - CURSOL_PLAY) { target.y += 20.0f * elapsed_time; }
				else if (mouse->get_cursor_pos().y > mouse->get_old_cursor_pos().y + CURSOL_PLAY) { target.y -= 20.0f * elapsed_time; }
			}
		}
		// �E�N���b�N�����Ȃ���}�E�X�ړ��ŉ�]
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
	// orientation_vec����orientation���X�V
	XMStoreFloat4(&orientation, orientation_vec);
}

void Camera::keyboard_operation_with_quaternion(float elapsed_time)
{
	using namespace DirectX;
	// XMVECTOR�N���X�֕ϊ�
	XMVECTOR orientation_vec = DirectX::XMLoadFloat4(&orientation);
	//��]�s�񂩂�e�������o��
	XMVECTOR forward_vec;
	XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
	XMFLOAT4X4 m4x4 = {};
	XMStoreFloat4x4(&m4x4, m);
	forward_vec = { m4x4._31, m4x4._32, m4x4._33 };


	// I K �őO��ړ�
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
	// W A S D �ŉ�]
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
	//�r���[�s����t�s�񂩂����[���h�s��ɖ߂�
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