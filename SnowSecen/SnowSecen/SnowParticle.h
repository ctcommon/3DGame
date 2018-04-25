#pragma once
#ifndef  SNOWPARTICLE_H_
#define  SNOWPARTICLE_H_

#include "SecenBase.h"
#include "Camera.h"

class Camera;

class SnowParticle : public SecenBase
{
public:
	SnowParticle(ID3D11Device* device, CXMMATRIX world, UINT maxParticles);
	~SnowParticle();

	SnowParticle(const SnowParticle& rhs) = delete;
	SnowParticle& operator=(const SnowParticle& rhs) = delete;

	void SetEyeAndEmit(const XMFLOAT3& eyePosW) { mEmitPosW  = eyePosW, mEyePosW = eyePosW;}
	void Update(float dt, float gameTime)
	{
		mGameTime = gameTime;
		mTimeStep = dt;

		mAge += dt;
	}

	void Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj) override;
protected:
	bool BuildBuffers();
	bool BuildSRVs()override;
	bool BuildInputLayouts()override;
	void SetEffect(CXMMATRIX vieproj)override;
	ID3D11ShaderResourceView* CreateRandomTexture1DSRV();
private:
	UINT mMaxParticles;
	bool mFirstRun;

	float mGameTime;
	float mTimeStep;
	float mAge;

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	ID3D11ShaderResourceView* mRandomTexSRV;
	ID3D11ShaderResourceView* mSnowTexSRV;


};
#endif // ! SNOWPARTICLE_H_
