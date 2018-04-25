#include "SecenFactory.h"
bool SecenFactory::Init(ID3D11Device* device,float aspectratio)
{
	if (!Effects::InitAll(device))
		return false;
	//��ʼ�����
	mCamera = new Camera();
	//���ǰ��������ʼλ������[0,f,5.f,-10.f]��Ĭ�ϳ���Ϊz��������
	mCamera->SetPosition(0.f, 10.f, 0.f);
	//���ú�ͶӰ��ز���
	mCamera->SetLens(XM_PIDIV4, aspectratio, 0.5f, 1000.f);
	//��ʼ������
	Terrain::InitInfo tii;
	tii.HeightmapFilename = L"Textures//terrain.raw";
	tii.LayerMapArrayFilename = L"Textures//TerrainArray.dds";
	tii.BlendMapFilename = L"Textures//blend.dds";
	tii.HeightScale = 20.f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 1.f;

	XMMATRIX TerrainWorld = XMMatrixIdentity();
	mTerrain = new Terrain(device, TerrainWorld, tii);
	mTerrain->Init();

	//��ʼ����պ�
	XMMATRIX SkyWorld = XMMatrixIdentity();
	mSky = new Sky(device, SkyWorld, 1000.f, 30, 30);
	mSky->Init();

	//��ʼ��������
	XMMATRIX BoxWorld = XMMatrixTranslation(0.f, 0.f, 200.f);
	mBox = new Box(device, BoxWorld, 10, 20, 20);
	mBox->Init();

	//��ʼ���������
	XMMATRIX TreeWorld = XMMatrixTranslation(0.f, 0.f, 0.f);
	mTree2D = new Tree2D(device, TreeWorld, mTerrain);
	mTree2D->Init();

	//��ʼ����һ��ѩ��
	XMMATRIX SnowmanWorld = XMMatrixTranslation(0.f, mTerrain->getHeight(0.f, 150.f) + 5.f, 150.f);
	mSnowMan = new SnowMan(device, SnowmanWorld);
	mSnowMan->Init();

	//��ʼ���ڶ���ѩ��
	SnowmanWorld = XMMatrixTranslation(0.f, mTerrain->getHeight(0.f, 200.f) + mBox->GetHeight() + 5.f, 200.f);
	mSnowMan2 = new SnowMan(device, SnowmanWorld);
	mSnowMan2->Init();

	//��ʼ��ѩ������
	XMMATRIX SnowparticleWorld = XMMatrixIdentity();
	mSnowParticle = new SnowParticle(device, SnowparticleWorld, 60000);
	mSnowParticle->Init();
	return true;
}


void SecenFactory::Update(float dt, const GameTimer& mtimer)
{
	//ǰ����������
	XMFLOAT3 old_pos = mCamera->GetPosition();

	if (GetAsyncKeyState('A') & 0x8000)
	{
		mCamera->Strafe(-25.f*dt);
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		mCamera->Strafe(25.f*dt);
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		mCamera->Walk(25.f*dt);
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		mCamera->Walk(-25.f*dt);
	}
	XMFLOAT3 new_pos = mCamera->GetPosition();
	float new_y = 10.f + mTerrain->getHeight(new_pos.x, new_pos.z);
	new_pos.y = new_y;
	XMVECTOR POSW = XMLoadFloat3(&new_pos);

	//�������߷��򣬲���λ���� Ȼ������������������߷���������֡���ʱ��dt�����˶೤����
	XMVECTOR tangent = XMLoadFloat3(&new_pos) - XMLoadFloat3(&old_pos);
	tangent = XMVector3Normalize(tangent);
	POSW += tangent * 25.f*dt;

	//��������֮���ٽ���һ�¸߶�
	XMStoreFloat3(&new_pos, POSW);
	new_y = 10.f + mTerrain->getHeight(new_pos.x, new_pos.z);
	new_pos = XMFLOAT3(new_pos.x, new_y, new_pos.z);

	//��ֹ������ε���Խ��
	if (new_pos.x <= -0.5f*mTerrain->width() + 5)
		new_pos.x = -0.5f*mTerrain->width() + 5;
	if (new_pos.x >= 0.5f*mTerrain->width() - 5)
		new_pos.x = 0.5f*mTerrain->width() - 5;
	if (new_pos.z <= -0.5f*mTerrain->depth() + 5)
		new_pos.z = -0.5f*mTerrain->depth() + 5;
	if (new_pos.z >= 0.5f*mTerrain->depth() - 5)
		new_pos.z = 0.5f*mTerrain->depth() - 5;

	POSW = XMLoadFloat3(&new_pos);
	mCamera->SetPosition(POSW);

	//���������ӽ�ͶӰ����
	XMMATRIX viewProj = mCamera->GetViewProj();
	XMFLOAT3 eye = mCamera->GetPosition();

	//���õ��ε��ӽ�
	mTerrain->Update(eye);

	//������պе��ӽ� 
	mSky->Update(viewProj, eye);
	/*XMMATRIX T = XMMatrixTranslation(eye.x, eye.y, eye.z); //����ʵ���������Update������
	XMMATRIX WVP = XMMatrixMultiply(T, mCamera->GetViewProj());
	Effects::pSkyEffect->WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&WVP));*/

	//�������ӵ��ӽ�
	mBox->Update(dt / 3, mTerrain, mCamera, eye);  //���Ե�ʱ�䣬���ڵ��Լ򵥵��ϳ����

	//���ù���������ӽ�
	mTree2D->Update(eye, mCamera, old_pos);

	//���õ�һ��ѩ�˵��ӽ�
	mSnowMan->Update(eye, mCamera, old_pos);

	//���õڶ���ѩ�˵��ӽ�
	mSnowMan2->Update(eye);

	//����ѩ��״̬
	mSnowParticle->Update(25.f*dt, mtimer.TotalTime());

	mCamera->UpdateViewMatrix();
}
void SecenFactory::Draw(ID3D11DeviceContext* dc)
{
	//���Ƶ���
	mTerrain->Draw(dc, mCamera->GetViewProj());

	//������պ�
	mSky->Draw(dc, mCamera->GetViewProj());

	//����������
	mBox->Draw(dc, mCamera->GetViewProj());

	//���ƹ������
	mTree2D->Draw(dc, mCamera->GetViewProj());

	//���Ƶ�һ��ѩ��
	mSnowMan->Draw(dc, mCamera->GetViewProj());

	//���Ƶڶ���ѩ��  ʹ��һֱ��������
	XMMATRIX BoxWorld = mBox->GetWorld();
	XMFLOAT4X4 SnowWorld;
	XMStoreFloat4x4(&SnowWorld, BoxWorld);
	SnowWorld._42 += mBox->GetHeight() / 2 + 5.f;  //5.fΪѩ������߶ȣ�������ͷ���ĸ߶ȣ�
	mSnowMan2->SetSnowWorld(SnowWorld);
	mSnowMan2->Draw(dc, mCamera->GetViewProj());

	//����ѩ��
	mSnowParticle->SetEyeAndEmit(mCamera->GetPosition());
	mSnowParticle->Draw(dc, mCamera->GetViewProj());

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	dc->RSSetState(0);
	dc->OMSetDepthStencilState(0, 0);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff);
}