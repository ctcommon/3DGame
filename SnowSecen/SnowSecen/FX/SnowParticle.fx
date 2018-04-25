//=============================================================================
// Rain.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Rain particle system.  Particles are emitted directly in world space.
//=============================================================================


//***********************************************
// GLOBALS                                      *
//***********************************************

cbuffer cbPerFrame
{
	float3 gEyePosW;
	
	// for when the emit position/direction is varying
	float3 gEmitPosW;
	float3 gEmitDirW;
	
	float gGameTime;
	float gTimeStep;
	float4x4 gViewProj; 
};
 
// Array of textures for texturing the particles.
Texture2DArray gTexArray;

// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex;
 
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};


//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	// project onto unit sphere
	return normalize(v);
}

float3 RandVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	return v;
}
 
//***********************************************
// STREAM-OUT TECH                              *
//***********************************************

#define PT_EMITTER 0
#define PT_FLARE 1
 
struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age          : AGE;
	uint Type          : TYPE;
};
  
Particle StreamOutVS(Particle vin)
{
	return vin;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.
[maxvertexcount(6)]
void StreamOutGS(point Particle gin[1], 
                 inout PointStream<Particle> ptStream)
{	
	gin[0].Age += gTimeStep;

	if( gin[0].Type == PT_EMITTER )
	{	
		// time to emit a new particle?
		if( gin[0].Age > 0.00001f )
		{
			for(int i = 0; i < 1; ++i)
			{
				// Spread rain drops out above the camera.
				float3 vRandom = 100.0f*RandVec3((float)i / 5.f);
				vRandom.y = 60.0f;
			
				Particle p;
				p.InitialPosW = gEmitPosW.xyz + vRandom;
				p.InitialVelW = float3(0.0f, -0.1f, 0.0f);
				p.SizeW       = float2(1.0f, 1.0f);
				p.Age         = 0.0f;
				p.Type        = PT_FLARE;
			
				ptStream.Append(p);
			}
			
			// reset the time to emit
			gin[0].Age = 0.0f;
		}

		// always keep emitters
		ptStream.Append(gin[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if( gin[0].Age <= 33.0f )
			ptStream.Append(gin[0]);
	}		
}

GeometryShader gsStreamOut = ConstructGSWithSO( 
	CompileShader( gs_5_0, StreamOutGS() ), 
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x" );
	
technique11 StreamOutTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, StreamOutVS() ) );
        SetGeometryShader( gsStreamOut );
        
        // disable pixel shader for stream-out only
        SetPixelShader(NULL);
        
        // we must also disable the depth buffer for stream-out only
        SetDepthStencilState( DisableDepth, 0 );
    }
}

//***********************************************
// DRAW TECH                                    *
//***********************************************

struct VertexOut
{
	float3 PosW  : POSITION;
	uint   Type  : TYPE;
};

VertexOut DrawVS(Particle vin)
{
	VertexOut vout;
	
	float t = vin.Age;
	float3 gAccelW = { -0.01f, -0.2f, 0.0f };
	//float3 vRandom = RandUnitVec3(t);
	// constant acceleration equation
	vout.PosW = 0.5f*t*t*gAccelW + t*vin.InitialVelW + vin.InitialPosW;
	vout.PosW.x += 2.0f * cos(0.4f * t) * sin(0.6 * t);
	vout.PosW.z += 1.9f * sin(0.5f * t) * cos(0.8 * t);
	vout.Type  = vin.Type;
	
	return vout;
}

struct GeoOut
{
	float4 PosH  : SV_Position;
	float2 Tex   : TEXCOORD;
};

