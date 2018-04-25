#include "SecenBase.h"

SecenBase::SecenBase(ID3D11Device* device, CXMMATRIX world):
	Device(device), mVerticeBuffer(nullptr), mIndiceBuffer(nullptr),
	mInputLayout(nullptr)
{
	XMStoreFloat4x4(&mWorld, world);
	VerticeCount = 0;
	IndiceCount = 0;
	SetLights();
}

SecenBase::~SecenBase()
{
	SafeRelease(mInputLayout);
	SafeRelease(mVerticeBuffer);
	SafeRelease(mIndiceBuffer);
}

void SecenBase::SetLights()
{
	//"三点式"照明
	//主光源 为平行光
	mDirLights[0].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);  //环境光
	mDirLights[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  //漫反射光
	mDirLights[0].specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);  //高光
	mDirLights[0].direction = XMFLOAT3(0.707f, -0.707f, 0.0f); //光线的方向
																//侧光源																	
	mDirLights[1].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[1].diffuse = XMFLOAT4(0.30f, 0.30f, 0.30f, 1.0f);
	mDirLights[1].specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].direction = XMFLOAT3(-0.707f, -0.707f, 0.f);
	//背光源
	mDirLights[2].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[2].diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[2].specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	//聚光灯
	mSpotLight.ambient = XMFLOAT4(0.4f, 0.2f, 0.1f, 1.0f);
	mSpotLight.diffuse = XMFLOAT4(0.5f, 0.3f, 0.1f, 1.0f);
	mSpotLight.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mSpotLight.att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mSpotLight.spot = 96.0f;
	mSpotLight.range = 1000.0f;
}

bool SecenBase::BuildBuffers()
{
	return true;
}

void SecenBase::SetEffect(CXMMATRIX vieproj)
{

}

bool SecenBase::BuildSRVs()
{
	return true;
}

bool SecenBase::BuildInputLayouts()
{
	return true;
}

bool SecenBase::Init()
{
	if (!BuildBuffers())
		return false;
	if (!BuildSRVs())
		return false;
	if (!BuildInputLayouts())
		return false;
	return true;
}

void SecenBase::Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj)
{

}
void SecenBase::SetMaterial()
{

}