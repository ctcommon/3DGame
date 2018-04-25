#include "Box.h"
#include <ctime>
Box::Box(ID3D11Device* device, CXMMATRIX world, float height, float width, float depth) :
	SecenBase(device, world), pSRVBox(nullptr), pSecondSRVBox(nullptr), BoxHeight(height),
	BoxWidth(width), BoxDepth(depth)
{
	GeometryGenerator::GetInstance()->CreateBox(width, height, depth, box);
	SetMatrix();
	SetMaterial();
	RenderState();
}

Box::~Box()
{
	SafeRelease(pSecondSRVBox);
	SafeRelease(pSRVBox);
}

void Box::SetMatrix()
{
	XMStoreFloat4x4(&texTransbox, XMMatrixIdentity());
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMVECTOR det = XMMatrixDeterminant(world);
	XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det, world));
	XMStoreFloat4x4(&worldInvTrans, worldInvTranspose);
}

void Box::SetMaterial()
{
	materialBox.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	materialBox.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.0f);
	materialBox.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);

	materialShadow.ambient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	materialShadow.diffuse = XMFLOAT4(0.f, 0.f, 0.f, 0.5f);
	materialShadow.specular = XMFLOAT4(0.f, 0.f, 0.f, 16.f);
}

bool Box::BuildBuffers()
{
	VerticeCount = static_cast<UINT>(box.vertices.size());
	IndiceCount = static_cast<UINT>(box.indices.size());
	std::vector<Vertex::NormalTexVertice> vertices(VerticeCount);
	for (UINT i = 0; i < box.vertices.size(); ++i)
	{
		vertices[i].pos = box.vertices[i].pos;
		vertices[i].normal = box.vertices[i].normal;
		vertices[i].tex = box.vertices[i].tex;
	}


	std::vector<UINT> indices(IndiceCount);
	indices.assign(box.indices.begin(), box.indices.end());

	return BuildBuffers_(vertices, indices);
}

bool Box::BuildSRVs()
{
	HR(CreateDDSTextureFromFile(Device, L"Textures//flare.dds", nullptr, &pSRVBox),L"CreateDDSTextureFromFile");
	HR(CreateDDSTextureFromFile(Device, L"Textures//flarealpha.dds", nullptr, &pSecondSRVBox),L"CreateDDSTextureFromFile");
	return true;
}

void Box::SetEffect(CXMMATRIX vieproj) //用宏还是不用宏？
{
	XMFLOAT4X4 worldViewProjbox;
	XMStoreFloat4x4(&worldViewProjbox, XMLoadFloat4x4(&mWorld)*vieproj);
	Effects::pBoxEffect->pFxWorld->SetMatrix(reinterpret_cast<const float*>(&mWorld));
	Effects::pBoxEffect->pFxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProjbox));
	Effects::pBoxEffect->pFxWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&worldInvTrans));
	Effects::pBoxEffect->pFxMaterial->SetRawValue(&materialBox, 0, sizeof(materialBox));
	Effects::pBoxEffect->pFxTexTrans->SetMatrix(reinterpret_cast<const float*>(&texTransbox));
	Effects::pBoxEffect->FogColor->SetFloatVector(reinterpret_cast<const float*>(&Colors::Silver));
	Effects::pBoxEffect->FogStart->SetFloat(200.0f);
	Effects::pBoxEffect->FogRange->SetFloat(500.0f);
	Effects::pBoxEffect->pFxSR->SetResource(pSRVBox);
	Effects::pBoxEffect->pFxsecondSR->SetResource(pSecondSRVBox);
	Effects::pBoxEffect->pFxDirLights->SetRawValue(&mDirLights[0], 0, 3 * sizeof(mDirLights[0]));
	Effects::pBoxEffect->pFxSpotLights->SetRawValue(&mSpotLight, 0,  sizeof(mSpotLight));

}

bool Box::BuildInputLayouts()  //可以复用代码，大体上是一样的
{
	UINT numLayoutElements = ARRAYSIZE(InputLayoutDesc::NormalTex);
	return BuildInputLayouts_(InputLayoutDesc::NormalTex, Effects::pBoxEffect->pFxLightTexTech, numLayoutElements);
	/*UINT numLayoutElements = ARRAYSIZE(InputLayoutDesc::NormalTex);

	D3DX11_PASS_DESC passDesc;
	Effects::pBoxEffect->pFxLightTexTech->GetPassByIndex(0)->GetDesc(&passDesc);

	HR(Device->CreateInputLayout(InputLayoutDesc::NormalTex, numLayoutElements, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout), L"CreateInputLayout");

	return true;*/
}

