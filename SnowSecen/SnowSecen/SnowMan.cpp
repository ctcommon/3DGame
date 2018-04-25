#include "SnowMan.h"

SnowMan::SnowMan(ID3D11Device* device, CXMMATRIX world) :
	SecenBase(device, world), mTexSnow(nullptr), mTexWood(nullptr), mTexStone(nullptr),
	mTexNose(nullptr)
{
	SetMaterial();
	SetMatrix();
	MakePartToMan();
}

SnowMan::~SnowMan()
{
	SafeRelease(mTexNose);
	SafeRelease(mTexStone);
	SafeRelease(mTexWood);
	SafeRelease(mTexSnow);
}

void SnowMan::SetMaterial()
{
	mMatBodyAndHead.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mMatBodyAndHead.diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mMatBodyAndHead.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	mMatMouth.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMatMouth.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	mMatMouth.specular = XMFLOAT4(0.001f, 0.001f, 0.001f, 1.0f);

	mMatNose.ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mMatNose.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	mMatNose.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	mMatHand.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMatHand.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMatHand.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	mMatEye.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mMatEye.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	mMatEye.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
}

void SnowMan::SetMatrix()
{
	XMStoreFloat4x4(&texTransbox, XMMatrixIdentity());
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMVECTOR det = XMMatrixDeterminant(world);
	XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det, world));
	XMStoreFloat4x4(&worldInvTrans, worldInvTranspose);
}
void SnowMan::MakePartToMan()
{
	GeometryGenerator::MeshData                        dBody;
	GeometryGenerator::MeshData                        dHead;
	GeometryGenerator::MeshData                        dLeftHandPart;
	GeometryGenerator::MeshData                        dRightHandPart;
	GeometryGenerator::MeshData                        dButton1;
	GeometryGenerator::MeshData                        dButton2;
	GeometryGenerator::MeshData                        dButton3;
	GeometryGenerator::MeshData                        dMouth;
	GeometryGenerator::MeshData                        dNose;
	GeometryGenerator::MeshData                        dLeftEye;
	GeometryGenerator::MeshData                        dRightEye;

	//初始化各个部分的数据
	GeometryGenerator::GetInstance()->CreateSphere(5.f, 30, 30, dBody);
	GeometryGenerator::GetInstance()->CreateSphere(3.5f, 30, 30, dHead);
	GeometryGenerator::GetInstance()->CreateCylinder(0.2f, 1.f, 8.f, 20, 20, dLeftHandPart);
	GeometryGenerator::GetInstance()->CreateCylinder(0.2f, 1.f, 8.f, 20, 20, dRightHandPart);
	GeometryGenerator::GetInstance()->CreateCylinder(0.7f, 0.7f, 0.25f, 20, 20, dButton1);
	GeometryGenerator::GetInstance()->CreateCylinder(0.7f, 0.7f, 0.25f, 20, 20, dButton2);
	GeometryGenerator::GetInstance()->CreateCylinder(0.7f, 0.7f, 0.25f, 20, 20, dButton3);
	GeometryGenerator::GetInstance()->CreateTorus(3.f, 0.3f, 30, dMouth);
	GeometryGenerator::GetInstance()->CreateCylinder(0.175f, 0.4f, 2.5f, 30, 30, dNose);
	GeometryGenerator::GetInstance()->CreateSphere(0.15f, 20, 20, dLeftEye);
	GeometryGenerator::GetInstance()->CreateSphere(0.15f, 20, 20, dRightEye);

	XMStoreFloat4x4(&mBody, XMMatrixTranslation(0.f, 0.f, 0.f));
	XMStoreFloat4x4(&mHead, XMMatrixTranslation(0.f, 8.f, 0.f));
	XMStoreFloat4x4(&mLeftHand, XMMatrixRotationZ(XM_PIDIV4)*XMMatrixTranslation(-5.f, 2.0f, 0.0f));
	XMStoreFloat4x4(&mRightHand, XMMatrixRotationZ(-XM_PIDIV4)*XMMatrixTranslation(5.f, 2.0f, 0.0f));
	XMStoreFloat4x4(&mButton1, XMMatrixRotationX(-XM_PIDIV2)* XMMatrixTranslation(0.f, 0.f, -5.f));
	XMStoreFloat4x4(&mButton2, XMMatrixRotationX(-XM_PIDIV4)*XMMatrixTranslation(0.f, 3.5f, -3.5f));
	XMStoreFloat4x4(&mButton3, XMMatrixRotationX(XM_PIDIV4)*XMMatrixTranslation(0.f, -3.5f, -3.5f));
	XMStoreFloat4x4(&mMouth, XMMatrixRotationX(-XM_PIDIV4)*XMMatrixTranslation(0.f, 8.5f, 0.0f)*XMMatrixTranslation(0.f, -1.8f, -1.8f));
	XMStoreFloat4x4(&mNose, XMMatrixRotationX(-XM_PIDIV2)*XMMatrixTranslation(0.f, 7.f, -3.5f));
	XMStoreFloat4x4(&mLeftEye, XMMatrixTranslation(1.f, 8.5f, -3.5f));
	XMStoreFloat4x4(&mRightEye, XMMatrixTranslation(-1.f, 8.5f, -3.5f));

	mBodyVStart = 0;
	mBodyIStart = 0;
	mHeadVStart = mBodyVStart + static_cast<UINT>(dBody.vertices.size());
	mHeadIStart = mBodyIStart + static_cast<UINT>(dBody.indices.size());
	mLeftHandVStart = mHeadVStart + static_cast<UINT>(dHead.vertices.size());
	mLeftHandIStart = mHeadIStart + static_cast<UINT>(dHead.indices.size());
	mRightHandVStart = mLeftHandVStart + static_cast<UINT>(dLeftHandPart.vertices.size());
	mRightHandIStart = mLeftHandIStart + static_cast<UINT>(dLeftHandPart.indices.size());
	mButton1VStart = mRightHandVStart + static_cast<UINT>(dRightHandPart.vertices.size());
	mButton1IStart = mRightHandIStart + static_cast<UINT>(dRightHandPart.indices.size());
	mButton2VStart = mButton1VStart + static_cast<UINT>(dButton1.vertices.size());
	mButton2IStart = mButton1IStart + static_cast<UINT>(dButton1.indices.size());
	mButton3VStart = mButton2VStart + static_cast<UINT>(dButton2.vertices.size());
	mButton3IStart = mButton2IStart + static_cast<UINT>(dButton2.indices.size());
	mMouthVStart = mButton3VStart + static_cast<UINT>(dButton3.vertices.size());
	mMouthIStart = mButton3IStart + static_cast<UINT>(dButton3.indices.size());
	mNoseVStart = mMouthVStart + static_cast<UINT>(dMouth.vertices.size());
	mNoseIStart = mMouthIStart + static_cast<UINT>(dMouth.indices.size());
	mLeftEyeVStart = mNoseVStart + static_cast<UINT>(dNose.vertices.size());
	mLeftEyeIStart = mNoseIStart + static_cast<UINT>(dNose.indices.size());
	mRightEyeVStart = mLeftEyeVStart + static_cast<UINT>(dLeftEye.vertices.size());
	mRightEyeIStart = mLeftEyeIStart + static_cast<UINT>(dLeftEye.indices.size());

	VerticeCount = mRightEyeVStart + static_cast<UINT>(dRightEye.vertices.size());
	IndiceCount = mRightEyeIStart + static_cast<UINT>(dRightEye.indices.size());

	Snowman.vertices.resize(VerticeCount);
	Snowman.indices.resize(IndiceCount);
	AppendPart(Snowman, dBody, mBodyVStart, mBodyIStart);
	AppendPart(Snowman, dHead, mHeadVStart, mHeadIStart);
	AppendPart(Snowman, dLeftHandPart, mLeftHandVStart, mLeftHandIStart);
	AppendPart(Snowman, dRightHandPart, mRightHandVStart, mRightHandIStart);
	AppendPart(Snowman, dButton1, mButton1VStart, mButton1IStart);
	AppendPart(Snowman, dButton2, mButton2VStart, mButton2IStart);
	AppendPart(Snowman, dButton3, mButton3VStart, mButton3IStart);
	AppendPart(Snowman, dMouth, mMouthVStart, mMouthIStart);
	AppendPart(Snowman, dNose, mNoseVStart, mNoseIStart);
	AppendPart(Snowman, dLeftEye, mLeftEyeVStart, mLeftEyeIStart);
	AppendPart(Snowman, dRightEye, mRightEyeVStart, mRightEyeIStart);
}

