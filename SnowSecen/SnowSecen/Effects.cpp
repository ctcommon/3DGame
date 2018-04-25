#include "Effects.h"


bool Effect::Init(ID3D11Device* device, std::wstring fileName)
{
	HRESULT hr;
	ID3DBlob* errorBlob;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG || defined DEBUG
	shaderFlags = D3DCOMPILE_DEBUG;
#endif
	hr = D3DX11CompileEffectFromFile(fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags,
		0, device, &pFx, &errorBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"create shader failed!", L"error", MB_OK);
		return hr;
	}
	return true;
}


bool BoxEffect::Init(ID3D11Device* device, std::wstring fileName)
{
	if (!Effect::Init(device, fileName))
	{
		MessageBox(nullptr, L"create basic shader failed!", L"error", MB_OK);
	}
	pFxWorldViewProj = pFx->GetVariableByName("worldViewProj")->AsMatrix();
	pFxWorld = pFx->GetVariableByName("world")->AsMatrix();
	pFxWorldInvTranspose = pFx->GetVariableByName("worldInvTranspose")->AsMatrix();
	pFxTexTrans = pFx->GetVariableByName("texTrans")->AsMatrix();
	pFxMaterial = pFx->GetVariableByName("material");
	pFxSpotLights = pFx->GetVariableByName("sPotLight");
	pFxDirLights = pFx->GetVariableByName("lights");

	pFxEyePos = pFx->GetVariableByName("eyePos")->AsVector();
	FogColor = pFx->GetVariableByName("gFogColor")->AsVector();
	FogStart = pFx->GetVariableByName("gFogStart")->AsScalar();
	FogRange = pFx->GetVariableByName("gFogRange")->AsScalar();

	pFxSR = pFx->GetVariableByName("gtex")->AsShaderResource();
	pFxsecondSR = pFx->GetVariableByName("secondgtex")->AsShaderResource();

	pFxLightTexTech = pFx->GetTechniqueByName("Light3doubleTex");
	pFxLight = pFx->GetTechniqueByName("Light1");

	return true;
}


bool TerrainEffect::Init(ID3D11Device* device, std::wstring fileName)
{
	if (!Effect::Init(device, fileName))
	{
		MessageBox(nullptr, L"create basic shader failed!", L"error", MB_OK);
		return false;
	}


	//LightFogTech = pFx->GetTechniqueByName("Light1");
	LightFogTech = pFx->GetTechniqueByName("Light2Fog");
	ViewProj = pFx->GetVariableByName("gViewProj")->AsMatrix();
	World = pFx->GetVariableByName("gWorld")->AsMatrix();
	EyePosW = pFx->GetVariableByName("gEyePosW")->AsVector();
	FogColor = pFx->GetVariableByName("gFogColor")->AsVector();
	FogStart = pFx->GetVariableByName("gFogStart")->AsScalar();
	FogRange = pFx->GetVariableByName("gFogRange")->AsScalar();
	DirLights = pFx->GetVariableByName("gDirLights");
	Mat = pFx->GetVariableByName("gMaterial");

	MinDist = pFx->GetVariableByName("gMinDist")->AsScalar();
	MaxDist = pFx->GetVariableByName("gMaxDist")->AsScalar();
	MinTess = pFx->GetVariableByName("gMinTess")->AsScalar();
	MaxTess = pFx->GetVariableByName("gMaxTess")->AsScalar();
	TexelCellSpaceU = pFx->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	TexelCellSpaceV = pFx->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	WorldCellSpace = pFx->GetVariableByName("gWorldCellSpace")->AsScalar();
	WorldFrustumPlanes = pFx->GetVariableByName("gWorldFrustumPlanes")->AsVector();

	LayerMapArray = pFx->GetVariableByName("gLayerMapArray")->AsShaderResource();
	BlendMap = pFx->GetVariableByName("gBlendMap")->AsShaderResource();
	HeightMap = pFx->GetVariableByName("gHeightMap")->AsShaderResource();

	return true;
}

bool SkyEffect::Init(ID3D11Device* device, std::wstring fileName)
{
	if (!Effect::Init(device, fileName))
	{
		MessageBox(nullptr, L"create basic shader failed!", L"error", MB_OK);
	}

	SkyTech = pFx->GetTechniqueByName("SkyTech");
	WorldViewProj = pFx->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap = pFx->GetVariableByName("gCubeMap")->AsShaderResource();
	return true;
}


bool TreeEffect::Init(ID3D11Device* device, std::wstring fileName)
{
	if (!Effect::Init(device, fileName))
	{
		MessageBox(nullptr, L"create basic shader failed!", L"error", MB_OK);
	}
	Light3TexAlphaClipFogTech = pFx->GetTechniqueByName("Light3TexAlphaClipFog");
	ViewProj = pFx->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW = pFx->GetVariableByName("gEyePosW")->AsVector();
	FogColor = pFx->GetVariableByName("gFogColor")->AsVector();
	FogStart = pFx->GetVariableByName("gFogStart")->AsScalar();
	FogRange = pFx->GetVariableByName("gFogRange")->AsScalar();
	DirLights = pFx->GetVariableByName("gDirLights");
	Mat = pFx->GetVariableByName("gMaterial");
	TreeTextureMapArray = pFx->GetVariableByName("gTreeMapArray")->AsShaderResource();


	return true;
}

