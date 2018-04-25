#pragma once
#ifndef COMMON_H_
#define COMMON_H_

#include <DirectXMath.h>
#include <vector>
#include <d3d11.h>
#include <windows.h>
#include "debug.h"

using namespace DirectX;

namespace Vertex
{
	struct BaseVertice
	{
		BaseVertice() :pos(0.0f, 0.0f, 0.0f) {}
		XMFLOAT3 pos;
	};
	struct SizeVertice :public BaseVertice
	{
		SizeVertice() :size(0.0f, 0.0f) {}
		XMFLOAT2 size;
	};
	struct ColoarVertice :public BaseVertice
	{
		ColoarVertice() :corlor(0.0f, 0.0f, 0.0f) {}
		XMFLOAT3 corlor;
	};

	struct TerrainVertice :public BaseVertice
	{
		TerrainVertice():tex(0.0f,0.0f), BoundsY(0.0f,0.0f){}
		XMFLOAT2 tex;
		XMFLOAT2 BoundsY;
	};

	struct NormalTexVertice :public BaseVertice
	{
		NormalTexVertice() :normal(0.0f, 0.0f,0.0f), tex(0.0f, 0.0f) {}
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};

	struct Particle : public BaseVertice
	{
		Particle():InitialVel(0.0f,0.0f,0.0f), Size(0.0f,0.0f),Age(0.0f), Type(0){}
		XMFLOAT3 InitialVel;
		XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	};

	struct NormalTexTanVertice : public NormalTexVertice
	{
		NormalTexTanVertice() :tangentu(0.0f, 0.0f, 0.0f) {}
		XMFLOAT3 tangentu;
	};
}

namespace InputLayoutDesc
{
	const D3D11_INPUT_ELEMENT_DESC Pos[1] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const D3D11_INPUT_ELEMENT_DESC TreePointSprite[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	const D3D11_INPUT_ELEMENT_DESC NormalTex[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const D3D11_INPUT_ELEMENT_DESC Terrain[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const D3D11_INPUT_ELEMENT_DESC NormalTexTan[4] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const D3D11_INPUT_ELEMENT_DESC Particle[5] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
}

namespace Lights {
	//ƽ�й�
	struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 ambient;	//������
		XMFLOAT4 diffuse;	//�������
		XMFLOAT4 specular;	//�߹�
		XMFLOAT3 direction;	//���շ���
		float pad;			//������HLSL�С�4D�������������ƥ��
	};
	//���Դ
	struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;

		// Packed into 4D vector: (Position, Range)
		XMFLOAT3 position;//��Դλ��
		float range;      //���շ�Χ

						  // Packed into 4D vector: (A0, A1, A2, Pad)
		XMFLOAT3 att;     //˥��ϵ��
		float pad; // Pad the last float so we can set an array of lights if we wanted.
	};
	//�۹��
	struct SpotLight
	{
		SpotLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;

		// Packed into 4D vector: (Position, Range)
		XMFLOAT3 position;//����λ��
		float range;      //���շ�Χ

						  // Packed into 4D vector: (Direction, Spot)
		XMFLOAT3 direction;//���շ���
		float spot;        //����ǿ��ϵ��   

						   // Packed into 4D vector: (Att, Pad)
		XMFLOAT3 att;      //˥��ϵ��
		float pad; // Pad the last float so we can set an array of lights if we wanted.
	};
	//����
	struct Material
	{
		Material() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;//w��ʾ�߹�ǿ��
		XMFLOAT4 reflect;
	};

}


#endif // !COMMON_H_