bool SnowMan::BuildBuffers()
{
	std::vector<Vertex::NormalTexVertice> vertices(VerticeCount);
	std::vector<UINT> indices(IndiceCount);
	for (UINT i = 0; i < VerticeCount; i++) {
		vertices[i].pos = Snowman.vertices[i].pos;
		vertices[i].normal = Snowman.vertices[i].normal;
		vertices[i].tex = Snowman.vertices[i].tex;
	}

	indices.assign(Snowman.indices.begin(), Snowman.indices.end());

	return BuildBuffers_(vertices, indices);
}

bool SnowMan::BuildSRVs()
{
	HR(CreateDDSTextureFromFile(Device, L"Textures/snow2.dds", nullptr, &mTexSnow), L"CreateDDSTextureFromFile");
	HR(CreateDDSTextureFromFile(Device, L"Textures/wood.dds", nullptr, &mTexWood), L"CreateDDSTextureFromFile");
	HR(CreateDDSTextureFromFile(Device, L"Textures/stone.dds", nullptr, &mTexStone), L"CreateDDSTextureFromFile");
	HR(CreateDDSTextureFromFile(Device, L"Textures/nose.dds", nullptr, &mTexNose), L"CreateDDSTextureFromFile");
	return true;
}

bool SnowMan::BuildInputLayouts()
{
	UINT numLayoutElements = ARRAYSIZE(InputLayoutDesc::NormalTex);
	return BuildInputLayouts_(InputLayoutDesc::NormalTex, Effects::pSnowManEffect->LightTexTech, numLayoutElements);
}

