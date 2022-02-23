#pragma once

#include "CoreMinimal.h"

#include "PickupHandler.h"
#include "PickupInterface.h"
#include "VelocityTracker.h"

#include "GameFramework/Actor.h"

#include "DrumStick.generated.h"

#define COLLISION_STICKHEAD ECC_GameTraceChannel1
#define COLLISION_HAND ECC_GameTraceChannel2

UCLASS()
class QUESTTEST_API ADrumStick : public AActor, public IPickupInterface
{
	GENERATED_BODY()
	
public:	
	ADrumStick();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Pickup_Implementation(USceneComponent* AttachTo) override;
	virtual void Drop_Implementation() override;

	FVector GetStickVelocity() const { return StickVelocity.GetVelocity(); }
	FVector GetHandVelocity() const { return HandVelocity.GetVelocity(); }
	FVector StickHeadLocation() const { return StickheadCollider->GetComponentLocation(); }
	
private:
	void CreateCollisionComponents();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* CollisionRoot;
	
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	UStaticMeshComponent* StickheadCollider = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	UStaticMeshComponent* HandCollider = nullptr;
	
	PickupHandler pickupHandler;
	
	VelocityTracker StickVelocity{5};
	VelocityTracker HandVelocity{5};
	
	FDebugFloatHistory SpeedHistory;

	UPROPERTY(EditAnywhere)
	bool ShowDebugInfo;
};
