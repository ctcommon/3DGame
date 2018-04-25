#pragma once
#ifndef EFFECTS_H_
#define EFFECTS_H_

#include "Common.h"
#include "d3dx11effect.h"
#include "Dx11Base.h"


#define SetWorldViewProj(TMP,C) {TMP->SetMatrix(reinterpret_cast<const float*>(&C));}
#define SetWor(TMP,C) {TMP->SetMatrix(reinterpret_cast<const float*>(&C));}
#define SetWorldInvTranspose(TMP,C) { TMP->SetMatrix(reinterpret_cast<const float*>(&C)); }
#define SetEyePosW(TMP,C) { TMP->SetRawValue(&C, 0, sizeof(XMFLOAT3)); }
#define SetDirLight(TMP,C,N) { TMP->SetRawValue(&C, 0, N*sizeof(C)); }
#define SetSpotLight(TMP,C,N) {TMP->SetRawValue(&C, 0, N*sizeof(C));}
#define SetTransMatrix(TMP,C) {TMP->SetMatrix(reinterpret_cast<const float*>(&C));}
#define SetMat(TMP,C) { TMP->SetRawValue(&C, 0, sizeof(C)); }
#define SetTex(TMP,C) { TMP->SetResource(C); }



//基类
class Effect
{
public:
	Effect() : pFx(nullptr){}
	virtual ~Effect()
	{
		SafeRelease(pFx);
	}

	//禁止复制
	Effect(const Effect&) = delete;
	Effect& operator=(const Effect&) = delete;

	//通过device和fx文件初始化
	virtual bool Init(ID3D11Device* device, std::wstring fileName);

	ID3DX11Effect* pFx;
};

class BoxEffect : public Effect
{
public:
	BoxEffect():pFxWorldViewProj(nullptr), pFxWorld(nullptr), pFxWorldInvTranspose(nullptr), pFxTexTrans(nullptr), pFxMaterial(nullptr),
		pFxDirLights(nullptr), pFxSpotLights(nullptr), pFxEyePos(nullptr), pFxsecondSR(nullptr),
		pFxSR(nullptr), pFxLight(nullptr), pFxLightTexTech(nullptr), FogColor(nullptr), FogStart(nullptr), FogRange(nullptr)
	{}
	~BoxEffect()
	{
		SafeRelease(FogRange);
		SafeRelease(FogStart);
		SafeRelease(FogColor);
		SafeRelease(pFxLightTexTech);
		SafeRelease(pFxLight);
		SafeRelease(pFxSR);
		SafeRelease(pFxsecondSR);
		SafeRelease(pFxEyePos);
		SafeRelease(pFxSpotLights);
		SafeRelease(pFxDirLights);
		SafeRelease(pFxMaterial);
		SafeRelease(pFxTexTrans);
		SafeRelease(pFxWorldInvTranspose);
		SafeRelease(pFxWorld);
		SafeRelease(pFxWorldViewProj);
	}
	bool Init(ID3D11Device* device, std::wstring fileName);

	ID3DX11EffectMatrixVariable* pFxWorldViewProj;
	ID3DX11EffectMatrixVariable* pFxWorld;
	ID3DX11EffectMatrixVariable* pFxWorldInvTranspose;
	ID3DX11EffectMatrixVariable* pFxTexTrans;
	ID3DX11EffectVariable*	     pFxMaterial;

	ID3DX11EffectVariable*		 pFxDirLights;
	ID3DX11EffectVariable*		 pFxSpotLights;
	ID3DX11EffectVectorVariable* pFxEyePos;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;

	ID3DX11EffectShaderResourceVariable*	pFxSR;
	ID3DX11EffectShaderResourceVariable*	pFxsecondSR;

	ID3DX11EffectTechnique		*pFxLightTexTech;
	ID3DX11EffectTechnique		*pFxLight;
};