void SnowMan::Update(const XMFLOAT3& eyepos,Camera* camera,const XMFLOAT3& old_pos)
{
	Effects::pSnowManEffect->EyePosW->SetRawValue(&eyepos, 0, sizeof(XMFLOAT3));
	XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(&mWorld));
	XMVECTOR camPosV = XMVector3TransformCoord(camera->GetPosotionXM(), XMMatrixInverse(&det, XMLoadFloat4x4(&mWorld)));
	XMFLOAT4 camPosToSnowMan;
	XMStoreFloat4(&camPosToSnowMan, camPosV);

	//限制相机相对于雪人的距离,雪人身体半径为5
	float distance = 5.f;

	if ((abs(camPosToSnowMan.x) < 5 + distance && abs(camPosToSnowMan.z) < 5 + distance))
	{
		camera->SetPosition(old_pos.x, old_pos.y, old_pos.z);
	}
}


void  SnowMan::Update(const XMFLOAT3& eyepos)
{
	Effects::pSnowManEffect->EyePosW->SetRawValue(&eyepos, 0, sizeof(XMFLOAT3));
}
void SnowMan::SetEffect(CXMMATRIX vieproj)
{
	XMFLOAT4X4 worldViewProjbox;
	XMStoreFloat4x4(&worldViewProjbox, XMLoadFloat4x4(&mWorld)*vieproj);
	Effects::pSnowManEffect->World->SetMatrix(reinterpret_cast<const float*>(&mWorld));
	Effects::pSnowManEffect->WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProjbox));
	Effects::pSnowManEffect->WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&worldInvTrans));
	Effects::pSnowManEffect->DirLight->SetRawValue(&mDirLights[0],0,sizeof(mDirLights[0]));
	Effects::pSnowManEffect->FogColor->SetFloatVector(reinterpret_cast<const float*>(&Colors::Silver));
	Effects::pSnowManEffect->FogStart->SetFloat(200.0f);
	Effects::pSnowManEffect->FogRange->SetFloat(500.0f);
	//SetMat(Effects::pSnowManEffect->Material, materialBox);
	Effects::pSnowManEffect->TexTrans->SetMatrix(reinterpret_cast<const float*>(&texTransbox));
	//SetTex(Effects::pBoxEffect->pFxSR, pSRVBox);
}

