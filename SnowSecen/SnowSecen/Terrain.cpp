#include "Terrain.h"
Terrain::Terrain(ID3D11Device* device, CXMMATRIX world, const InitInfo& initInfo):
	SecenBase(device,world), mInfo(initInfo), mNumPatchVertices(0), mNumPatchQuadFaces(0),
	mNumPatchVertRows(0), mNumPatchVertCols(0), mLayerMapArraySRV(nullptr), mBlendMap(nullptr),
	mHeightMapSRV(nullptr)
{

	mNumPatchVertRows = ((mInfo.HeightmapHeight - 1) / CellsPerPatch) + 1;
	mNumPatchVertCols = ((mInfo.HeightmapWidth - 1) / CellsPerPatch) + 1;

	mNumPatchVertices = mNumPatchVertRows * mNumPatchVertCols;
	mNumPatchQuadFaces = (mNumPatchVertRows - 1)*(mNumPatchVertCols - 1);

	SetMaterial();

	loadHeightmap();

	smooth();

	CalcAllPatchBoundsY();
}

Terrain::~Terrain()
{
	SafeRelease(mHeightMapSRV);
	SafeRelease(mBlendMap);
	SafeRelease(mLayerMapArraySRV);
}

void Terrain::SetMaterial()
{
	mTerrainMat.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mTerrainMat.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mTerrainMat.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	mTerrainMat.reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}



float Terrain::getHeight(float x, float z)const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*width()) / mInfo.CellSpacing;
	float d = (z - 0.5f*depth()) / -mInfo.CellSpacing;


	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = mHeightmap[row*mInfo.HeightmapWidth + col];
	float B = mHeightmap[row*mInfo.HeightmapWidth + col + 1];
	float C = mHeightmap[(row + 1)*mInfo.HeightmapWidth + col];
	float D = mHeightmap[(row + 1)*mInfo.HeightmapWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}

