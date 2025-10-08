#include "pch.h"
#include "RenderPass.h"
#include "Engine.h"
#include "World.h"
#include "Level.h"
#include "Actor.h"
#include "StaticMeshComponent.h"
#include "StaticMesh.h"
#include "MaterialInterface.h"
#include "MaterialResource.h"
#include "Shader.h"
#include "StaticMeshRenderResource.h"
#include "SceneView.h"

// 전역 엔진 인스턴스 (extern 선언)
extern UEngine* GEngine;

void FDepthPrePass::Execute(const FRenderPassContext& Context)
{
	if (!Context.DeviceContext || !Context.DepthStencil)
	{
		return;
	}

	Context.DeviceContext->ClearDepthStencilView(
		Context.DepthStencil,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	ID3D11RenderTargetView* NullRTV = nullptr;
	Context.DeviceContext->OMSetRenderTargets(1, &NullRTV, Context.DepthStencil);

	if (Context.Viewport)
	{
		Context.DeviceContext->RSSetViewports(1, Context.Viewport);
	}
}

void FBasePass::Execute(const FRenderPassContext& Context)
{
	if (!Context.DeviceContext || !Context.RenderTarget)
	{
		return;
	}

	// 1. Clear RenderTarget
	Context.DeviceContext->ClearRenderTargetView(Context.RenderTarget, Context.ClearColor);

	// 2. RenderTarget 설정
	Context.DeviceContext->OMSetRenderTargets(1, &Context.RenderTarget, Context.DepthStencil);

	if (Context.Viewport)
	{
		Context.DeviceContext->RSSetViewports(1, Context.Viewport);
	}

	// 3. Primitive Topology 설정
	Context.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 4. GEngine에서 World 가져오기
	UWorld* World = GEngine ? GEngine->GetWorld() : nullptr;
	if (!World || !Context.SceneView)
	{
		return;
	}

	// 5. View/Projection 행렬 준비
	FMatrix ViewMatrix = Context.SceneView->GetViewMatrix();
	FMatrix ProjectionMatrix = Context.SceneView->GetProjectionMatrix();

	// 6. 모든 Actor 순회
	ULevel* Level = World->GetCurrentLevel();
	if (!Level)
	{
		return;
	}

	const TArray<AActor*>& Actors = Level->GetActors();
	for (AActor* Actor : Actors)
	{
		if (!Actor || Actor->IsHidden())
		{
			continue;
		}

		// 7. StaticMeshComponent 찾기
		UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
		if (!MeshComp)
		{
			continue;
		}

		UStaticMesh* Mesh = MeshComp->GetStaticMesh();
		if (!Mesh)
		{
			continue;
		}

		FStaticMeshRenderResource* RenderResource = Mesh->GetRenderResource();
		if (!RenderResource || !RenderResource->IsInitialized())
		{
			continue;
		}

		// 8. Material 가져오기
		UMaterialInterface* Material = Mesh->GetMaterial(0);
		if (!Material)
		{
			continue;
		}

		FMaterialResource* MatResource = Material->GetMaterialResource();
		if (!MatResource || !MatResource->IsInitialized())
		{
			continue;
		}

		FShader* Shader = MatResource->GetShader();
		if (!Shader || !Shader->IsInitialized())
		{
			continue;
		}

		// 9. Shader 바인딩
		Shader->Bind(Context.DeviceContext);

		// 10. MVP 행렬 계산
		FMatrix WorldMatrix = Actor->GetActorTransform();

		FMVPConstantBuffer MVPData;
		MVPData.World = WorldMatrix;
		MVPData.View = ViewMatrix;
		MVPData.Projection = ProjectionMatrix;

		// 11. Constant Buffer 설정 (FShader::SetConstantBuffer가 자동으로 처리)
		Shader->SetConstantBuffer(Context.DeviceContext, "MVPBuffer", MVPData);

		// 12. 버퍼 바인딩 및 그리기
		RenderResource->BindBuffers(Context.DeviceContext);
		RenderResource->Draw(Context.DeviceContext);
	}
}
