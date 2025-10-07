#include "pch.h"
#include "EditorEngine.h"
#include "SLevelViewport.h"
#include "EditorViewportClient.h"
#include "D3D11GraphicsDevice.h"
#include "Renderer.h"
#include "World.h"
#include "Level.h"
#include "StaticMeshActor.h"
#include "StaticMesh.h"
#include "KismetProceduralMeshLibrary.h"

IMPLEMENT_CLASS(UEditorEngine, UEngine)

UEditorEngine::UEditorEngine()
	: ViewportClient(nullptr)
	, bEditorInitialized(false)
{
}

UEditorEngine::~UEditorEngine()
{
}

bool UEditorEngine::Init(HWND WindowHandle, uint32 WindowWidth, uint32 WindowHeight)
{
	if (!UEngine::Init(WindowHandle, WindowWidth, WindowHeight))
	{
		return false;
	}

	// 테스트용 큐브 액터 생성
	if (World && World->GetCurrentLevel())
	{
		ULevel* Level = World->GetCurrentLevel();
		AStaticMeshActor* CubeActor = Level->SpawnActor<AStaticMeshActor>(FVector(0, 0, 100), FVector::Zero, FName("TestCube"));

		if (CubeActor)
		{
			// 큐브 메시 생성
			FStaticMeshRenderData CubeMeshData = UKismetProceduralMeshLibrary::CreateCubeMesh(FVector(50, 50, 50));

			UStaticMesh* CubeMesh = NewObject<UStaticMesh>();
			CubeMesh->SetRenderData(CubeMeshData);

			CubeActor->SetStaticMesh(CubeMesh);
		}
	}

	if (!CreateMainViewport())
	{
		return false;
	}

	if (!CreateViewportClient())
	{
		return false;
	}

	bEditorInitialized = true;
	return true;
}

void UEditorEngine::Tick(float DeltaTime)
{
	UEngine::Tick(DeltaTime);

	if (bEditorInitialized)
	{
		UpdateEditorViewport(DeltaTime);
	}
}

void UEditorEngine::Shutdown()
{
	if (MainViewport)
	{
		MainViewport->Shutdown();
		MainViewport.reset();
	}

	ViewportClient = nullptr;
	bEditorInitialized = false;

	UEngine::Shutdown();
}

bool UEditorEngine::CreateMainViewport()
{
	if (!GraphicsDevice || !Renderer)
	{
		return false;
	}

	MainViewport = std::make_shared<SLevelViewport>();
	if (!MainViewport)
	{
		return false;
	}

	MainViewport->Initialize(GraphicsDevice, Renderer);

	if (World)
	{
		MainViewport->SetWorld(World);
	}

	return true;
}

bool UEditorEngine::CreateViewportClient()
{
	if (!MainViewport)
	{
		return false;
	}

	TSharedPtr<SEditorViewport> EditorViewport = MainViewport->GetEditorViewport();
	if (!EditorViewport)
	{
		return false;
	}

	ViewportClient = EditorViewport->GetViewportClient().get();

	if (ViewportClient)
	{
		ViewportClient->SetCameraMode(ECameraMode::Orbit);
		ViewportClient->SetOrbitTarget(FVector(0, 0, 100));
		ViewportClient->SetOrbitDistance(500.0f);
	}

	return ViewportClient != nullptr;
}

void UEditorEngine::UpdateEditorViewport(float DeltaTime)
{
	if (!MainViewport)
	{
		return;
	}

	FGeometry ViewportGeometry;
	ViewportGeometry.AbsolutePosition = FVector2(0, 0);
	ViewportGeometry.LocalSize = FVector2(1600, 900);

	MainViewport->Tick(ViewportGeometry, 0.0f, DeltaTime);
}

void UEditorEngine::RenderEditorViewport()
{
}