// The draw GS just expands points into snows.
[maxvertexcount(54)]
void DrawGS(point VertexOut gin[1], 
            inout TriangleStream<GeoOut> snowStream)
{	
	// do not draw emitter particles.
	if( gin[0].Type != PT_EMITTER )
	{
		float PI = 3.1415926f;
		float r = 0.22f;
		float theta = PI / 13.f;
		// Slant line in acceleration direction.
		float3 p0 = gin[0].PosW;

		float3 p1 = gin[0].PosW + r * float3(cos(PI / 2 + theta), sin(PI / 2 + theta), 0);
		float3 p2 = gin[0].PosW + r * float3(cos(PI / 2 - theta), sin(PI / 2 - theta), 0);
		float3 p3 = gin[0].PosW + r * float3(cos(PI / 6 + theta), sin(PI / 6 + theta), 0);
		float3 p4 = gin[0].PosW + r * float3(cos(PI / 6 - theta), sin(PI / 6 - theta), 0);
		float3 p5 = gin[0].PosW + r * float3(cos(-PI / 6 + theta), sin(-PI / 6 + theta), 0);
		float3 p6 = gin[0].PosW + r * float3(cos(-PI / 6 - theta), sin(-PI / 6 - theta), 0);
		float3 p7 = gin[0].PosW + r * float3(cos(-PI / 2 + theta), sin(-PI / 2 + theta), 0);
		float3 p8 = gin[0].PosW + r * float3(cos(-PI / 2 - theta), sin(-PI / 2 - theta), 0);
		float3 p9 = gin[0].PosW + r * float3(cos(-5 * PI / 6 + theta), sin(-5 * PI / 6 + theta), 0);
		float3 p10 = gin[0].PosW + r * float3(cos(-5 * PI / 6 - theta), sin(-5 * PI / 6 - theta), 0);
		float3 p11 = gin[0].PosW + r * float3(cos(5 * PI / 6 + theta), sin(5 * PI / 6 + theta), 0);
		float3 p12 = gin[0].PosW + r * float3(cos(5 * PI / 6 - theta), sin(5 * PI / 6 - theta), 0);

		float3 pp1 = gin[0].PosW + r * float3(cos(PI / 2 + theta), 0, sin(PI / 2 + theta));
		float3 pp2 = gin[0].PosW + r * float3(cos(PI / 2 - theta), 0, sin(PI / 2 - theta));
		float3 pp3 = gin[0].PosW + r * float3(cos(PI / 6 + theta), 0, sin(PI / 6 + theta));
		float3 pp4 = gin[0].PosW + r * float3(cos(PI / 6 - theta), 0, sin(PI / 6 - theta));
		float3 pp5 = gin[0].PosW + r * float3(cos(-PI / 6 + theta), 0, sin(-PI / 6 + theta));
		float3 pp6 = gin[0].PosW + r * float3(cos(-PI / 6 - theta), 0, sin(-PI / 6 - theta));
		float3 pp7 = gin[0].PosW + r * float3(cos(-PI / 2 + theta), 0, sin(-PI / 2 + theta));
		float3 pp8 = gin[0].PosW + r * float3(cos(-PI / 2 - theta), 0, sin(-PI / 2 - theta));
		float3 pp9 = gin[0].PosW + r * float3(cos(-5 * PI / 6 + theta), 0, sin(-5 * PI / 6 + theta));
		float3 pp10 = gin[0].PosW + r * float3(cos(-5 * PI / 6 - theta), 0, sin(-5 * PI / 6 - theta));
		float3 pp11 = gin[0].PosW + r * float3(cos(5 * PI / 6 + theta), 0, sin(5 * PI / 6 + theta));
		float3 pp12 = gin[0].PosW + r * float3(cos(5 * PI / 6 - theta), 0, sin(5 * PI / 6 - theta));

		float3 ppp1 = gin[0].PosW + r * float3(0, cos(PI / 2 + theta), sin(PI / 2 + theta));
		float3 ppp2 = gin[0].PosW + r * float3(0, cos(PI / 2 - theta), sin(PI / 2 - theta));
		float3 ppp3 = gin[0].PosW + r * float3(0, cos(PI / 6 + theta), sin(PI / 6 + theta));
		float3 ppp4 = gin[0].PosW + r * float3(0, cos(PI / 6 - theta), sin(PI / 6 - theta));
		float3 ppp5 = gin[0].PosW + r * float3(0, cos(-PI / 6 + theta), sin(-PI / 6 + theta));
		float3 ppp6 = gin[0].PosW + r * float3(0, cos(-PI / 6 - theta), sin(-PI / 6 - theta));
		float3 ppp7 = gin[0].PosW + r * float3(0, cos(-PI / 2 + theta), sin(-PI / 2 + theta));
		float3 ppp8 = gin[0].PosW + r * float3(0, cos(-PI / 2 - theta), sin(-PI / 2 - theta));
		float3 ppp9 = gin[0].PosW + r * float3(0, cos(-5 * PI / 6 + theta), sin(-5 * PI / 6 + theta));
		float3 ppp10 = gin[0].PosW + r * float3(0, cos(-5 * PI / 6 - theta), sin(-5 * PI / 6 - theta));
		float3 ppp11 = gin[0].PosW + r * float3(0, cos(5 * PI / 6 + theta), sin(5 * PI / 6 + theta));
		float3 ppp12 = gin[0].PosW + r * float3(0, cos(5 * PI / 6 - theta), sin(5 * PI / 6 - theta));
		
		GeoOut v0;
		v0.PosH = mul(float4(p0, 1.0f), gViewProj);
		v0.Tex = float2(0.0f, 0.0f);
		
		GeoOut v1;
		v1.PosH = mul(float4(p1, 1.0f), gViewProj);
		v1.Tex  = float2(1.0f, 1.0f);

		GeoOut v2;
		v2.PosH = mul(float4(p2, 1.0f), gViewProj);
		v2.Tex = float2(1.0f, 1.0f);

		GeoOut v3;
		v3.PosH = mul(float4(p3, 1.0f), gViewProj);
		v3.Tex = float2(1.0f, 1.0f);

		GeoOut v4;
		v4.PosH = mul(float4(p4, 1.0f), gViewProj);
		v4.Tex = float2(1.0f, 1.0f);

		GeoOut v5;
		v5.PosH = mul(float4(p5, 1.0f), gViewProj);
		v5.Tex = float2(1.0f, 1.0f);

		GeoOut v6;
		v6.PosH = mul(float4(p6, 1.0f), gViewProj);
		v6.Tex = float2(1.0f, 1.0f);

		GeoOut v7;
		v7.PosH = mul(float4(p7, 1.0f), gViewProj);
		v7.Tex = float2(1.0f, 1.0f);

		GeoOut v8;
		v8.PosH = mul(float4(p8, 1.0f), gViewProj);
		v8.Tex = float2(1.0f, 1.0f);

		GeoOut v9;
		v9.PosH = mul(float4(p9, 1.0f), gViewProj);
		v9.Tex = float2(1.0f, 1.0f);

		GeoOut v10;
		v10.PosH = mul(float4(p10, 1.0f), gViewProj);
		v10.Tex = float2(1.0f, 1.0f);

		GeoOut v11;
		v11.PosH = mul(float4(p11, 1.0f), gViewProj);
		v11.Tex = float2(1.0f, 1.0f);

		GeoOut v12;
		v12.PosH = mul(float4(p12, 1.0f), gViewProj);
		v12.Tex = float2(1.0f, 1.0f);

		GeoOut vv1;
		vv1.PosH = mul(float4(pp1, 1.0f), gViewProj);
		vv1.Tex = float2(1.0f, 1.0f);

		GeoOut vv2;
		vv2.PosH = mul(float4(pp2, 1.0f), gViewProj);
		vv2.Tex = float2(1.0f, 1.0f);

		GeoOut vv3;
		vv3.PosH = mul(float4(pp3, 1.0f), gViewProj);
		vv3.Tex = float2(1.0f, 1.0f);

		GeoOut vv4;
		vv4.PosH = mul(float4(pp4, 1.0f), gViewProj);
		vv4.Tex = float2(1.0f, 1.0f);

		GeoOut vv5;
		vv5.PosH = mul(float4(pp5, 1.0f), gViewProj);
		vv5.Tex = float2(1.0f, 1.0f);

		GeoOut vv6;
		vv6.PosH = mul(float4(pp6, 1.0f), gViewProj);
		vv6.Tex = float2(1.0f, 1.0f);

		GeoOut vv7;
		vv7.PosH = mul(float4(pp7, 1.0f), gViewProj);
		vv7.Tex = float2(1.0f, 1.0f);

		GeoOut vv8;
		vv8.PosH = mul(float4(pp8, 1.0f), gViewProj);
		vv8.Tex = float2(1.0f, 1.0f);

		GeoOut vv9;
		vv9.PosH = mul(float4(pp9, 1.0f), gViewProj);
		vv9.Tex = float2(1.0f, 1.0f);

		GeoOut vv10;
		vv10.PosH = mul(float4(pp10, 1.0f), gViewProj);
		vv10.Tex = float2(1.0f, 1.0f);

		GeoOut vv11;
		vv11.PosH = mul(float4(pp11, 1.0f), gViewProj);
		vv11.Tex = float2(1.0f, 1.0f);

		GeoOut vv12;
		vv12.PosH = mul(float4(pp12, 1.0f), gViewProj);
		vv12.Tex = float2(1.0f, 1.0f);

		GeoOut vvv1;
		vvv1.PosH = mul(float4(ppp1, 1.0f), gViewProj);
		vvv1.Tex = float2(1.0f, 1.0f);

		GeoOut vvv2;
		vvv2.PosH = mul(float4(ppp2, 1.0f), gViewProj);
		vvv2.Tex = float2(1.0f, 1.0f);

		GeoOut vvv3;
		vvv3.PosH = mul(float4(ppp3, 1.0f), gViewProj);
		vvv3.Tex = float2(1.0f, 1.0f);

		GeoOut vvv4;
		vvv4.PosH = mul(float4(ppp4, 1.0f), gViewProj);
		vvv4.Tex = float2(1.0f, 1.0f);

		GeoOut vvv5;
		vvv5.PosH = mul(float4(ppp5, 1.0f), gViewProj);
		vvv5.Tex = float2(1.0f, 1.0f);

		GeoOut vvv6;
		vvv6.PosH = mul(float4(ppp6, 1.0f), gViewProj);
		vvv6.Tex = float2(1.0f, 1.0f);

		GeoOut vvv7;
		vvv7.PosH = mul(float4(ppp7, 1.0f), gViewProj);
		vvv7.Tex = float2(1.0f, 1.0f);

		GeoOut vvv8;
		vvv8.PosH = mul(float4(ppp8, 1.0f), gViewProj);
		vvv8.Tex = float2(1.0f, 1.0f);

		GeoOut vvv9;
		vvv9.PosH = mul(float4(ppp9, 1.0f), gViewProj);
		vvv9.Tex = float2(1.0f, 1.0f);

		GeoOut vvv10;
		vvv10.PosH = mul(float4(ppp10, 1.0f), gViewProj);
		vvv10.Tex = float2(1.0f, 1.0f);

		GeoOut vvv11;
		vvv11.PosH = mul(float4(ppp11, 1.0f), gViewProj);
		vvv11.Tex = float2(1.0f, 1.0f);

		GeoOut vvv12;
		vvv12.PosH = mul(float4(ppp12, 1.0f), gViewProj);
		vvv12.Tex = float2(1.0f, 1.0f);

		snowStream.Append(v0);
		snowStream.Append(v1);
		snowStream.Append(v2);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(v3);
		snowStream.Append(v4);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(v5);
		snowStream.Append(v6);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(v7);
		snowStream.Append(v8);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(v9);
		snowStream.Append(v10);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(v11);
		snowStream.Append(v12);
		snowStream.RestartStrip();

		snowStream.Append(v0);
		snowStream.Append(vv1);
		snowStream.Append(vv2);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vv3);
		snowStream.Append(vv4);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vv5);
		snowStream.Append(vv6);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vv7);
		snowStream.Append(vv8);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vv9);
		snowStream.Append(vv10);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vv11);
		snowStream.Append(vv12);
		snowStream.RestartStrip();

		snowStream.Append(v0);
		snowStream.Append(vvv1);
		snowStream.Append(vvv2);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vvv3);
		snowStream.Append(vvv4);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vvv5);
		snowStream.Append(vvv6);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vvv7);
		snowStream.Append(vvv8);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vvv9);
		snowStream.Append(vvv10);
		snowStream.RestartStrip();
		snowStream.Append(v0);
		snowStream.Append(vvv11);
		snowStream.Append(vvv12);
		snowStream.RestartStrip();
	}
}

float4 DrawPS(GeoOut pin) : SV_TARGET
{
	return gTexArray.Sample(samLinear, float3(pin.Tex, 0)) + float4(0.8f, 0.8f, 0.8f, 1.f);
}

technique11 DrawTech
{
    pass P0
    {
        SetVertexShader(   CompileShader( vs_5_0, DrawVS() ) );
        SetGeometryShader( CompileShader( gs_5_0, DrawGS() ) );
        SetPixelShader(    CompileShader( ps_5_0, DrawPS() ) );
        
        SetDepthStencilState( NoDepthWrites, 0 );
    }
}