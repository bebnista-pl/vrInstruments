// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#include "DrumStick.h"
#include "PickupHandler.h"
#include "PickupInterface.h"
#include "SimpleSamplePlayer.h"
#include "GameFramework/Actor.h"
#include "Drum.generated.h"

class USoundCue;
class SupuriousHitDetector;
class UDrumZone;

UCLASS()
class QUESTTEST_API ADrum : public AActor, public IPickupInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrum();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Pickup_Implementation(USceneComponent* AttachTo) override;
	virtual void Drop_Implementation() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
    void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult);
	
private:
	void SkinHit(ADrumStick*);
	void CymbalsHit(ADrumStick*);

	float GetEdgeSpeed();

	UDrumZone* GetClosestSkinZone(const FVector&) const;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	UCurveFloat* StickVolumeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	UCurveFloat* CymbalsVolumeCurve;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere)
	USceneComponent* SkinZonesRoot;
	
	UPROPERTY(EditAnywhere, Category="Debug")
	bool ShowDebugInfo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* EdgeComponent;
		
	PickupHandler pickupHandler;

	constexpr static float StickNormalizationCoeff = 1.0f / 500.0f;
	constexpr static float CymbalsNormalizationCoeff = 1.0f / 400.0f;
	constexpr static float EdgeNormalizationCoeff = 1.0f / 300.0f;
	
	VelocityTracker EdgeVelocityTracker;
	
	SupuriousHitDetector* SkinHitValidation = nullptr;
	SupuriousHitDetector* HandHitValidation = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TArray<UDrumZone*> SkinZones;

	UPROPERTY(EditAnywhere, Category="Sound")
	USimpleSamplePlayer* CymbalsSamplePlayer;
};
