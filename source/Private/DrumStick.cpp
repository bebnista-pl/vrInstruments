#include "DrumStick.h"


#include "DrawDebugHelpers.h"
#include "PhysicsEngine/BodyInstance.h"

ADrumStick::ADrumStick()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);
	
	// StaticMeshComponent
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetNotifyRigidBodyCollision(false);
	StaticMeshComponent->SetUseCCD(false);
	StaticMeshComponent->bMultiBodyOverlap = false;

	CreateCollisionComponents();

	pickupHandler.Setup(StaticMeshComponent);

	ShowDebugInfo = false;
}

void ADrumStick::CreateCollisionComponents()
{
	CollisionRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Collision"));
	CollisionRoot->SetupAttachment(SceneComponent);

	// Hand
	HandCollider = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandCollider"));
	HandCollider->SetupAttachment(CollisionRoot);
	HandCollider->SetSimulatePhysics(false);
	HandCollider->SetNotifyRigidBodyCollision(true);
	HandCollider->SetUseCCD(true);
	HandCollider->bMultiBodyOverlap = true;
	
	HandCollider->SetCollisionObjectType(COLLISION_HAND);

	// Stickhead
	StickheadCollider = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StickheadCollider"));
	StickheadCollider->SetupAttachment(CollisionRoot);
	StickheadCollider->SetSimulatePhysics(false);
	StickheadCollider->SetNotifyRigidBodyCollision(true);
	StickheadCollider->SetUseCCD(true);
	StickheadCollider->bMultiBodyOverlap = true;
	StickheadCollider->SetCollisionObjectType(COLLISION_STICKHEAD);
}

void ADrumStick::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("Hand channel: %d, Stick channel: %d"), HandCollider->GetCollisionObjectType(), StickheadCollider->GetCollisionObjectType());
	Super::BeginPlay();
	SpeedHistory.MaxSamples = 2000.0f;
	SpeedHistory.MaxValue = 1000.0f;
	SpeedHistory.MinValue = 0.0f;
	SpeedHistory.bAutoAdjustMinMax = false;
}

void ADrumStick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StickVelocity.Tick(DeltaTime, StickheadCollider->GetComponentLocation());
	HandVelocity.Tick(DeltaTime, HandCollider->GetComponentLocation());

	const FVector2D GraphSz(10, 10);
	const auto speed = StickVelocity.GetVelocity().Size();
	SpeedHistory.AddSample(StickVelocity.GetVelocity().Size());

	if (ShowDebugInfo)
	{
		DrawDebugFloatHistory(*GetWorld(), SpeedHistory, StickheadCollider->GetComponentLocation(), GraphSz, FColor::Blue);
		DrawDebugString(GetWorld(), FVector::ZeroVector, FString::Printf(TEXT("%f"), speed), this, FColor::White, 0.f);
	}
}

void ADrumStick::Pickup_Implementation(USceneComponent* AttachTo)
{
	pickupHandler.Pickup(AttachTo);
}

void ADrumStick::Drop_Implementation()
{
	pickupHandler.Drop();
}
