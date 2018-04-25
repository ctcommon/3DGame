#include "SnowParticle.h"
#include <ctime>

SnowParticle::SnowParticle(ID3D11Device* device, CXMMATRIX world, UINT maxparticles) :
	SecenBase(device, world), mMaxParticles(maxparticles), mFirstRun(true),mRandomTexSRV(nullptr), mSnowTexSRV(nullptr),
	mDrawVB(nullptr), mStreamOutVB(nullptr)
{
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

SnowParticle::~SnowParticle()
{
	SafeRelease(mStreamOutVB);
	SafeRelease(mDrawVB);
	SafeRelease(mSnowTexSRV);
	SafeRelease(mRandomTexSRV);
}

bool SnowParticle::BuildBuffers()
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex::Particle);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	Vertex::Particle p;
	ZeroMemory(&p, sizeof(Vertex::Particle));
	p.Age = 0.0f;
	p.Type = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

	HR(Device->CreateBuffer(&vbd, &vinitData, &mVerticeBuffer),L"CreateBuffer");

	vbd.ByteWidth = sizeof(Vertex::Particle) * mMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	HR(Device->CreateBuffer(&vbd, 0, &mDrawVB), L"CreateBuffer");
	HR(Device->CreateBuffer(&vbd, 0, &mStreamOutVB), L"CreateBuffer");
	return true;
}

bool SnowParticle::BuildSRVs()
{
	mRandomTexSRV = CreateRandomTexture1DSRV();
	if (!mRandomTexSRV)
		return false;
	HR(CreateDDSTextureFromFile(Device, L"Textures//snow.dds", nullptr, &mSnowTexSRV), L"CreateDDSTextureFromFile");
	return true;
}

bool SnowParticle::BuildInputLayouts()
{
	UINT numLayoutElements = ARRAYSIZE(InputLayoutDesc::Particle);
	return BuildInputLayouts_(InputLayoutDesc::Particle, Effects::pParticleEffect->StreamOutTech, numLayoutElements);
}

ID3D11ShaderResourceView* SnowParticle::CreateRandomTexture1DSRV()
{
	XMFLOAT4 randomValues[1024];

	srand((unsigned)time(NULL));
	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = -1.f + (static_cast<float>(rand()) / RAND_MAX)*(2.f);
		randomValues[i].y = -1.f + (static_cast<float>(rand()) / RAND_MAX)*(2.f);
		randomValues[i].z = -1.f + (static_cast<float>(rand()) / RAND_MAX)*(2.f);
		randomValues[i].w = -1.f + (static_cast<float>(rand()) / RAND_MAX)*(2.f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	HR(Device->CreateTexture1D(&texDesc, &initData, &randomTex), L"CreateTexture1D");

	//
	// Create the resource view.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* randomTexSRV = 0;
	HR(Device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV), L"CreateShaderResourceView");

	SafeRelease(randomTex);
	return randomTexSRV;
}

void SnowParticle::SetEffect(CXMMATRIX vieproj)
{
	Effects::pParticleEffect->ViewProj->SetMatrix(reinterpret_cast<const float*>(&vieproj));
	Effects::pParticleEffect->GameTime->SetFloat(mGameTime);
	Effects::pParticleEffect->TimeStep->SetFloat(mTimeStep);
	Effects::pParticleEffect->EyePosW->SetRawValue(&mEyePosW, 0, sizeof(XMFLOAT3));
	Effects::pParticleEffect->EmitPosW->SetRawValue(&mEmitPosW, 0, sizeof(XMFLOAT3));
	Effects::pParticleEffect->TexArray->SetResource(mSnowTexSRV);
	Effects::pParticleEffect->RandomTex->SetResource(mRandomTexSRV);
}

void SnowParticle::Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj)
{
	SetEffect(viewproj);
	dc->IASetInputLayout(mInputLayout);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(Vertex::Particle);
	UINT offset = 0;

	if (mFirstRun)
		dc->IASetVertexBuffers(0, 1, &mVerticeBuffer, &stride, &offset);
	else
		dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	dc->SOSetTargets(1, &mStreamOutVB, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::pParticleEffect->StreamOutTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::pParticleEffect->StreamOutTech->GetPassByIndex(p)->Apply(0, dc);

		if (mFirstRun)
		{
			dc->Draw(1, 0);
			mFirstRun = false;
		}
		else
		{
			dc->DrawAuto();
		}
	}

	ID3D11Buffer* bufferArray[1] = { 0 };
	dc->SOSetTargets(1, bufferArray, &offset);

	std::swap(mDrawVB, mStreamOutVB);

	dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	Effects::pParticleEffect->DrawTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::pParticleEffect->DrawTech->GetPassByIndex(p)->Apply(0, dc);

		dc->DrawAuto();
	}
}