class TerrainEffect : public Effect
{
public:
	TerrainEffect() :LightTech(nullptr), LightFogTech(nullptr), ViewProj(nullptr), World(nullptr), WorldInvTranspose(nullptr),
		TexTransform(nullptr), EyePosW(nullptr), FogColor(nullptr), FogStart(nullptr), FogRange(nullptr),
		DirLights(nullptr), Mat(nullptr), MinDist(nullptr), MaxDist(nullptr), MinTess(nullptr), MaxTess(nullptr), TexelCellSpaceU(nullptr),
		TexelCellSpaceV(nullptr), WorldCellSpace(nullptr), WorldFrustumPlanes(nullptr), LayerMapArray(nullptr), BlendMap(nullptr),
		HeightMap(nullptr)
	{}

	~TerrainEffect()
	{
		SafeRelease(HeightMap);
		SafeRelease(BlendMap);
		SafeRelease(LayerMapArray);
		SafeRelease(WorldFrustumPlanes);
		SafeRelease(WorldCellSpace);
		SafeRelease(TexelCellSpaceV);
		SafeRelease(TexelCellSpaceU);
		SafeRelease(MaxTess);
		SafeRelease(MinTess);
		SafeRelease(MaxDist);
		SafeRelease(MinDist);
		SafeRelease(Mat);
		SafeRelease(DirLights);
		SafeRelease(FogRange);
		SafeRelease(FogStart);
		SafeRelease(FogColor);
		SafeRelease(EyePosW);
		SafeRelease(TexTransform);
		SafeRelease(WorldInvTranspose);
		SafeRelease(World);
		SafeRelease(ViewProj);
		SafeRelease(LightFogTech);
		SafeRelease(LightTech);
	}
	bool Init(ID3D11Device* device, std::wstring fileName) override;


	ID3DX11EffectTechnique* LightTech;
	ID3DX11EffectTechnique* LightFogTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectScalarVariable* MinDist;
	ID3DX11EffectScalarVariable* MaxDist;
	ID3DX11EffectScalarVariable* MinTess;
	ID3DX11EffectScalarVariable* MaxTess;
	ID3DX11EffectScalarVariable* TexelCellSpaceU;
	ID3DX11EffectScalarVariable* TexelCellSpaceV;
	ID3DX11EffectScalarVariable* WorldCellSpace;
	ID3DX11EffectVectorVariable* WorldFrustumPlanes;

	ID3DX11EffectShaderResourceVariable* LayerMapArray;
	ID3DX11EffectShaderResourceVariable* BlendMap;
	ID3DX11EffectShaderResourceVariable* HeightMap;


};

class SkyEffect : public Effect
{
public:
	SkyEffect() :SkyTech(nullptr), WorldViewProj(nullptr), CubeMap(nullptr)
	{}

	bool Init(ID3D11Device* device, std::wstring fileName) override;
	~SkyEffect()
	{
		SafeRelease(CubeMap);
		SafeRelease(WorldViewProj);
		SafeRelease(SkyTech);
	}

	ID3DX11EffectTechnique* SkyTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;

	ID3DX11EffectShaderResourceVariable* CubeMap;
};

class TreeEffect : public Effect
{
public:
	TreeEffect() :Light3TexAlphaClipFogTech(nullptr), ViewProj(nullptr), EyePosW(nullptr),
		FogColor(nullptr), FogStart(nullptr), FogRange(nullptr), DirLights(nullptr),
		Mat(nullptr), TreeTextureMapArray(nullptr)
	{

	}

	bool Init(ID3D11Device* device, std::wstring fileName) override;

	~TreeEffect()
	{
		SafeRelease(TreeTextureMapArray);
		SafeRelease(Mat);
		SafeRelease(DirLights);
		SafeRelease(FogRange);
		SafeRelease(FogStart);
		SafeRelease(FogColor);
		SafeRelease(EyePosW);
		SafeRelease(ViewProj);
		SafeRelease(Light3TexAlphaClipFogTech);
	}

