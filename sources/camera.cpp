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
	//�ω���������΃X���[
	if (horizonDegree < 0.1f && horizonDegree >-0.1f
		&& verticalDegree < 0.1f && verticalDegree > -0.1f)return;

	DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);
	DirectX::XMFLOAT3 up = { 0,1,0 };
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

	DirectX::XMVECTOR Right = DirectX::XMVector3Cross(Up, DirectX::XMVectorScale(EyeVector, -1));

	//�v���C���[�ɑ΂��ďc�����̉�]
	//�v���C���[�̐^��Ɛ^���ɋ߂��Ƃ��͉�]���Ȃ�
	if (verticalDegree > 0.1f || verticalDegree < -0.1f)
	{
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(PlayerUp), DirectX::XMVector3Normalize(EyeVector));
		float dot{};
		DirectX::XMStoreFloat(&dot, Dot);
		//�^��ɋ߂���
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
		//�^���ɋ߂���
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
		//�ʏ펞
		else
		{
			DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(Right);
			DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(verticalDegree));
			EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
			verticalDegree = 0;
		}
	}
	//�v���C���[�ɑ΂��ĉ��̉�]
	if (horizonDegree > 0.1f || horizonDegree < -0.1f)
	{
		DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(PlayerUp);
		DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(horizonDegree));
		EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
		horizonDegree = 0;
	}


	//�Ō�Ɉ�񂾂��s��
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
		//���E�̂���C��
		DirectX::XMVECTOR EyeVector = DirectX::XMLoadFloat3(&eyeVector);

		const DirectX::XMVECTOR PlayerToRockOn = DirectX::XMLoadFloat3(&playerToRockOn);
		DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(PlayerUp, PlayerToRockOn);
		Cross = DirectX::XMVector3Normalize(Cross);
		const DirectX::XMVECTOR ProjectionLength = DirectX::XMVector3Dot(Cross, EyeVector);
		float projectionLength{};
		DirectX::XMStoreFloat(&projectionLength, ProjectionLength);

		EyeVector = DirectX::XMVectorSubtract(EyeVector, DirectX::XMVectorScale(Cross, projectionLength));
		EyeVector = DirectX::XMVector3Normalize(EyeVector);

		//�ω���������΃X���[
		if (horizonDegree < 0.1f && horizonDegree >-0.1f
			&& verticalDegree < 0.1f && verticalDegree > -0.1f){}
		else
		{
			DirectX::XMFLOAT3 up = { 0,1,0 };
			DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

			DirectX::XMVECTOR Right = DirectX::XMVector3Cross(Up, DirectX::XMVectorScale(EyeVector, -1));

			//�v���C���[�ɑ΂��ďc�����̉�]
			//�v���C���[�̐^��Ɛ^���ɋ߂��Ƃ��͉�]���Ȃ�
			if (verticalDegree > 0.1f || verticalDegree < -0.1f)
			{
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(PlayerUp), DirectX::XMVector3Normalize(EyeVector));
				float dot{};
				DirectX::XMStoreFloat(&dot, Dot);
				//�^��ɋ߂���
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
				//�^���ɋ߂���
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
				//�ʏ펞
				else
				{
					DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(Right);
					DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, DirectX::XMConvertToRadians(verticalDegree));
					EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
					verticalDegree = 0;
				}
			}
		}
		//�Ō�Ɉ�񂾂��s��
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