bool SnowManEffect::Init(ID3D11Device* device, std::wstring fileName)
{
	if (!Effect::Init(device, fileName))
	{
		MessageBox(nullptr, L"create basic shader failed!", L"error", MB_OK);
	}
	LightTexTech = pFx->GetTechniqueByName("LightTech");
	Light = pFx->GetTechniqueByName("Light");
	World = pFx->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = pFx->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	WorldViewProj = pFx->GetVariableByName("gWorldViewProj")->AsMatrix();
	TexTrans = pFx->GetVariableByName("texTrans")->AsMatrix();
	Material = pFx->GetVariableByName("gMaterial");
	DirLight = pFx->GetVariableByName("gDirLight");
	PointLight = pFx->GetVariableByName("gPointLight");
	SpotLight = pFx->GetVariableByName("gSpotLight");
	EyePosW = pFx->GetVariableByName("gEyePosW")->AsVector();
	FogColor = pFx->GetVariableByName("gFogColor")->AsVector();
	FogStart = pFx->GetVariableByName("gFogStart")->AsScalar();
	FogRange = pFx->GetVariableByName("gFogRange")->AsScalar();
	Tex = pFx->GetVariableByName("g_tex")->AsShaderResource();
	return true;
}

bool ParticleEffect::Init(ID3D11Device* device, std::wstring fileName)
{
	if (!Effect::Init(device, fileName))
	{
		MessageBox(nullptr, L"create basic shader failed!", L"error", MB_OK);
		return false;
	}

	StreamOutTech = pFx->GetTechniqueByName("StreamOutTech");
	DrawTech = pFx->GetTechniqueByName("DrawTech");

	ViewProj = pFx->GetVariableByName("gViewProj")->AsMatrix();
	GameTime = pFx->GetVariableByName("gGameTime")->AsScalar();
	TimeStep = pFx->GetVariableByName("gTimeStep")->AsScalar();
	EyePosW = pFx->GetVariableByName("gEyePosW")->AsVector();
	EmitPosW = pFx->GetVariableByName("gEmitPosW")->AsVector();
	EmitDirW = pFx->GetVariableByName("gEmitDirW")->AsVector();
	TexArray = pFx->GetVariableByName("gTexArray")->AsShaderResource();
	RandomTex = pFx->GetVariableByName("gRandomTex")->AsShaderResource();
	return true;
}


BoxEffect* Effects::pBoxEffect(nullptr);
TerrainEffect* Effects::pTerrainEffect(nullptr);
SkyEffect* Effects::pSkyEffect(nullptr);
TreeEffect* Effects::pTreeEffect(nullptr);
SnowManEffect* Effects::pSnowManEffect(nullptr);
ParticleEffect* Effects::pParticleEffect(nullptr);

bool Effects::InitAll(ID3D11Device* device)
{
	if (!pBoxEffect)
	{
		pBoxEffect = new BoxEffect;
		if (!pBoxEffect->Init(device, L"FX//Box.fx"))
		{
			MessageBox(nullptr,L"FX//Box.fx is wrong",L"pBoxEffect" ,MB_OK);
			return false;
		}
	}
	if (!pTerrainEffect)
	{
		pTerrainEffect = new TerrainEffect;
		if (!pTerrainEffect->Init(device, L"FX//terrain.fx"))
		{
			MessageBox(nullptr, L"FX//terrain.fx is wrong", L"pTerrainEffect", MB_OK);
			return false;
		}
	}
	if (!pSkyEffect)
	{
		pSkyEffect = new SkyEffect;
		if (!pSkyEffect->Init(device, L"FX//Sky.fx"))
		{
			MessageBox(nullptr, L"FX//Sky.fx", L"pSkyEffect", MB_OK);
			return false;
		}
	}
	if (!pTreeEffect)
	{
		pTreeEffect = new TreeEffect;
		if (!pTreeEffect->Init(device, L"FX//tree.fx"))
		{
			MessageBox(nullptr, L"FX//tree.fx", L"pTreeEffect", MB_OK);
			return false;
		}
	}
	if (!pSnowManEffect)
	{
		pSnowManEffect = new SnowManEffect;
		if (!pSnowManEffect->Init(device, L"FX//snowman.fx"))
		{
			MessageBox(nullptr, L"FX//snowman.fx", L"pSnowManEffect", MB_OK);
			return false;
		}
	}
	if (!pParticleEffect)
	{
		pParticleEffect = new ParticleEffect;
		if (!pParticleEffect->Init(device, L"FX//SnowParticle.fx"))
		{
			MessageBox(nullptr, L"FX//SnowParticle.fx", L"pParticleEffect", MB_OK);
			return false;
		}
	}
	return true;
}

void Effects::ReleaseAll()
{
	if (pBoxEffect)
	{
		delete pBoxEffect;
		pBoxEffect = nullptr;
	}
	if (pTerrainEffect)
	{
		delete pTerrainEffect;
		pTerrainEffect = nullptr;
	}
	if (pSkyEffect)
	{
		delete pSkyEffect;
		pSkyEffect = nullptr;
	}
	if (pTreeEffect)
	{
		delete pTreeEffect;
		pTreeEffect = nullptr;
	}
	if (pSnowManEffect)
	{
		delete pSnowManEffect;
		pSnowManEffect = nullptr;
	}

	if (pParticleEffect)
	{
		delete pParticleEffect;
		pParticleEffect = nullptr;
	}
}