void SnowMan::DrawPart(ID3DX11EffectTechnique* tech, ID3D11DeviceContext* dc,const Lights::Material& mat, ID3D11ShaderResourceView* tex, CXMMATRIX world, CXMMATRIX viewproj,
	UINT inum,UINT istart, UINT vstart)
{
	Effects::pSnowManEffect->Material->SetRawValue(&mat, 0, sizeof(mat));
	Effects::pSnowManEffect->Tex->SetResource(tex);
	XMStoreFloat4x4(&mWorld, world);
	SetMatrix();
	Render(tech, dc, viewproj, inum, istart, vstart);
}
void SnowMan::Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj)
{
	dc->IASetInputLayout(mInputLayout);
	UINT stride = sizeof(Vertex::NormalTexVertice);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mVerticeBuffer, &stride, &offset);
	dc->IASetIndexBuffer(mIndiceBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3DX11EffectTechnique* tech = Effects::pSnowManEffect->LightTexTech;
	XMFLOAT4X4 oldworld = mWorld;

	/*SetMat(Effects::pSnowManEffect->Material, mMatBodyAndHead);//重构代码，为了提高复用
	SetTex(Effects::pSnowManEffect->Tex, mTexSnow);
	//设置身体的世界矩阵
	XMStoreFloat4x4(&mWorld, XMLoadFloat4x4(&mBody)*XMLoadFloat4x4(&oldworld));
	SetMatrix();
	Render(tech, dc, viewproj, mHeadIStart - mBodyIStart, mBodyIStart, mBodyVStart);*/
	//绘制身体
	DrawPart(tech, dc, mMatBodyAndHead, mTexSnow, XMLoadFloat4x4(&mBody)*XMLoadFloat4x4(&oldworld), viewproj,
		mHeadIStart - mBodyIStart, mBodyIStart, mBodyVStart);
	//绘制头部
	DrawPart(tech, dc, mMatBodyAndHead, mTexSnow, XMLoadFloat4x4(&mHead)*XMLoadFloat4x4(&oldworld), viewproj,
		mLeftHandIStart - mHeadIStart, mHeadIStart, mHeadVStart);
	//绘制左手
	DrawPart(tech, dc, mMatHand, mTexWood, XMLoadFloat4x4(&mLeftHand)*XMLoadFloat4x4(&oldworld), viewproj,
		mRightHandIStart - mLeftHandIStart, mLeftHandIStart, mLeftHandVStart);
	//绘制右手
	DrawPart(tech, dc, mMatHand, mTexWood, XMLoadFloat4x4(&mRightHand)*XMLoadFloat4x4(&oldworld), viewproj,
		mButton1IStart - mRightHandIStart, mRightHandIStart, mRightHandVStart);
	//绘制纽扣
	DrawPart(tech, dc, mMatButton, mTexStone, XMLoadFloat4x4(&mButton1)*XMLoadFloat4x4(&oldworld), viewproj,
		mButton2IStart - mButton1IStart, mButton1IStart, mButton1VStart);
	DrawPart(tech, dc, mMatButton, mTexStone, XMLoadFloat4x4(&mButton2)*XMLoadFloat4x4(&oldworld), viewproj,
		mButton3IStart - mButton2IStart, mButton2IStart, mButton2VStart);
	DrawPart(tech, dc, mMatButton, mTexStone, XMLoadFloat4x4(&mButton3)*XMLoadFloat4x4(&oldworld), viewproj,
		mMouthIStart - mButton3IStart, mButton3IStart, mButton3IStart);
	//绘制嘴巴
	DrawPart(tech, dc, mMatMouth, mTexStone, XMLoadFloat4x4(&mMouth)*XMLoadFloat4x4(&oldworld), viewproj,
		mNoseIStart - mMouthIStart, mMouthIStart, mMouthVStart);
	//绘制鼻子
	DrawPart(tech, dc, mMatNose, mTexNose, XMLoadFloat4x4(&mNose)*XMLoadFloat4x4(&oldworld), viewproj,
		mLeftEyeIStart - mNoseIStart, mNoseIStart, mNoseVStart);
	//绘制左眼
	DrawPart(tech, dc, mMatEye, mTexStone, XMLoadFloat4x4(&mLeftEye)*XMLoadFloat4x4(&oldworld), viewproj,
		mRightEyeIStart - mLeftEyeIStart, mLeftEyeIStart, mLeftEyeVStart);
	//绘制右眼
	DrawPart(tech, dc, mMatEye, mTexStone, XMLoadFloat4x4(&mRightEye)*XMLoadFloat4x4(&oldworld), viewproj,
		IndiceCount - mRightEyeIStart, mRightEyeIStart, mRightEyeVStart);
	mWorld = oldworld;
}