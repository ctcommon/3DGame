#pragma once
#ifndef SECENBASE_H_
#define SECENBASE_H_

#include "Common.h"
#include <d3dcompiler.h>
#include "GeometryGenerator.h"
#include "Effects.h"
#include "DDSTextureLoader.h"
#include "debug.h"

class SecenBase
{
public:
	SecenBase(ID3D11Device* device, CXMMATRIX world);
	virtual ~SecenBase()=0;

	SecenBase(const SecenBase& rhs) = delete;
	SecenBase& operator=(const SecenBase& rhs) = delete;

public:


	bool Init();

	virtual void Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj);

	XMMATRIX GetWorld()const { return XMLoadFloat4x4(&mWorld); }
	XMMATRIX SetWorld(CXMMATRIX world) { XMStoreFloat4x4(&mWorld, world); }
	UINT GetVerticeCount() const { return VerticeCount; }
	UINT GetIndiceCount() const { return IndiceCount; }

protected:

	void Render(ID3DX11EffectTechnique* tech, ID3D11DeviceContext* dc, CXMMATRIX viewproj, UINT inum, UINT istart, UINT vstart)
	{
		D3DX11_TECHNIQUE_DESC desc;
		tech->GetDesc(&desc);
		for (UINT i = 0; i < desc.Passes; ++i)
		{
			SetEffect(viewproj);
			tech->GetPassByIndex(i)->Apply(0, dc);
			dc->DrawIndexed(inum, istart, vstart);
		}
	}
	virtual void SetMaterial();
	virtual void SetEffect(CXMMATRIX vieproj);
	void SetLights();
	//������������������
	virtual bool BuildBuffers();

	template<typename T> bool BuildBuffers_(std::vector<T>& vertices, std::vector<UINT>& indices) {
		VerticeCount = vertices.size();
		IndiceCount = indices.size();

		//��������
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(T) * VerticeCount;
		vertexDesc.CPUAccessFlags = 0;

		//���ݶ������������ݴ������㻺��
		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = &vertices[0];
		HR(Device->CreateBuffer(&vertexDesc, &resourceData, &mVerticeBuffer),L"CreateBuffer");

		//��������
		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = sizeof(UINT) * IndiceCount;
		indexDesc.CPUAccessFlags = 0;

		//�����������ݺ���������������������
		D3D11_SUBRESOURCE_DATA indexData;
		ZeroMemory(&indexData, sizeof(indexData));
		indexData.pSysMem = &indices[0];
		HR(Device->CreateBuffer(&indexDesc, &indexData, &mIndiceBuffer), L"CreateBuffer");
		return true;
	}
	//����shaderResourceView
	virtual bool BuildSRVs();
	//����input Layout
	virtual bool BuildInputLayouts();

	bool BuildInputLayouts_(const D3D11_INPUT_ELEMENT_DESC input[], ID3DX11EffectTechnique* tech, UINT n)
	{

		D3DX11_PASS_DESC passDesc;
		tech->GetPassByIndex(0)->GetDesc(&passDesc);

		HR(Device->CreateInputLayout(input, n, passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize, &mInputLayout), L"CreateInputLayout");

		return true;
	}

	ID3D11Device * Device;  //�豸
	ID3D11Buffer * mVerticeBuffer; //���㻺����
	ID3D11Buffer * mIndiceBuffer;  //����������
	ID3D11InputLayout *mInputLayout;  //����

	XMFLOAT4X4	mWorld;  //��ò�Ҫ��XMMATRIX

	Lights::DirectionalLight	mDirLights[3];
	Lights::SpotLight			mSpotLight;

	UINT	VerticeCount;
	UINT	IndiceCount;
};
#endif // !SECENBASE_H_
