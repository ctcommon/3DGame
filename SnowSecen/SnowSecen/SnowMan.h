#pragma once
#ifndef SNOWMAN_H_
#define SNOWMAN_H_
#include "SecenBase.h"
#include "Terrain.h"
#include "Camera.h"

class Terrain;
class Camera;

class SnowMan : public SecenBase
{
public:
	SnowMan(ID3D11Device* device, CXMMATRIX world);
	~SnowMan();

	SnowMan(const SnowMan& rhs) = delete;
	SnowMan& operator=(const SnowMan& rhs) = delete;

	void SetSnowWorld(const XMFLOAT4X4& wrold) { mWorld = wrold; }

	void Update(const XMFLOAT3& eyepos,Camera* camera,const XMFLOAT3& old_pos); //为在地上的雪人所用的函数，主要应用简单的碰撞检测

	void Update(const XMFLOAT3& eyepos);  //为在立方体上的雪人所用的函数

	void Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj) override;

protected:
	void SetMaterial() override;
	void SetEffect(CXMMATRIX vieproj)override;
	bool BuildBuffers()override;
	bool BuildSRVs()override;
	bool BuildInputLayouts()override;

private:
	void SetMatrix();
	void MakePartToMan();
	void DrawPart(ID3DX11EffectTechnique* tech, ID3D11DeviceContext* dc, const Lights::Material& mat, ID3D11ShaderResourceView* tex, CXMMATRIX world, CXMMATRIX viewproj,
		UINT inum, UINT istart, UINT vstart);
	void AppendPart(GeometryGenerator::MeshData& Total, GeometryGenerator::MeshData& data, UINT vStart, UINT iStart) {
		for (size_t i = 0; i < data.vertices.size(); i++) {
			Total.vertices[vStart + i] = data.vertices[i];
		}
		for (size_t i = 0; i < data.indices.size(); i++) {
			Total.indices[iStart + i] = data.indices[i];
		}
	}

private:
	XMFLOAT4X4				mBody;
	XMFLOAT4X4				mHead;
	XMFLOAT4X4				mLeftHand;
	XMFLOAT4X4				mRightHand;
	XMFLOAT4X4              mButton1;
	XMFLOAT4X4              mButton2;
	XMFLOAT4X4              mButton3;
	XMFLOAT4X4              mMouth;
	XMFLOAT4X4              mNose;
	XMFLOAT4X4              mLeftEye;
	XMFLOAT4X4              mRightEye;

	UINT                    mBodyVStart, mBodyIStart;
	UINT                    mHeadVStart, mHeadIStart;
	UINT                    mLeftHandVStart, mLeftHandIStart;
	UINT                    mRightHandVStart, mRightHandIStart;
	UINT                    mButton1VStart, mButton1IStart;
	UINT                    mButton2VStart, mButton2IStart;
	UINT                    mButton3VStart, mButton3IStart;
	UINT                    mMouthVStart, mMouthIStart;
	UINT                    mNoseVStart, mNoseIStart;
	UINT                    mLeftEyeVStart, mLeftEyeIStart;
	UINT                    mRightEyeVStart, mRightEyeIStart;

	Lights::Material        mMatBodyAndHead;	//身体和头
	Lights::Material        mMatHand;			//双手
	Lights::Material        mMatButton;			//纽扣
	Lights::Material        mMatMouth;			//嘴
	Lights::Material        mMatNose;			//鼻子
	Lights::Material        mMatEye;			//眼睛

	ID3D11ShaderResourceView*       mTexStone;
	ID3D11ShaderResourceView*       mTexWood;
	ID3D11ShaderResourceView*       mTexSnow;
	ID3D11ShaderResourceView*       mTexNose;

	GeometryGenerator::MeshData              Snowman;

	XMFLOAT4X4	texTransbox;
	XMFLOAT4X4	worldInvTrans;
};
#endif // !SNOWMAN_H_
