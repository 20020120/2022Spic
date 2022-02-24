#include "BaseCamera.h"
#include "framework.h"

void BaseCamera::UpdateViewProjection()
{
	DirectX::XMFLOAT3 up = { 0, 1, 0 };
	DirectX::XMVECTOR eye_vec = XMLoadFloat3(&eye);
	DirectX::XMVECTOR focus_vec = XMLoadFloat3(&target);
	DirectX::XMMATRIX view_mat = DirectX::XMMatrixLookAtLH(eye_vec, focus_vec, DirectX::XMLoadFloat3(&up)); // V
	DirectX::XMStoreFloat4x4(&view, view_mat);
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

	DirectX::XMMATRIX projection_mat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), aspect_ratio, near_far.x, near_far.y); // P
	DirectX::XMStoreFloat4x4(&projection, projection_mat);
}

void BaseCamera::AttitudeControl(float elapsedTime)
{
	using namespace DirectX;
	DirectX::XMVECTOR Orientation = DirectX::XMLoadFloat4(&orientation);
	DirectX::XMVECTOR Forward, Right, Up;
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

void BaseCamera::DebugGUI()
{
#ifdef USE_IMGUI
	ImGui::Begin("Camera", false);
	ImGui::SliderFloat3("Position", &eye.x, -10.0f, 10.0f);
	ImGui::InputFloat3("target", &target.x);

	ImGui::InputFloat3("forward", &forward.x);

	ImGui::SliderFloat("range", &radius, 3.0f, 15.0f);
	ImGui::End();
#endif
}

