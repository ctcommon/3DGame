#pragma once
#ifndef  TERRAIN_H_
#define	 TERRAIN_H_
#include "SecenBase.h"
#include <sstream>
#include <fstream>
#include <float.h>
#include <algorithm>
#include <DirectXPackedVector.h>

using namespace PackedVector;

class Terrain : public SecenBase
{
public:
	struct InitInfo
	{
		std::wstring HeightmapFilename;
		std::wstring LayerMapArrayFilename;
		std::wstring BlendMapFilename;
		float HeightScale;
		UINT HeightmapWidth;
		UINT HeightmapHeight;
		float CellSpacing;
	};

public:
	Terrain(ID3D11Device* device, CXMMATRIX world, const InitInfo& initInfo);
	~Terrain();

	Terrain(const Terrain& rhs) = delete;
	Terrain& operator=(const Terrain& rhs) = delete;

	void Update(const XMFLOAT3& eyepos);
	void Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj) override;

	float width()const { return (mInfo.HeightmapWidth - 1)*mInfo.CellSpacing; }
	float depth()const { return (mInfo.HeightmapHeight - 1)*mInfo.CellSpacing; }
	float getHeight(float x, float z)const;

protected:
	void SetMaterial() override;
	void SetEffect(CXMMATRIX vieproj)override;
	bool BuildBuffers()override;
	bool BuildSRVs()override;
	bool BuildInputLayouts()override;
private:
	bool BuildHeightmapSRV();
	void loadHeightmap();
	void smooth();
	bool inBounds(UINT i, UINT j);
	float average(UINT i, UINT j);
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT i, UINT j);
	void ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M);
private:
	static const int CellsPerPatch = 64;
	InitInfo mInfo;

	UINT mNumPatchVertices;
	UINT mNumPatchQuadFaces;

	UINT mNumPatchVertRows;
	UINT mNumPatchVertCols;

	std::vector<XMFLOAT2> mPatchBoundsY;
	std::vector<float> mHeightmap;

	Lights::Material mTerrainMat;

	ID3D11ShaderResourceView* mLayerMapArraySRV;
	ID3D11ShaderResourceView* mBlendMap;
	ID3D11ShaderResourceView* mHeightMapSRV;
};
#endif // ! TERRAIN_H_
