cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
};

TextureCube gCubeMap;

SamplerState samTexture
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 posL: POSITION;
};

struct VertexOut
{
	float4 posH: SV_POSITION;
	float3 posL: POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.posL = vin.posL;
	vout.posH = mul(float4(vin.posL, 1.f), gWorldViewProj).xyww;

	return vout;
}

float4 PS(VertexOut pin) : SV_TARGET
{
	return gCubeMap.Sample(samTexture,pin.posL);
}

RasterizerState NoCull
{
	CullMode = None;
};

DepthStencilState LessEqualDSS
{
	DepthFunc = LESS_EQUAL;
};

technique11 SkyTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
		SetDepthStencilState(LessEqualDSS, 0);
		SetRasterizerState(NoCull);
	}
}