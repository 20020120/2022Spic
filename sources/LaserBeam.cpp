#include"LaserBeam.h"
#include"user.h"
#include"Operators.h"
void LaserBeam::fInitialize(ID3D11Device* pDevice_, const wchar_t* TextureName_)
{
    // 初期化
    mpSkinnedMesh = std::make_unique<SkinnedMesh>(pDevice_, "./resources/Models/Laser/LaserBeam.fbx");

}

void LaserBeam::fUpdate()
{
	fCalcTransform();
}

void LaserBeam::fRender(GraphicsPipeline& Graphics_)
{
	Graphics_.set_pipeline_preset(SHADER_TYPES::LaserBeam);
	DirectX::XMFLOAT4X4 worldMat= Math::calc_world_matrix(mScale, mOrientation, mStartPoint);
	mpSkinnedMesh->render(Graphics_.get_dc().Get(), worldMat, { 1.0f,1.0f,1.0f,1.0 });
}

void LaserBeam::fSetPosition(DirectX::XMFLOAT3 Start_, DirectX::XMFLOAT3 End_)
{
    mStartPoint = Start_;
    mEndPoint = End_;
}

void LaserBeam::fSetRadius(float Radius_)
{
    mRadius = Radius_;
}


void LaserBeam::fCalcTransform()
{
	using namespace DirectX;
    // スケールを算出
    mScale.x = mScale.z = mRadius;
	mScale.y = Math::Length(mStartPoint- mEndPoint);

    // 回転を算出
	XMFLOAT3 n(0, 1, 0); // 軸（正規化）
	XMFLOAT4 orientation = {
		sinf(XMConvertToRadians(0) / 2) * n.x,
		sinf(XMConvertToRadians(0) / 2) * n.y,
		sinf(XMConvertToRadians(0) / 2) * n.z,
		cosf(XMConvertToRadians(0) / 2)
	};
	// XMVECTORクラスへ変換
	XMVECTOR orientation_vec = XMLoadFloat4(&orientation);
	XMFLOAT3 d = {
		mEndPoint.x - mStartPoint.x,
		mEndPoint.y - mStartPoint.y,
		mEndPoint.z - mStartPoint.z };
	XMVECTOR d_norm = XMVector3Normalize(XMLoadFloat3(&d));
	XMVECTOR d_length_vec = XMVector3Length(XMLoadFloat3(&d));
	float d_length;
	XMStoreFloat(&d_length, d_length_vec);

	XMFLOAT3 d2 = { 0, mEndPoint.y - mStartPoint.y + FLT_EPSILON, 0 };
	XMVECTOR d2_norm = XMVector3Normalize(XMLoadFloat3(&d2));
	XMVECTOR dot = XMVector3Dot(d2_norm, d_norm);
	float angle;
	XMStoreFloat(&angle, dot);
	angle = acosf(angle);
	XMVECTOR axis = XMVector3Cross(d2_norm, d_norm);
	if (fabsf(angle) > 1e-8f)
	{
		XMVECTOR q = XMQuaternionRotationAxis(axis, angle);
		orientation_vec = XMQuaternionMultiply(orientation_vec, q);
	}

	DirectX::XMStoreFloat4(&mOrientation, orientation_vec);
}