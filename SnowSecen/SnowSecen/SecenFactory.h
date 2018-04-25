#pragma once
#ifndef  SECENFACTORY_H_
#define	 SECENFACTORY_H_
#include "Camera.h"
#include "Terrain.h"
#include "Sky.h"
#include "Box.h"
#include "Tree2D.h"
#include "SnowMan.h"
#include "SnowParticle.h"
#include "GameTimer.h"
class GameTimer;

class SecenFactory
{
public:
	SecenFactory():
		mCamera(nullptr), mTerrain(nullptr), mSky(nullptr), mBox(nullptr),
		mTree2D(nullptr), mSnowMan(nullptr), mSnowMan2(nullptr), mSnowParticle(nullptr)
	{}
	~SecenFactory()
	{
		Effects::ReleaseAll();
		ShutDown<SnowParticle>(mSnowParticle);
		ShutDown<SnowMan>(mSnowMan2);
		ShutDown<SnowMan>(mSnowMan);
		ShutDown<Tree2D>(mTree2D);
		ShutDown<Box>(mBox);
		ShutDown<Sky>(mSky);
		ShutDown<Terrain>(mTerrain);
		ShutDown<Camera>(mCamera);
	}
	template<typename T> void ShutDown(T* ptr)
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}
	Camera* GetCamera() const { return mCamera; }
	bool Init(ID3D11Device* device,float aspectratio);
	void Draw(ID3D11DeviceContext* dc);
	void Update(float dt,const GameTimer& mtimer);

private:

	Camera *		mCamera;
	Terrain*		mTerrain;
	Sky*			mSky;
	Box*			mBox;
	Tree2D *		mTree2D;
	SnowMan*		mSnowMan;
	SnowMan*		mSnowMan2;
	SnowParticle*	mSnowParticle;

};
#endif // ! SECENFACTORY_H_
