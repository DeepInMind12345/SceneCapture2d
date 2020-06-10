// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/PostProcessVolume.h>
#include <GameFramework/Character.h>
#include <Particles/Emitter.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/Material.h>
#include <GameFramework/Actor.h>
#include <Engine/SceneCapture2D.h>
#include "Engine/TextureRenderTarget2D.h"
#include "GhostTrail.generated.h"

UCLASS()
class MYPROJECT_RADIALBLUR_API AGhostTrail : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGhostTrail();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Trigger();

	UFUNCTION(BlueprintCallable)
	void ShutDown();

public:

	UPROPERTY(VisibleAnywhere)
	UMaterial* GhostTrailMat = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* GhostTrailMID = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APostProcessVolume* PostProcessVolumeActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ASceneCapture2D* SceneCapture2D = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RefractionAmount = 1.0f;

	UPROPERTY(Transient)
	ACharacter* Character;

	UPROPERTY(Transient)
	AEmitter* Emitter;
};