	ID3DX11EffectTechnique * Light3TexAlphaClipFogTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectShaderResourceVariable* TreeTextureMapArray;
};

class SnowManEffect : public Effect
{
public:
	SnowManEffect():TexTrans(nullptr), LightTexTech(nullptr), Light(nullptr), EyePosW(nullptr), SpotLight(nullptr),
		PointLight(nullptr), DirLight(nullptr), Tex(nullptr), Material(nullptr), WorldInvTranspose(nullptr),
		World(nullptr), WorldViewProj(nullptr),FogColor(nullptr), FogStart(nullptr), FogRange(nullptr)
	{}
	~SnowManEffect()
	{
		SafeRelease(FogRange);
		SafeRelease(FogStart);
		SafeRelease(FogColor);
		SafeRelease(WorldViewProj);
		SafeRelease(World);
		SafeRelease(WorldInvTranspose);
		SafeRelease(Material);
		SafeRelease(Tex);
		SafeRelease(DirLight);
		SafeRelease(PointLight);
		SafeRelease(SpotLight);
		SafeRelease(EyePosW);
		SafeRelease(Light);
		SafeRelease(LightTexTech);
		SafeRelease(TexTrans);
	}

	bool Init(ID3D11Device* device, std::wstring fileName);



	//供每一个物体进行设置的effect相关变量
	ID3DX11EffectMatrixVariable*	World;
	ID3DX11EffectMatrixVariable*	WorldInvTranspose;
	ID3DX11EffectMatrixVariable*    WorldViewProj;
	ID3DX11EffectMatrixVariable*	TexTrans;
	ID3DX11EffectVariable*			Material;
	ID3DX11EffectShaderResourceVariable* Tex;
	//供每一帧进行设置的effect相关变量
	ID3DX11EffectVariable*			DirLight;
	ID3DX11EffectVariable*			PointLight;
	ID3DX11EffectVariable*			SpotLight;
	ID3DX11EffectVectorVariable*	EyePosW;
	ID3DX11EffectVectorVariable*	FogColor;
	ID3DX11EffectScalarVariable*	FogStart;
	ID3DX11EffectScalarVariable*	FogRange;
	ID3DX11EffectTechnique			*LightTexTech;
	ID3DX11EffectTechnique			*Light;
};

class ParticleEffect : public Effect
{
public:
	ParticleEffect() :StreamOutTech(nullptr), DrawTech(nullptr), ViewProj(nullptr), GameTime(nullptr), TimeStep(nullptr),
		EyePosW(nullptr), EmitPosW(nullptr), EmitDirW(nullptr), TexArray(nullptr), RandomTex(nullptr)
	{}
	~ParticleEffect()
	{
		SafeRelease(RandomTex);
		SafeRelease(TexArray);
		SafeRelease(EmitDirW);
		SafeRelease(EmitPosW);
		SafeRelease(EyePosW);
		SafeRelease(TimeStep);
		SafeRelease(GameTime);
		SafeRelease(ViewProj);
		SafeRelease(DrawTech);
		SafeRelease(StreamOutTech);
	}

	bool Init(ID3D11Device* device, std::wstring fileName) override;

	ID3DX11EffectTechnique* StreamOutTech;
	ID3DX11EffectTechnique* DrawTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectScalarVariable* GameTime;
	ID3DX11EffectScalarVariable* TimeStep;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* EmitPosW;
	ID3DX11EffectVectorVariable* EmitDirW;
	ID3DX11EffectShaderResourceVariable* TexArray;
	ID3DX11EffectShaderResourceVariable* RandomTex;
};

class Effects
{
public:
	static bool InitAll(ID3D11Device* device);
	static void ReleaseAll();

	static BoxEffect* pBoxEffect;
	static TerrainEffect* pTerrainEffect;
	static SkyEffect* pSkyEffect;
	static TreeEffect* pTreeEffect;
	static SnowManEffect* pSnowManEffect;
	static ParticleEffect* pParticleEffect;
};

#endif