#include "Sky.h"

Sky::Sky(ID3D11Device* device, CXMMATRIX world, float skySphereRadius, int slice, int stack):
	SecenBase(device, world),mCubeMapSRV(nullptr)
{
	GeometryGenerator::GetInstance()->CreateSphere(skySphereRadius, slice, stack, sphere);
}

Sky::~Sky()
{
	SafeRelease(mCubeMapSRV);
}

bool Sky::BuildBuffers()
{
	VerticeCount = sphere.vertices.size();
	IndiceCount = sphere.indices.size();
	std::vector<Vertex::BaseVertice> vertices(sphere.vertices.size());

	for (size_t i = 0; i < sphere.vertices.size(); ++i)
	{
		vertices[i].pos = sphere.vertices[i].pos;
	}

	std::vector<UINT> indices;
	indices.assign(sphere.indices.begin(), sphere.indices.end());
	return BuildBuffers_(vertices, indices);
}

bool Sky::BuildSRVs()
{
	HR(CreateDDSTextureFromFile(Device, L"Textures//snowcube1024.dds", nullptr, &mCubeMapSRV), L"CreateDDSTextureFromFile");
	return true;
}

bool Sky::BuildInputLayouts()
{
	UINT numLayoutElements = ARRAYSIZE(InputLayoutDesc::Pos);
	return BuildInputLayouts_(InputLayoutDesc::Pos, Effects::pSkyEffect->SkyTech, numLayoutElements);
}

void Sky::Update(CXMMATRIX vieproj,const XMFLOAT3& eyepos)
{
	XMMATRIX T = XMMatrixTranslation(eyepos.x, eyepos.y, eyepos.z);
	XMMATRIX WVP = T * vieproj;
	Effects::pSkyEffect->WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&WVP));
}
void Sky::SetEffect(CXMMATRIX vieproj)
{
	Effects::pSkyEffect->CubeMap->SetResource(mCubeMapSRV);
}

void Sky::Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj)
{
	dc->IASetInputLayout(mInputLayout);
	UINT stride = sizeof(Vertex::BaseVertice);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mVerticeBuffer, &stride, &offset);
	dc->IASetIndexBuffer(mIndiceBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	ID3DX11EffectTechnique* tech = Effects::pSkyEffect->SkyTech;
	Render(tech, dc, viewproj, IndiceCount, 0, 0);
}