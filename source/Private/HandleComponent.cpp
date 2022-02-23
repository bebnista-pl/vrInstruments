#include "HandleComponent.h"

UHandleComponent::UHandleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetTickGroup(TG_PostPhysics);
}

void UHandleComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UHandleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Attachment)
	{
		SetWorldLocation(Attachment->GetComponentLocation());
		SetWorldRotation(Attachment->GetComponentRotation());
	}
}

void UHandleComponent::Pickup_Implementation(USceneComponent* AttachTo)
{
	Attachment = AttachTo;
	SetComponentTickEnabled(true);
	UE_LOG(LogTemp, Warning, TEXT("Attached"));
}

void UHandleComponent::Drop_Implementation()
{
	Attachment = nullptr;
	SetComponentTickEnabled(false);
}
