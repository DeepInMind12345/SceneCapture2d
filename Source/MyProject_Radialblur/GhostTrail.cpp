// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostTrail.h"
#include "EngineUtils.h"
#include <Components/SceneCaptureComponent2D.h>
#include <Camera/PlayerCameraManager.h>
#include <Particles/ParticleSystemComponent.h>
#include <Components/SkeletalMeshComponent.h>


// Sets default values
AGhostTrail::AGhostTrail()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGhostTrail::BeginPlay()
{
	Super::BeginPlay();


	//load material 
	UWorld* World = GetWorld();
	GhostTrailMat = LoadObject<UMaterial>(GetTransientPackage(), TEXT("/Game/M_GhostTrail.M_GhostTrail"));
	if (GhostTrailMat)
	{
		GhostTrailMID = UMaterialInstanceDynamic::Create(GhostTrailMat, this, FName("GhostTrailMID"));
	}

	//find post process
	int32 num = World->PostProcessVolumes.Num();
	if (num > 0)
	{
		PostProcessVolumeActor = (APostProcessVolume*)(World->PostProcessVolumes[0]);
		PostProcessVolumeActor->bEnabled = true;
		PostProcessVolumeActor->BlendWeight = 1.0f;
		PostProcessVolumeActor->bUnbound = true;
	}

	//cull character and emiisve
	for (TActorIterator<ACharacter> It(World); It; ++It)
	{
		Character = *It;
	}

	for (TActorIterator<AEmitter> It(World); It; ++It)
	{
		Emitter = *It;
	}


	//create scene capture2d
	SceneCapture2D = World->SpawnActor<ASceneCapture2D>();

	FVector2D ViewportSize = FVector2D(1, 1);
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	USceneCaptureComponent2D* SceneCaptureComponent2D = SceneCapture2D->GetCaptureComponent2D();
	SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;

	SceneCaptureComponent2D->TextureTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("SceneCaptureTextureTarget"));
	SceneCaptureComponent2D->TextureTarget->InitCustomFormat(ViewportSize.X, ViewportSize.Y, PF_A16B16G16R16, false);
	SceneCaptureComponent2D->TextureTarget->ClearColor = FLinearColor::Black;

	GhostTrailMID->SetTextureParameterValue(TEXT("RenderTexture"), SceneCaptureComponent2D->TextureTarget);


	//disable fog
	FEngineShowFlagsSetting NewFlagSetting;
	NewFlagSetting.ShowFlagName = TEXT("Fog");
	NewFlagSetting.Enabled = false;
	SceneCaptureComponent2D->ShowFlagSettings.Add(NewFlagSetting);

	int32 SettingIndex = SceneCaptureComponent2D->ShowFlags.FindIndexByName(TEXT("Fog"));
	SceneCaptureComponent2D->ShowFlags.SetSingleFlag(SettingIndex, false);
}

// Called every frame
void AGhostTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GhostTrailMID != nullptr)
	{
		GhostTrailMID->SetScalarParameterValue(FName("RefractionAmount"), RefractionAmount);
	}


	APlayerCameraManager* PlayerCameraManager = nullptr;
	for (TActorIterator<APlayerController> It(GetWorld()); It; ++It)
	{
		PlayerCameraManager = (*It)->PlayerCameraManager;
	}

	if (PlayerCameraManager != nullptr)
	{
		SceneCapture2D->SetActorLocationAndRotation(PlayerCameraManager->GetCameraLocation(), PlayerCameraManager->GetCameraRotation());
	
		USceneCaptureComponent2D* SceneCaptureComponent2D = SceneCapture2D->GetCaptureComponent2D();
		SceneCaptureComponent2D->ProjectionType = PlayerCameraManager->bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
		SceneCaptureComponent2D->FOVAngle = PlayerCameraManager->GetFOVAngle();
		SceneCaptureComponent2D->OrthoWidth = PlayerCameraManager->GetOrthoWidth();
	}
}

void AGhostTrail::Trigger()
{
	USceneCaptureComponent2D* SceneCaptureComponent2D = SceneCapture2D->GetCaptureComponent2D();
	if (SceneCaptureComponent2D == nullptr)
	{
		return;
	}
	if (Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		Mesh->bOwnerNoSee = true;
		Mesh->MarkRenderStateDirty();
		SceneCaptureComponent2D->ShowOnlyActors.Add(Character);
	}

	if (Emitter)
	{
		Emitter->SetOwner(Character);
		UParticleSystemComponent* psc = Emitter->GetParticleSystemComponent();
		psc->bOwnerNoSee = true;
		psc->MarkRenderStateDirty();
		SceneCaptureComponent2D->ShowOnlyActors.Add(Emitter);
	}

	if (PostProcessVolumeActor)
	{
		PostProcessVolumeActor->AddOrUpdateBlendable(GhostTrailMID, 1.0);
	}
}

void AGhostTrail::ShutDown()
{
	USceneCaptureComponent2D* SceneCaptureComponent2D = SceneCapture2D->GetCaptureComponent2D();
	if (SceneCaptureComponent2D == nullptr)
	{
		return;
	}

	SceneCaptureComponent2D->ShowOnlyActors.Empty();

	if (Character)
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		Mesh->bOwnerNoSee = false;
		Mesh->MarkRenderStateDirty();
	}

	if (Emitter)
	{
		Emitter->SetOwner(nullptr);
		UParticleSystemComponent* psc = Emitter->GetParticleSystemComponent();
		psc->bOwnerNoSee = false;
		psc->MarkRenderStateDirty();
	}

	if (PostProcessVolumeActor)
	{
		PostProcessVolumeActor->AddOrUpdateBlendable(GhostTrailMID, 0);
	}
}