void Box::SetShadowMartrix(const Camera* camera,const Terrain* terrain)
{
	//设置绘制阴影
	XMVECTOR ground = XMVectorSet(0.f, 1.f, 0.f, terrain->getHeight(mWorld._41, mWorld._43));  //平面y=2.5
																				   //投影方向：光源方向的反向
	XMVECTOR lightDir = -XMLoadFloat3(&mDirLights[0].direction);
	//生成投影矩阵
	XMMATRIX S = XMMatrixShadow(ground, lightDir);
	//箱子阴影 相关变换矩阵
	XMMATRIX Worldsha = XMLoadFloat4x4(&mWorld) * S * XMMatrixTranslation(0.f, 0.001f, 0.f);
	XMStoreFloat4x4(&worldShadow, Worldsha);
	XMVECTOR det = XMMatrixDeterminant(Worldsha);  //求逆矩阵，可以用函数
	XMMATRIX worldInvTransposeShadow = XMMatrixTranspose(XMMatrixInverse(&det, Worldsha));
	XMStoreFloat4x4(&worldInvTransShadow, worldInvTransposeShadow);
	XMStoreFloat4x4(&wvpShadow, Worldsha * (camera->GetViewProj()));
	XMStoreFloat4x4(&texTransShadow, XMMatrixIdentity());
}

void Box::Update(float dt, const Terrain* terrain,  Camera* camera, const XMFLOAT3& eyeposw)
{
	XMMATRIX P = XMMatrixRotationY(dt);
	XMStoreFloat4x4(&mWorld, XMLoadFloat4x4(&mWorld)*P);
	SetMatrix();


	float x = mWorld._41;
	float y = mWorld._42;
	float z = mWorld._43;

	/*float tt_1, tt_2, tt_3;
	tt_1 = mWorld._41;
	tt_2 = mWorld._42;
	tt_3 = mWorld._43;*/

	float new_y = terrain->getHeight(mWorld._41, mWorld._43) + BoxHeight / 2;
	mWorld._42 = new_y;

	XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(&mWorld));
	XMVECTOR camPosV = XMVector3TransformCoord(camera->GetPosotionXM(), XMMatrixInverse(&det, XMLoadFloat4x4(&mWorld)));
	XMFLOAT4 camPosToBox;
	XMStoreFloat4(&camPosToBox, camPosV);

	//限制相机相对于盒子距离,盒子长宽高为10,10
	float distance = 5.f;

	//第一种方案，按F离开箱子，不能在箱子上漫游，位于箱子的雪人头顶上面，按F之后落地坐标为箱子周围随机点。
	{
		if ((abs(camPosToBox.x) < BoxWidth / 2 + distance && abs(camPosToBox.z) < BoxDepth / 2 + distance))
		{
			//考虑到站在箱子的雪人的头上再高一点的视角。 雪人高度为17
			camera->SetPosition(mWorld._41, mWorld._42 + BoxHeight / 2 + 20, mWorld._43);
			camonbox = true;
		}
		if (camonbox && GetAsyncKeyState('F') & 0x8000)
		{
			float x_pos[] = { mWorld._41 - BoxWidth / 2 - 10,mWorld._41 + BoxWidth / 2 + 10 };
			float y_pos[] = { mWorld._43 - BoxDepth / 2 - 10, mWorld._43 + BoxDepth / 2 + 10 };
			srand((unsigned)time(NULL));
			int i = rand() % 2;
			int j = rand() % 2;
			float new_y = terrain->getHeight(x_pos[i], y_pos[j]) + 10;
			camera->SetPosition(x_pos[i], new_y, y_pos[j]);
			camonbox = false;
		}
	}
	//第二种方案，非按键下车，可以在箱子上漫游，但是不能在箱子运动前方下车，相机与箱子上的雪人没有设置碰撞检测
	/*{
		if (!camonbox && (abs(camPosToBox.x) < BoxWidth / 2 + distance && abs(camPosToBox.z) < BoxDepth / 2 + distance))
		{
			//考虑到站在箱子的雪人的头上再高一点的视角。 雪人高度为17
			camera->SetPosition(mWorld._41, mWorld._42 + BoxHeight / 2 + 20, mWorld._43);
			camonbox = true;
		}
		if (camonbox && (abs(camPosToBox.x) > BoxWidth / 2 + distance + 2 || abs(camPosToBox.z) > BoxDepth / 2 + distance + 2))
		{
			//设置比漫游到箱子上面的距离限制要大，这样就不会在漫游下去的时候又自动到箱子上面
			//默认不能从箱子运动的前方下去，从前方下去会无限制的自动到箱子上面
			XMFLOAT3 campos = camera->GetPosition();
			camera->SetPosition(campos.x, terrain->getHeight(campos.x, campos.z) + 10.f, campos.z);
			camonbox = false;
		}
		if (camonbox)
		{
			XMVECTOR camPosV = XMVector3TransformCoord(XMLoadFloat3(&camera->GetPosition()),
				XMMatrixRotationY(dt));
			//更新摄像机
			camera->SetPosition(camPosV);
			camera->SetPosition(camera->GetPosition().x, terrain->getHeight(camera->GetPosition().x, camera->GetPosition().z) + BoxHeight + 10.f, camera->GetPosition().z);
		}
	}*/

		//聚光灯位置设为相机位置，朝向看的位置
		mSpotLight.position = eyeposw;
		XMStoreFloat3(&mSpotLight.direction, camera->GetLookXM());

		Effects::pBoxEffect->pFxEyePos->SetRawValue(&eyeposw, 0, sizeof(XMFLOAT3));

		SetShadowMartrix(camera, terrain);
}

