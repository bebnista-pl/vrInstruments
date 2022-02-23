// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "VelocityTracker.h"
#include "Components/ActorComponent.h"
#include "BowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUESTTEST_API UBowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBowComponent();

	void SetStringContact(USceneComponent* Component) { StringContactPoint = Component; }
	void SetBowRoot(USceneComponent* Component) { Bow = Component; }
	void SetBowHandle(USceneComponent* Component) { BowHandle = Component; }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetBowingSpeed() { return BowingSpeed; }
	
private:
	void CalculateBowingSpeed();
	
	VelocityTracker VTracker;

	USceneComponent* StringContactPoint;
	USceneComponent* BowHandle;
	USceneComponent* Bow;

	float BowingSpeed = 0.0f;
};
