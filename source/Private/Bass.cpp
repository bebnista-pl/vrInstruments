

#include "Bass.h"


#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

namespace
{
	void SnapToHandle(USceneComponent* Component, UHandleComponent* HandleComponent)
	{
		const auto& Loc = HandleComponent->GetComponentLocation();
		const auto& Rot = HandleComponent->GetComponentRotation();
	
		Component->SetWorldLocation(Loc);
		Component->SetWorldRotation(Rot);
	}
}

ABass::ABass()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Instrument handle
	InstrumentHandle = CreateDefaultSubobject<UHandleComponent>(TEXT("InstrumentHandle"));
	InstrumentHandle->SetupAttachment(RootSceneComponent);
	InstrumentHandle->SetHiddenInGame(true);

	const ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere"));

	{
		if(SphereMeshFinder.Succeeded() && InstrumentHandle)
		{
			InstrumentHandle->SetStaticMesh(SphereMeshFinder.Object);
		}
	}
	
	// Main instrument
	BassRoot = CreateDefaultSubobject<USceneComponent>(TEXT("InstrumentRoot"));
	BassRoot->SetupAttachment(RootSceneComponent);
	
	BassMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BassMesh"));
	BassMesh->SetupAttachment(BassRoot);

	{
		const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/InstrumentsVR/Models/3-Bass_01"));
    
		if(MeshFinder.Succeeded() && BassMesh)
		{
			BassMesh->SetStaticMesh(MeshFinder.Object);
		}
	}

	BassMesh->SetRelativeLocation(FVector(-10.0f, 0.0f, 0.0f));
	BassMesh->SetRelativeRotation(FRotator::MakeFromEuler(FVector(80.0f, 0.0f, 0.0f)));
	
	StringContactPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StringContactPoint"));
	StringContactPoint->SetupAttachment(BassRoot);
	StringContactPoint->SetRelativeRotation(FRotator::MakeFromEuler(FVector(-10.0f, 0.0f, 0.0f)));
	
	// Bow
	BowRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BowRoot"));
	BowRoot->SetupAttachment(RootSceneComponent);
	
	BowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BowMesh"));
	BowMesh->SetupAttachment(BowRoot);
	
	PrimaryActorTick.bCanEverTick = true;

	{
		const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/InstrumentsVR/Models/SM_Smyczek"));

		if(MeshFinder.Succeeded() && BowMesh)
		{
			BowMesh->SetStaticMesh(MeshFinder.Object);
		}
	}
	
	// Bow interaction	
	BowHandle = CreateDefaultSubobject<UHandleComponent>(TEXT("BowHandle"));
	BowHandle->SetupAttachment(RootSceneComponent);
	BowHandle->SetHiddenInGame(true);

	LoopPlayer = CreateDefaultSubobject<ULoopPlayer>(TEXT("LoopPlayer"));
	LoopPlayer->SetupAttachment(RootSceneComponent);
	LoopPlayer->SetHiddenInGame(true);

	{
		if(SphereMeshFinder.Succeeded() && BowHandle)
		{
			BowHandle->SetStaticMesh(SphereMeshFinder.Object);
		}
	}
	
	BowComponent = CreateDefaultSubobject<UBowComponent>(TEXT("BowComponent"));
    BowComponent->SetStringContact(StringContactPoint);
    BowComponent->SetBowRoot(BowRoot);
    BowComponent->SetBowHandle(BowHandle);

	BassVolumeCurve = CreateDefaultSubobject<UCurveFloat>(TEXT("BassVolumeCurve"));
}

void ABass::BeginPlay()
{
	Super::BeginPlay();

	LoopPlayer->Activate();
}

void ABass::Destroyed()
{
}

void ABass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SnapToHandle(BassRoot, InstrumentHandle);
	// static float p = 0.0f;
	// p += DeltaTime;
	
	// const float sinVal = FMath::Sin(p * 4.0f);
	// LoopPlayer->SetVolume(FMath::Abs(sinVal));

	const auto Speed = BowComponent->GetBowingSpeed() * 0.02f;
	// UE_LOG(LogTemp, Display, TEXT("Speed: %f"), Speed);

	const float Volume = BassVolumeCurve->GetFloatValue(Speed);
	// UE_LOG(LogTemp, Display, TEXT("Volume: %f"), Volume);
	
	const float ClampedVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	
	// InterpolatedVolume = FMath::Lerp(InterpolatedVolume, ClampedVolume, 0.4f);
	LoopPlayer->SetVolume(ClampedVolume);
	
	const auto end = BowHandle->GetComponentLocation() + BowRoot->GetUpVector() * 50.0f;
	// DrawDebugDirectionalArrow(GetWorld(), BowHandle->GetComponentLocation(), end, 3.0f, FColor::Yellow);
}
