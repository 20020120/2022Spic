#include "camera.h"


void ClearCamera::Initialize(GraphicsPipeline& graphics)
{
    eye = keep_eye;
    target = keep_target;
	using namespace DirectX;
	const DirectX::XMFLOAT3 playerPosition = player->GetPosition();
	const DirectX::XMFLOAT3 playerForward = player->GetForward();
	const DirectX::XMFLOAT3 playerUp = player->GetUp();

	const DirectX::XMVECTOR PlayerPosition = DirectX::XMLoadFloat3(&playerPosition);
	const DirectX::XMVECTOR PlayerForward = DirectX::XMLoadFloat3(&playerForward);
	const DirectX::XMVECTOR PlayerUp = DirectX::XMLoadFloat3(&playerUp);

	const DirectX::XMVECTOR CameraPosition = DirectX::XMLoadFloat3(&eye);

	const DirectX::XMVECTOR EyeCenter = PlayerPosition + PlayerUp * 6;
	DirectX::XMStoreFloat3(&eyeCenter, EyeCenter);

	DirectX::XMVECTOR EyeVector = EyeCenter - CameraPosition;
	EyeVector = DirectX::XMVector3Normalize(EyeVector);
	DirectX::XMStoreFloat3(&eyeVector, EyeVector);

    UpdateViewProjection();
}

void ClearCamera::Update(float elapsedTime)
{
	if(cameraReset)
	{
		const DirectX::XMFLOAT3 playerForward = player->GetForward();
		const DirectX::XMFLOAT3 playerUp = player->GetUp();

		const DirectX::XMVECTOR PlayerForward = DirectX::XMLoadFloat3(&playerForward);
		const DirectX::XMVECTOR PlayerUp = DirectX::XMLoadFloat3(&playerUp);

		if(CameraReset(elapsedTime, PlayerForward, PlayerUp))
		{
			cameraReset = false;
		}
	}

	AttitudeControl(elapsedTime);
	DebugGUI();
	UpdateViewProjection();
}

bool ClearCamera::CameraReset(float elapsedTime, DirectX::XMVECTOR PlayerForward, DirectX::XMVECTOR PlayerUp)
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

		if (dot < 0.98f)
		{
			const DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, angle * elapsedTime / 0.1f);
			EyeVector = DirectX::XMVector3Rotate(EyeVector, Quaternion);
		}
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
