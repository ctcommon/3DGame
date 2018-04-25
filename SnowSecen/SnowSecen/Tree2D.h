#pragma once
#ifndef  TREE2D_H_
#define TREE2D_H_

#include "SecenBase.h"
#include "Terrain.h"
#include "Camera.h"

class Terrain;
class Camera;

class Tree2D : public SecenBase
{
public:
	Tree2D(ID3D11Device* device, CXMMATRIX world,const Terrain* terrain);
	~Tree2D();

	Tree2D(const Tree2D& rhs) = delete;
	Tree2D& operator=(const Tree2D& rhs) = delete;

	void SetTreePos(const Terrain* terrain);
	void Update(const XMFLOAT3& eyepos,Camera* camera, const XMFLOAT3& old_pos);
	void Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj) override;

protected:
	void SetMaterial() override;
	void SetEffect(CXMMATRIX vieproj)override;
	bool BuildBuffers()override;
	bool BuildSRVs()override;
	bool BuildInputLayouts()override;

private:
	void SetMatrix();
	bool RenderState();

private:
	ID3D11ShaderResourceView * mTreeTextureMapArraySRV;
	ID3D11BlendState* AlphaToCoverageBS;
	Lights::Material mTreeMat;
	XMFLOAT4X4 TreesInvTranspose;
	XMFLOAT4X4 TexTransTree;

	static const int Treecount = 8;
	std::vector<Vertex::SizeVertice> mTrees;
};
#endif // ! TREE2D_H_

