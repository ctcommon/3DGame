#include "Tree2D.h"
Tree2D::Tree2D(ID3D11Device* device, CXMMATRIX world,const Terrain* terrain):
	SecenBase(device,world), mTreeTextureMapArraySRV(nullptr), AlphaToCoverageBS(nullptr)
{
	mTrees.resize(Treecount);
	SetMatrix();
	SetMaterial();
	RenderState();
	SetTreePos(terrain);//设置公告板树的位置
}

Tree2D::~Tree2D()
{
	SafeRelease(AlphaToCoverageBS);
	SafeRelease(mTreeTextureMapArraySRV);
}

void Tree2D::SetMaterial()
{
	mTreeMat.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mTreeMat.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mTreeMat.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
}

void Tree2D::SetMatrix()
{
	XMMATRIX wTree = XMLoadFloat4x4(&mWorld);
	XMVECTOR det = XMMatrixDeterminant(wTree);  //求逆矩阵，可以用函数
	XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det, wTree));
	XMStoreFloat4x4(&TreesInvTranspose, worldInvTranspose);
	XMStoreFloat4x4(&TexTransTree, XMMatrixIdentity());
}

void Tree2D::SetTreePos(const Terrain* terrain)  //公告牌多的话可以采取随机置位，不过要采取措施避免随机后公告牌的位置还是一样
{
	mTrees[0].pos = XMFLOAT3(80.f, terrain->getHeight(80.f, 400.f) + 35.f, 400.f);
	mTrees[0].size = XMFLOAT2(80.f, 80.f);
	mTrees[1].pos = XMFLOAT3(170.f, terrain->getHeight(170.f, 300.f) + 35.f, 300.f);
	mTrees[1].size = XMFLOAT2(80.f, 80.f);
	mTrees[2].pos = XMFLOAT3(260.f, terrain->getHeight(260.f, 350.f) + 35.f, 350.f);
	mTrees[2].size = XMFLOAT2(80.f, 80.f);
	mTrees[3].pos = XMFLOAT3(-80.f, terrain->getHeight(-80.f, 400.f) + 35.f, 400.f);
	mTrees[3].size = XMFLOAT2(80.f, 80.f);
	mTrees[4].pos = XMFLOAT3(-170.f, terrain->getHeight(-170.f, 300.f) + 35.f, 300.f);
	mTrees[4].size = XMFLOAT2(80.f, 80.f);
	mTrees[5].pos = XMFLOAT3(-260.f, terrain->getHeight(-260.f, 300.f) + 35.f, 300.f);
	mTrees[5].size = XMFLOAT2(80.f, 80.f);
	mTrees[6].pos = XMFLOAT3(-260.f, terrain->getHeight(-260.f, 350.f) + 35.f, 350.f);
	mTrees[6].size = XMFLOAT2(80.f, 80.f);
	mTrees[7].pos = XMFLOAT3(50.f, terrain->getHeight(50.f, 350.f) + 35.f, 350.f);
	mTrees[7].size = XMFLOAT2(80.f, 80.f);
}
bool Tree2D::BuildBuffers()
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::SizeVertice) * Treecount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &mTrees[0];
	HR(Device->CreateBuffer(&vbd, &vinitData, &mVerticeBuffer), L"CreateBuffer");
	return true;
}

bool Tree2D::BuildSRVs()
{
	HR(CreateDDSTextureFromFile(Device, L"Textures//TreeArray.dds", nullptr, &mTreeTextureMapArraySRV), L"Device");
	return true;
}

bool Tree2D::BuildInputLayouts()
{
	UINT numLayoutElements = ARRAYSIZE(InputLayoutDesc::TreePointSprite);
	return BuildInputLayouts_(InputLayoutDesc::TreePointSprite, Effects::pTreeEffect->Light3TexAlphaClipFogTech, numLayoutElements);
}

void Tree2D::Update(const XMFLOAT3& eyepos,Camera* camera, const XMFLOAT3& old_pos)
{
	Effects::pTreeEffect->EyePosW->SetRawValue(&eyepos, 0, sizeof(XMFLOAT3));
	for (size_t i = 0; i < mTrees.size(); i++)
	{
		if (abs(camera->GetPosition().x - mTrees[i].pos.x) < 15 && abs(camera->GetPosition().z - mTrees[i].pos.z) < 15)
		{
			camera->SetPosition(old_pos.x, old_pos.y, old_pos.z);
		}
	}
}

void Tree2D::SetEffect(CXMMATRIX vieproj)
{
	Effects::pTreeEffect->DirLights->SetRawValue(&mDirLights[0], 0, 3 * sizeof(Lights::DirectionalLight));
	Effects::pTreeEffect->FogColor->SetFloatVector(reinterpret_cast<const float*>(&Colors::Silver));
	Effects::pTreeEffect->FogStart->SetFloat(200.0f);
	Effects::pTreeEffect->FogRange->SetFloat(500.0f);
	Effects::pTreeEffect->ViewProj->SetMatrix(reinterpret_cast<const float*>(&vieproj));
	Effects::pTreeEffect->Mat->SetRawValue(&mTreeMat, 0, sizeof(Lights::Material));
	Effects::pTreeEffect->TreeTextureMapArray->SetResource(mTreeTextureMapArraySRV);
}

bool Tree2D::RenderState()
{
	D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(Device->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS), L"CreateBlendState");
	return true;
}

void Tree2D::Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj)
{
	SetMatrix();

	UINT stride = sizeof(Vertex::SizeVertice);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->IASetInputLayout(mInputLayout);

	D3DX11_TECHNIQUE_DESC desc;
	ID3DX11EffectTechnique* tech = Effects::pTreeEffect->Light3TexAlphaClipFogTech;
	tech->GetDesc(&desc);

	for (UINT i = 0; i < desc.Passes; ++i)
	{
		SetEffect(viewproj);
		dc->IASetVertexBuffers(0, 1, &mVerticeBuffer, &stride, &offset);
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		dc->OMSetBlendState(AlphaToCoverageBS, blendFactor, 0xffffffff);
		tech->GetPassByIndex(i)->Apply(0, dc);
		dc->Draw(Treecount, 0);
		dc->OMSetBlendState(0, blendFactor, 0xffffffff);
	}
}