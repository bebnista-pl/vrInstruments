// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BowComponent.h"
#include "HandleComponent.h"
#include "LoopPlayer.h"
#include "VelocityTracker.h"
#include "GameFramework/Actor.h"
#include "Bass.generated.h"

UCLASS()
class QUESTTEST_API ABass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere)
	USceneComponent* BassRoot;
	
	UPROPERTY(EditAnywhere, Category="Visualization")
	UStaticMeshComponent* BassMesh;

	UPROPERTY(EditAnywhere)
	USceneComponent* StringContactPoint;

	UPROPERTY(EditAnywhere)
	USceneComponent* BowRoot;
	
	UPROPERTY(EditAnywhere, Category="Visualization")
	UStaticMeshComponent* BowMesh;

	UPROPERTY(EditAnywhere, Category="Interaction")
	UHandleComponent* InstrumentHandle;

	FVector InstrumentOffset;

	UPROPERTY(VisibleAnywhere)
	UBowComponent* BowComponent;

	UPROPERTY(EditAnywhere, Category="Interaction")
	UHandleComponent* BowHandle;

	UPROPERTY(EditAnywhere, Category="Sound")
	ULoopPlayer* LoopPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	UCurveFloat* BassVolumeCurve;
	
	float InterpolatedVolume = 0.0f;
};
