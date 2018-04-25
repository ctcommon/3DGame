/*****************
//三点式照明
****************/

#include "LightHelper.fx"

//针对每个物体
cbuffer PerObject
{
    float4x4 worldViewProj;       //世界 视角 投影矩阵
    float4x4 world;               //世界矩阵
    float4x4 worldInvTranspose;   //世界矩阵逆矩阵的转置，用于调整法线
    Material material;            //材质
    float4x4 texTrans;            //纹理变换
};

//针对每一帧
cbuffer PerFrame
{
    DirectionalLight lights[3]; //平行光数组
	SpotLight        sPotLight;    //聚光
   	float3 eyePos; //观察点

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

//纹理
Texture2D gtex;
Texture2D secondgtex;

SamplerState samplerTex
{
   	Filter = ANISOTROPIC;
	AddressU = WRAP;
	MaxAnisotropy = 4;
};

struct VertexIn
{
    float3 pos      : POSITION;     //顶点坐标
    float3 normal   : NORMAL;        //顶点法线
    float2 tex      : TEXCOORD;      //纹理坐标
};

struct VertexOut
{
    float3 posTrans : POSITION; //世界变换后的坐标，乘以world矩阵
    float4 posH : SV_Position;   //投影后的坐标
    float3 normal : NORMAL;      //世界变换后的顶点法线
    float2 tex : TEXCOORD;       //转换后的纹理坐标
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    //世界变换后的顶点坐标
    vout.posTrans = mul(float4(vin.pos, 1.0f), world).xyz;
    //投影变换后的顶点坐标
    vout.posH = mul(float4(vin.pos, 1.0f), worldViewProj);
    //变换后的法线坐标
    vout.normal = mul(float4(vin.normal, 1.0f), worldInvTranspose).xyz;
    //变换后的纹理坐标
    vout.tex = mul(float4(vin.tex, 0.0f, 1.0f), texTrans).xy;

    return vout;
}

float4 PS(VertexOut pin, uniform int numLights, uniform bool usetex,uniform bool secondTexture):SV_Target
{
	float3 normal = normalize(pin.normal);
	float3 toEye = eyePos - pin.posTrans;
	float distToEye = length(toEye);
	toEye /= distToEye;
    //初始颜色为白色
    float4 texColor = float4(1.f, 1.f, 1.f, 1.f);
    //使用纹理则获得相应颜色值
    if(usetex == true)
    {
    	texColor = gtex.Sample(samplerTex, pin.tex);
         //使用多重纹理
    	if(secondTexture == true)
    	{
		texColor *= secondgtex.Sample(samplerTex, pin.tex);
     	}
    }
    float4 litColor = texColor;
    
    //开始光照计算
    if(numLights > 0)
    {
        float4 A = float4(0.f, 0.f, 0.f, 0.f);
        float4 D = float4(0.f, 0.f, 0.f, 0.f);
        float4 S = float4(0.f, 0.f, 0.f, 0.f);

		float4 ambient, diffuse, specular;
        [unroll]
        for (int i = 0; i < numLights; ++i)
        {
            ComputeDirectionalLight(material, lights[i], normal,toEye,ambient, diffuse, specular);
            A += ambient;
            D += diffuse;
            S += specular;
        }
		ComputeSpotLight(material,sPotLight,pin.posTrans,normal,toEye,ambient,diffuse,specular);
		A += ambient;
        D += diffuse;
        S += specular;
        //纹理+光照计算公式： 纹理*(环境光+漫反射光)+高光
        litColor = texColor * (A + D) + S;
    }

	float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogLerp);
    //透明度 纹理透明度*材质漫反射透明度
    litColor.a = material.diffuse.a * texColor.a;

    return litColor;
}

//不使用纹理
technique11 Light1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS(1, false,false)));
    }
}

technique11 Light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS(3, false,false)));
    }
}

//单纹理
technique11 Light1Tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS(1, true,false)));
    }
}

technique11 Light3Tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS(3, true,false)));
    }
}

//多重纹理
technique11 Light1doubleTex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS(1, true,true)));
    }
}

technique11 Light3doubleTex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS(3, true,true)));
    }
}