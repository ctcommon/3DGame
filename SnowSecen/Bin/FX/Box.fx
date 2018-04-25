/*****************
//����ʽ����
****************/

#include "LightHelper.fx"

//���ÿ������
cbuffer PerObject
{
    float4x4 worldViewProj;       //���� �ӽ� ͶӰ����
    float4x4 world;               //�������
    float4x4 worldInvTranspose;   //�������������ת�ã����ڵ�������
    Material material;            //����
    float4x4 texTrans;            //����任
};

//���ÿһ֡
cbuffer PerFrame
{
    DirectionalLight lights[3]; //ƽ�й�����
	SpotLight        sPotLight;    //�۹�
   	float3 eyePos; //�۲��

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

//����
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
    float3 pos      : POSITION;     //��������
    float3 normal   : NORMAL;        //���㷨��
    float2 tex      : TEXCOORD;      //��������
};

struct VertexOut
{
    float3 posTrans : POSITION; //����任������꣬����world����
    float4 posH : SV_Position;   //ͶӰ�������
    float3 normal : NORMAL;      //����任��Ķ��㷨��
    float2 tex : TEXCOORD;       //ת�������������
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    //����任��Ķ�������
    vout.posTrans = mul(float4(vin.pos, 1.0f), world).xyz;
    //ͶӰ�任��Ķ�������
    vout.posH = mul(float4(vin.pos, 1.0f), worldViewProj);
    //�任��ķ�������
    vout.normal = mul(float4(vin.normal, 1.0f), worldInvTranspose).xyz;
    //�任�����������
    vout.tex = mul(float4(vin.tex, 0.0f, 1.0f), texTrans).xy;

    return vout;
}

float4 PS(VertexOut pin, uniform int numLights, uniform bool usetex,uniform bool secondTexture):SV_Target
{
	float3 normal = normalize(pin.normal);
	float3 toEye = eyePos - pin.posTrans;
	float distToEye = length(toEye);
	toEye /= distToEye;
    //��ʼ��ɫΪ��ɫ
    float4 texColor = float4(1.f, 1.f, 1.f, 1.f);
    //ʹ������������Ӧ��ɫֵ
    if(usetex == true)
    {
    	texColor = gtex.Sample(samplerTex, pin.tex);
         //ʹ�ö�������
    	if(secondTexture == true)
    	{
		texColor *= secondgtex.Sample(samplerTex, pin.tex);
     	}
    }
    float4 litColor = texColor;
    
    //��ʼ���ռ���
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
        //����+���ռ��㹫ʽ�� ����*(������+�������)+�߹�
        litColor = texColor * (A + D) + S;
    }

	float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogLerp);
    //͸���� ����͸����*����������͸����
    litColor.a = material.diffuse.a * texColor.a;

    return litColor;
}

//��ʹ������
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

//������
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

//��������
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