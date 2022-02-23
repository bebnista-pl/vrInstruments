// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "PickupHandler.h"
#include "PickupInterface.h"
#include "Components/SceneComponent.h"
#include "HandleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUESTTEST_API UHandleComponent : public UStaticMeshComponent, public IPickupInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHandleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Pickup_Implementation(USceneComponent* AttachTo) override;
	virtual void Drop_Implementation() override;

	USceneComponent* Attachment = nullptr;
};