void Terrain::ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M)
{
	XMFLOAT4X4 tempM;
	XMStoreFloat4x4(&tempM, M);

	//
	// Left
	//
	planes[0].x = tempM(0, 3) + tempM(0, 0);
	planes[0].y = tempM(1, 3) + tempM(1, 0);
	planes[0].z = tempM(2, 3) + tempM(2, 0);
	planes[0].w = tempM(3, 3) + tempM(3, 0);

	//
	// Right
	//
	planes[1].x = tempM(0, 3) - tempM(0, 0);
	planes[1].y = tempM(1, 3) - tempM(1, 0);
	planes[1].z = tempM(2, 3) - tempM(2, 0);
	planes[1].w = tempM(3, 3) - tempM(3, 0);

	//
	// Bottom
	//
	planes[2].x = tempM(0, 3) + tempM(0, 1);
	planes[2].y = tempM(1, 3) + tempM(1, 1);
	planes[2].z = tempM(2, 3) + tempM(2, 1);
	planes[2].w = tempM(3, 3) + tempM(3, 1);

	//
	// Top
	//
	planes[3].x = tempM(0, 3) - tempM(0, 1);
	planes[3].y = tempM(1, 3) - tempM(1, 1);
	planes[3].z = tempM(2, 3) - tempM(2, 1);
	planes[3].w = tempM(3, 3) - tempM(3, 1);

	//
	// Near
	//
	planes[4].x = tempM(0, 2);
	planes[4].y = tempM(1, 2);
	planes[4].z = tempM(2, 2);
	planes[4].w = tempM(3, 2);

	//
	// Far
	//
	planes[5].x = tempM(0, 3) - tempM(0, 2);
	planes[5].y = tempM(1, 3) - tempM(1, 2);
	planes[5].z = tempM(2, 3) - tempM(2, 2);
	planes[5].w = tempM(3, 3) - tempM(3, 2);

	// Normalize the plane equations.
	for (int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}
void Terrain::loadHeightmap()
{
	// A height for each vertex
	std::vector<unsigned char> in(mInfo.HeightmapWidth * mInfo.HeightmapHeight);

	// Open the file.
	std::ifstream inFile;
	inFile.open(mInfo.HeightmapFilename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array, and scale and offset the heights.
	mHeightmap.resize(mInfo.HeightmapHeight * mInfo.HeightmapWidth, 0);
	for (UINT i = 0; i < mInfo.HeightmapHeight * mInfo.HeightmapWidth; ++i)
	{
		mHeightmap[i] = (in[i] / 255.0f)*mInfo.HeightScale;
	}
}

void Terrain::smooth()
{
	std::vector<float> dest(mHeightmap.size());

	for (UINT i = 0; i < mInfo.HeightmapHeight; ++i)
	{
		for (UINT j = 0; j < mInfo.HeightmapWidth; ++j)
		{
			dest[i*mInfo.HeightmapWidth + j] = average(i, j);
		}
	}

	// Replace the old heightmap with the filtered one.
	mHeightmap = dest;
}

bool Terrain::inBounds(UINT i, UINT j)
{
	// True if ij are valid indices; false otherwise.
	return
		i >= 0 && i < mInfo.HeightmapHeight &&
		j >= 0 && j < mInfo.HeightmapWidth;
}

float Terrain::average(UINT i, UINT j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	for (UINT m = i - 1; m <= i + 1; ++m)
	{
		for (UINT n = j - 1; n <= j + 1; ++n)
		{
			if (inBounds(m, n))
			{
				avg += mHeightmap[m*mInfo.HeightmapWidth + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}


void Terrain::CalcAllPatchBoundsY()
{
	mPatchBoundsY.resize(mNumPatchQuadFaces);

	// For each patch
	for (UINT i = 0; i < mNumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < mNumPatchVertCols - 1; ++j)
		{
			CalcPatchBoundsY(i, j);
		}
	}
}

void Terrain::CalcPatchBoundsY(UINT i, UINT j)
{
	// Scan the heightmap values this patch covers and compute the min/max height.

	UINT x0 = j * CellsPerPatch;
	UINT x1 = (j + 1)*CellsPerPatch;

	UINT y0 = i * CellsPerPatch;
	UINT y1 = (i + 1)*CellsPerPatch;

	float minY = +FLT_MAX;
	float maxY = -FLT_MAX;
	for (UINT y = y0; y <= y1; ++y)
	{
		for (UINT x = x0; x <= x1; ++x)
		{
			UINT k = y * mInfo.HeightmapWidth + x;
			minY = min(minY, mHeightmap[k]);
			maxY = max(maxY, mHeightmap[k]);
		}
	}

	UINT patchID = i * (mNumPatchVertCols - 1) + j;
	mPatchBoundsY[patchID] = XMFLOAT2(minY, maxY);
}

bool Terrain::BuildHeightmapSRV()
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mInfo.HeightmapWidth;
	texDesc.Height = mInfo.HeightmapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;


	std::vector<HALF> hmap(mHeightmap.size());
	std::transform(mHeightmap.begin(), mHeightmap.end(), hmap.begin(), XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
	data.SysMemPitch = mInfo.HeightmapWidth * sizeof(HALF);
	data.SysMemSlicePitch = 0;

	ID3D11Texture2D* hmapTex = 0;
	HR(Device->CreateTexture2D(&texDesc, &data, &hmapTex), L"CreateTexture2D");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	HR(Device->CreateShaderResourceView(hmapTex, &srvDesc, &mHeightMapSRV), L"CreateShaderResourceView");

	// SRV saves reference.
	SafeRelease(hmapTex);
	return true;
}

bool Terrain::BuildBuffers()
{
	std::vector<Vertex::TerrainVertice> patchVertices(mNumPatchVertRows*mNumPatchVertCols);
	float halfWidth = 0.5f*width();
	float halfDepth = 0.5f*depth();
	float patchWidth = width() / (mNumPatchVertCols - 1);
	float patchDepth = depth() / (mNumPatchVertRows - 1);
	float du = 1.0f / (mNumPatchVertCols - 1);
	float dv = 1.0f / (mNumPatchVertRows - 1);

	for (UINT i = 0; i < mNumPatchVertRows; ++i)
	{
		float z = halfDepth - i * patchDepth;
		for (UINT j = 0; j < mNumPatchVertCols; ++j)
		{
			float x = -halfWidth + j * patchWidth;

			patchVertices[i*mNumPatchVertCols + j].pos = XMFLOAT3(x, 0.0f, z);

			// Stretch texture over grid.
			patchVertices[i*mNumPatchVertCols + j].tex.x = j * du;
			patchVertices[i*mNumPatchVertCols + j].tex.y = i * dv;
		}
	}

	// Store axis-aligned bounding box y-bounds in upper-left patch corner.
	for (UINT i = 0; i < mNumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < mNumPatchVertCols - 1; ++j)
		{
			UINT patchID = i * (mNumPatchVertCols - 1) + j;
			patchVertices[i*mNumPatchVertCols + j].BoundsY = mPatchBoundsY[patchID];
		}
	}

	std::vector<UINT> indices(mNumPatchQuadFaces * 4);

	int k = 0;
	for (UINT i = 0; i < mNumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < mNumPatchVertCols - 1; ++j)
		{
			// Top row of 2x2 quad patch
			indices[k] = i * mNumPatchVertCols + j;
			indices[k + 1] = i * mNumPatchVertCols + j + 1;

			// Bottom row of 2x2 quad patch
			indices[k + 2] = (i + 1)*mNumPatchVertCols + j;
			indices[k + 3] = (i + 1)*mNumPatchVertCols + j + 1;

			k += 4; // next quad
		}
	}

	VerticeCount = patchVertices.size();
	IndiceCount = mNumPatchQuadFaces * 4;

	return BuildBuffers_(patchVertices, indices);
}

bool Terrain::BuildSRVs()
{
	if (!BuildHeightmapSRV())
	{
		return false;
	}
	HR(CreateDDSTextureFromFile(Device, mInfo.LayerMapArrayFilename.c_str(), nullptr, &mLayerMapArraySRV), L"CreateDDSTextureFromFile");
	HR(CreateDDSTextureFromFile(Device, mInfo.BlendMapFilename.c_str(), nullptr, &mBlendMap), L"CreateDDSTextureFromFile");
	return true;
}

bool Terrain::BuildInputLayouts()
{
	UINT numLayoutElements = ARRAYSIZE(InputLayoutDesc::Terrain);
	return BuildInputLayouts_(InputLayoutDesc::Terrain, Effects::pTerrainEffect->LightFogTech, numLayoutElements);
}

void Terrain::Update(const XMFLOAT3& eyepos)
{
	Effects::pTerrainEffect->EyePosW->SetRawValue(&eyepos, 0, sizeof(eyepos));
}

void Terrain::SetEffect(CXMMATRIX vieproj)
{

	XMFLOAT4 worldPlanes[6];
	ExtractFrustumPlanes(worldPlanes, vieproj);

	Effects::pTerrainEffect->ViewProj->SetMatrix(reinterpret_cast<const float*>(&vieproj));
	Effects::pTerrainEffect->World->SetMatrix(reinterpret_cast<const float*>(&mWorld));
	//Effects::pTerrainEffect->EyePosW->SetRawValue(&camera.GetPosition(), 0, sizeof(XMFLOAT3));
	Effects::pTerrainEffect->DirLights->SetRawValue(&mDirLights[1], 0, 2 * sizeof(Lights::DirectionalLight));
	Effects::pTerrainEffect->FogColor->SetFloatVector(reinterpret_cast<const float*>(&Colors::Silver));
	Effects::pTerrainEffect->FogStart->SetFloat(200.0f);
	Effects::pTerrainEffect->FogRange->SetFloat(500.0f);
	Effects::pTerrainEffect->MinDist->SetFloat(20.0f);
	Effects::pTerrainEffect->MaxDist->SetFloat(500.0f);
	Effects::pTerrainEffect->MinTess->SetFloat(0.0f);
	Effects::pTerrainEffect->MaxTess->SetFloat(6.0f);
	Effects::pTerrainEffect->TexelCellSpaceU->SetFloat(1.0f / mInfo.HeightmapWidth);
	Effects::pTerrainEffect->TexelCellSpaceV->SetFloat(1.0f / mInfo.HeightmapHeight);
	Effects::pTerrainEffect->WorldCellSpace->SetFloat(mInfo.CellSpacing);
	Effects::pTerrainEffect->WorldFrustumPlanes->SetFloatVectorArray(reinterpret_cast<float*>(worldPlanes), 0, 6);
	Effects::pTerrainEffect->LayerMapArray->SetResource(mLayerMapArraySRV);
	Effects::pTerrainEffect->BlendMap->SetResource(mBlendMap);
	Effects::pTerrainEffect->HeightMap->SetResource(mHeightMapSRV);
	Effects::pTerrainEffect->Mat->SetRawValue(&mTerrainMat, 0, sizeof(Lights::Material));
}

void Terrain::Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj)
{
	dc->IASetInputLayout(mInputLayout);
	UINT stride = sizeof(Vertex::TerrainVertice);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mVerticeBuffer, &stride, &offset);
	dc->IASetIndexBuffer(mIndiceBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	ID3DX11EffectTechnique* tech = Effects::pTerrainEffect->LightFogTech;
	Render(tech, dc, viewproj, IndiceCount, 0, 0);

	dc->HSSetShader(0, 0, 0);
	dc->DSSetShader(0, 0, 0);
}