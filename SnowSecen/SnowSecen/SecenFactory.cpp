#include "SecenFactory.h"
bool SecenFactory::Init(ID3D11Device* device,float aspectratio)
{
	if (!Effects::InitAll(device))
		return false;
	//初始化相机
	mCamera = new Camera();
	//我们把摄像机初始位置设在[0,f,5.f,-10.f]，默认朝向为z轴正方向
	mCamera->SetPosition(0.f, 10.f, 0.f);
	//设置好投影相关参数
	mCamera->SetLens(XM_PIDIV4, aspectratio, 0.5f, 1000.f);
	//初始化地形
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

	//初始化天空盒
	XMMATRIX SkyWorld = XMMatrixIdentity();
	mSky = new Sky(device, SkyWorld, 1000.f, 30, 30);
	mSky->Init();

	//初始化立方体
	XMMATRIX BoxWorld = XMMatrixTranslation(0.f, 0.f, 200.f);
	mBox = new Box(device, BoxWorld, 10, 20, 20);
	mBox->Init();

	//初始化公告板树
	XMMATRIX TreeWorld = XMMatrixTranslation(0.f, 0.f, 0.f);
	mTree2D = new Tree2D(device, TreeWorld, mTerrain);
	mTree2D->Init();

	//初始化第一个雪人
	XMMATRIX SnowmanWorld = XMMatrixTranslation(0.f, mTerrain->getHeight(0.f, 150.f) + 5.f, 150.f);
	mSnowMan = new SnowMan(device, SnowmanWorld);
	mSnowMan->Init();

	//初始化第二个雪人
	SnowmanWorld = XMMatrixTranslation(0.f, mTerrain->getHeight(0.f, 200.f) + mBox->GetHeight() + 5.f, 200.f);
	mSnowMan2 = new SnowMan(device, SnowmanWorld);
	mSnowMan2->Init();

	//初始化雪花粒子
	XMMATRIX SnowparticleWorld = XMMatrixIdentity();
	mSnowParticle = new SnowParticle(device, SnowparticleWorld, 60000);
	mSnowParticle->Init();
	return true;
}


void SecenFactory::Update(float dt, const GameTimer& mtimer)
{
	//前后左右漫游
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

	//计算切线方向，并单位化。 然后第三个语句就是在切线方向更新这个帧间隔时间dt内走了多长距离
	XMVECTOR tangent = XMLoadFloat3(&new_pos) - XMLoadFloat3(&old_pos);
	tangent = XMVector3Normalize(tangent);
	POSW += tangent * 25.f*dt;

	//计算切线之后再矫正一下高度
	XMStoreFloat3(&new_pos, POSW);
	new_y = 10.f + mTerrain->getHeight(new_pos.x, new_pos.z);
	new_pos = XMFLOAT3(new_pos.x, new_y, new_pos.z);

	//防止相机漫游地形越界
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

	//从相机获得视角投影矩阵
	XMMATRIX viewProj = mCamera->GetViewProj();
	XMFLOAT3 eye = mCamera->GetPosition();

	//设置地形的视角
	mTerrain->Update(eye);

	//设置天空盒的视角 
	mSky->Update(viewProj, eye);
	/*XMMATRIX T = XMMatrixTranslation(eye.x, eye.y, eye.z); //可以实现于自身的Update函数里
	XMMATRIX WVP = XMMatrixMultiply(T, mCamera->GetViewProj());
	Effects::pSkyEffect->WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&WVP));*/

	//设置箱子的视角
	mBox->Update(dt / 3, mTerrain, mCamera, eye);  //可以调时间，便于调试简单的上车情况

	//设置公告板树的视角
	mTree2D->Update(eye, mCamera, old_pos);

	//设置第一个雪人的视角
	mSnowMan->Update(eye, mCamera, old_pos);

	//设置第二个雪人的视角
	mSnowMan2->Update(eye);

	//设置雪花状态
	mSnowParticle->Update(25.f*dt, mtimer.TotalTime());

	mCamera->UpdateViewMatrix();
}
void SecenFactory::Draw(ID3D11DeviceContext* dc)
{
	//绘制地形
	mTerrain->Draw(dc, mCamera->GetViewProj());

	//绘制天空盒
	mSky->Draw(dc, mCamera->GetViewProj());

	//绘制立方体
	mBox->Draw(dc, mCamera->GetViewProj());

	//绘制公告板树
	mTree2D->Draw(dc, mCamera->GetViewProj());

	//绘制第一个雪人
	mSnowMan->Draw(dc, mCamera->GetViewProj());

	//绘制第二个雪人  使其一直在箱子上
	XMMATRIX BoxWorld = mBox->GetWorld();
	XMFLOAT4X4 SnowWorld;
	XMStoreFloat4x4(&SnowWorld, BoxWorld);
	SnowWorld._42 += mBox->GetHeight() / 2 + 5.f;  //5.f为雪人身体高度（不包括头部的高度）
	mSnowMan2->SetSnowWorld(SnowWorld);
	mSnowMan2->Draw(dc, mCamera->GetViewProj());

	//绘制雪花
	mSnowParticle->SetEyeAndEmit(mCamera->GetPosition());
	mSnowParticle->Draw(dc, mCamera->GetViewProj());

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	dc->RSSetState(0);
	dc->OMSetDepthStencilState(0, 0);
	dc->OMSetBlendState(0, blendFactor, 0xffffffff);
}