#pragma once
#ifndef SKY_H_
#define SKY_H_

#include "SecenBase.h"
class Sky : public SecenBase
{
public:
	Sky(ID3D11Device* device, CXMMATRIX world, float skySphereRadius, int slice, int stack);
	~Sky();

	Sky(const Sky& rhs) = delete;
	Sky& operator=(const Sky& rhs) = delete;
	void Update(CXMMATRIX vieproj,const XMFLOAT3& eyepos);
	void Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj) override;
protected:
	void SetEffect(CXMMATRIX vieproj)override;
	bool BuildBuffers()override;
	bool BuildSRVs()override;
	bool BuildInputLayouts()override;
private:
	GeometryGenerator::MeshData		sphere;
	ID3D11ShaderResourceView* mCubeMapSRV;
};
#endif // !SKY_H_
