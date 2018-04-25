#pragma once
#ifndef BOX_H_
#define BOX_H_
#include "SecenBase.h"
#include "Terrain.h"
#include "Camera.h"

class Terrain;
class Camera;

class Box : public SecenBase
{
public:
	Box(ID3D11Device* device, CXMMATRIX world,float height, float width, float depth);
	~Box();

	Box(const Box& rhs) = delete;
	Box& operator=(const Box& rhs) = delete;

	void Update(float dt,const Terrain* terrain,  Camera* camera,const XMFLOAT3& eyeposw) ;
	void Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj) override;

	float GetWidth()const { return BoxWidth; }
	float GetHeight()const { return BoxHeight; }
	float GetDepth()const { return BoxDepth; }

protected:
	void SetMaterial() override;
	void SetEffect(CXMMATRIX vieproj)override;
	bool BuildBuffers()override;
	bool BuildSRVs()override;
	bool BuildInputLayouts()override;

private:
	void SetMatrix();  //很多场景都用得到，是否和texTransbox以及worldInvTrans放在基类当中？

	bool RenderState();  //设置平面阴影渲染状态
	void SetShadowMartrix(const Camera* camera, const Terrain* terrain); //设置绘制阴影的相关矩阵
	ID3D11DepthStencilState* NoDoubleBlendingDSS;  //用于绘制阴影的时候防止同一个位置阴影产生叠加
	ID3D11BlendState* NoColorWriteBS;

	GeometryGenerator::MeshData		box;
	float BoxHeight;
	float BoxWidth;
	float BoxDepth;

	ID3D11ShaderResourceView* pSRVBox;
	ID3D11ShaderResourceView* pSecondSRVBox;
	Lights::Material			materialBox;

	XMFLOAT4X4	texTransbox;
	XMFLOAT4X4	worldInvTrans;

	XMFLOAT4X4			texTransShadow;
	XMFLOAT4X4			wvpShadow;
	XMFLOAT4X4			worldShadow;
	XMFLOAT4X4			worldInvTransShadow;
	Lights::Material	materialShadow;  //阴影材质

	bool camonbox;

};
#endif // !BOX_H_