void Box::Draw(ID3D11DeviceContext* dc, CXMMATRIX viewproj)
{
	dc->IASetInputLayout(mInputLayout);
	UINT stride = sizeof(Vertex::NormalTexVertice);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mVerticeBuffer, &stride, &offset);
	dc->IASetIndexBuffer(mIndiceBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3DX11EffectTechnique* tech = Effects::pBoxEffect->pFxLightTexTech;
	Render(tech,dc, viewproj, IndiceCount, 0, 0);


	dc->IASetVertexBuffers(0, 1, &mVerticeBuffer, &stride, &offset);
	dc->IASetIndexBuffer(mIndiceBuffer, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC desc2;
	ID3DX11EffectTechnique* tech1 = Effects::pBoxEffect->pFxLight;
	tech1->GetDesc(&desc2);
	for (UINT i = 0; i < desc2.Passes; ++i)
	{
		Effects::pBoxEffect->pFxWorld->SetMatrix(reinterpret_cast<const float*>(&worldShadow));
		Effects::pBoxEffect->pFxMaterial->SetRawValue(&materialShadow, 0, sizeof(materialShadow));
		Effects::pBoxEffect->pFxWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&worldInvTransShadow));
		Effects::pBoxEffect->pFxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&wvpShadow));
		Effects::pBoxEffect->pFxTexTrans->SetMatrix(reinterpret_cast<const float*>(&texTransShadow));

		tech1->GetPassByIndex(i)->Apply(0, dc);
		dc->DrawIndexed(static_cast<UINT>(box.indices.size()), 0, 0);
	}
	dc->OMSetBlendState(0, 0, 0xffffffff);
	dc->OMSetDepthStencilState(0, 0);

}

bool Box::RenderState()
{


	//禁止写颜色
	D3D11_BLEND_DESC noColorWriteBlendDesc;
	noColorWriteBlendDesc.AlphaToCoverageEnable = false;
	noColorWriteBlendDesc.IndependentBlendEnable = false;
	noColorWriteBlendDesc.RenderTarget[0].BlendEnable = false;
	noColorWriteBlendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
	HR(Device->CreateBlendState(&noColorWriteBlendDesc, &NoColorWriteBS), L"CreateBlendState");

	//平面阴影渲染设置
	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	HR(Device->CreateDepthStencilState(&noDoubleBlendDesc, &NoDoubleBlendingDSS),L"CreateDepthStencilState")
	return true;
}
