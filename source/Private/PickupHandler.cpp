#include "PickupHandler.h"

void PickupHandler::Setup(UStaticMeshComponent *Mesh)
{
	StaticMeshComponent = Mesh;
	RootComponent = StaticMeshComponent->GetAttachmentRoot();
	bSimulatePhyscis = StaticMeshComponent->BodyInstance.bSimulatePhysics;
}

void PickupHandler::Pickup(USceneComponent *AttachTo) const
{
	const FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
	RootComponent->AttachToComponent(AttachTo, Rules);
	StaticMeshComponent->SetSimulatePhysics(false);
	RootComponent->SetRelativeLocation(FVector(0.f));
	RootComponent->SetRelativeRotation(FRotator::ZeroRotator);
}

void PickupHandler::Drop() const
{
	const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
	RootComponent->DetachFromComponent(Rules);

	StaticMeshComponent->SetSimulatePhysics(bSimulatePhyscis);
}
