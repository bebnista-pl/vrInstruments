#include "Drum.h"


#include "DrawDebugHelpers.h"
#include "DrumStick.h"
#include "DrumZone.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

#define DEBUG_LOG(__text__, ...) UE_LOG(LogTemp, Display, TEXT(__text__), __VA_ARGS__)

class SupuriousHitDetector final
{
public:
	SupuriousHitDetector(float MinInterval)
		: MinInterval(MinInterval)
	{ }
	
	bool IsValid(float Time)
	{
		if (Time > NextHitCooldown)
		{
			NextHitCooldown = Time + MinInterval;
			return true;
		}

		return false;
	}

private:
	const float MinInterval = 0.0f;
	float NextHitCooldown = 0.0f;
};

// Sets default values
ADrum::ADrum()
{
	PrimaryActorTick.bCanEverTick = true;
	ShowDebugInfo = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DrumRoot"));
	SetRootComponent(SceneComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetNotifyRigidBodyCollision(true);
	StaticMeshComponent->bMultiBodyOverlap = true;
	StaticMeshComponent->SetUseCCD(true);
	
	SkinZonesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SkinZones"));
	SkinZonesRoot->SetupAttachment(SceneComponent);
	
	pickupHandler.Setup(StaticMeshComponent);

	StickVolumeCurve = CreateDefaultSubobject<UCurveFloat>(TEXT("StickVolumeCurve"));
	CymbalsVolumeCurve = CreateDefaultSubobject<UCurveFloat>(TEXT("CymbalsVolumeCurve"));

	EdgeComponent = CreateDefaultSubobject<USceneComponent>(TEXT("EdgeComponent"));
	EdgeComponent->SetupAttachment(RootComponent);

	CymbalsSamplePlayer = CreateDefaultSubobject<USimpleSamplePlayer>(TEXT("CymbalsSamplePlayer"));
	CymbalsSamplePlayer->SetupAttachment(RootComponent);
}

void ADrum::SkinHit(ADrumStick* Stick)
{
	const bool Valid = SkinHitValidation->IsValid(GetGameTimeSinceCreation());
	if (!Valid)
	{
		return;
	}
	
	const auto Vel = Stick->GetStickVelocity();
	const auto Right = RootComponent->GetRightVector();

	const float dbgSpeed  = Vel.Size();
	DEBUG_LOG("Speed: %f", dbgSpeed);
	
	// auto dbgStart = GetActorLocation();
	// auto dbgEnd = dbgStart + Vel;
	// auto world  = GetWorld();

	// DrawDebugDirectionalArrow(world, dbgStart, dbgEnd, 1.0f, FColor::Black, false, 3.0f);
	// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + HandVel, 1.0f, FColor::Red, false, 3.0f);
	// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + EdgeVel, 1.0f, FColor::Blue, false, 3.0f);

	const auto VelDotF = -FVector::DotProduct(Vel, Right);
	if (VelDotF > 0.0f)
	{
		const float Speed = VelDotF;
		const float SpeedNorm = StickNormalizationCoeff * Speed;

		const float Volume = StickVolumeCurve->GetFloatValue(SpeedNorm);
		
		if (ShowDebugInfo)
		{
			// DrawDebugString(GetWorld(), Stick->GetActorLocation(), FString::Printf(TEXT("Stick speed %f, Volume: %f"), SpeedNorm, Volume), nullptr, FColor::Blue, 2.0f);
		}

		auto* zone = GetClosestSkinZone(Stick->StickHeadLocation());
		zone->PlayOne(Volume);
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("Stick velocity: %f"), Vel.Size());
}

void ADrum::CymbalsHit(ADrumStick* Stick)
{
	const bool Valid = HandHitValidation->IsValid(GetGameTimeSinceCreation());
	if (!Valid)
	{
		return;
	}
	
	const auto HandVel = Stick->GetHandVelocity();
	const auto EdgeVel = EdgeVelocityTracker.GetVelocity();
	const auto Vel = HandVel - EdgeVel;
	
	// auto dbgStart = GetActorLocation();

	// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + HandVel, 1.0f, FColor::Red, false, 3.0f);
	// DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + EdgeVel, 1.0f, FColor::Blue, false, 3.0f);
	// DrawDebugDirectionalArrow(GetWorld(), dbgStart, dbgStart + Vel, 1.0f, FColor::Red, false, 3.0f);
	// const float EdgeSpeed = GetEdgeSpeed();
	// DrawDebugString(GetWorld(), Stick->GetActorLocation(), FString::Printf(TEXT("Edge speed: %f"), EdgeSpeed), nullptr, FColor::Blue, 2.0f);

	const auto Right = RootComponent->GetRightVector();
	const auto VelDotF = -FVector::DotProduct(Vel, Right);
	const float Speed = VelDotF * CymbalsNormalizationCoeff;

	if (Speed > 0.0f)
	{
		const float Volume = CymbalsVolumeCurve->GetFloatValue(Speed);

		CymbalsSamplePlayer->PlayOne(Volume);

		// DrawDebugString(GetWorld(), Stick->GetActorLocation(), FString::Printf(TEXT("Hand speed %f, Volume: %f"), Speed, Volume), nullptr, FColor::Blue, 2.0f);
	}

	// UE_LOG(LogTemp, Display, TEXT("Velocity %s Normal %s"), *HitterVelocity.ToString(), *ImpactNormalWS.ToString());

	// UE_LOG(LogTemp, Warning, TEXT("Hand velocity: %s"), &Vel.ToString()[0]);
}

float ADrum::GetEdgeSpeed()
{
	const auto Vel = EdgeVelocityTracker.GetVelocity();
	const auto VelDotRight = FVector::DotProduct(Vel, EdgeComponent->GetRightVector());
	return VelDotRight * EdgeNormalizationCoeff;	
}

UDrumZone* ADrum::GetClosestSkinZone(const FVector& p) const
{
	if (SkinZones.Num() == 0)
	{
		return nullptr;
	}

	auto* closestZone = SkinZones[0];
	float smallestDistSq = FVector::DistSquared(closestZone->GetComponentLocation(), p);
	
	for (auto *zone : SkinZones)
	{
		const float currDistSq = FVector::DistSquared(zone->GetComponentLocation(), p);
		if (currDistSq < smallestDistSq)
		{
			smallestDistSq = currDistSq;
			closestZone = zone;
		}
	}

	return closestZone;
}

// Called when the game starts or when spawned
void ADrum::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ADrum::OnOverlapBegin);
	StaticMeshComponent->OnComponentEndOverlap.AddDynamic(this, &ADrum::OnOverlapEnd);
	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ADrum::OnHit);

	constexpr float MaxBpm = 300.f;
	constexpr float MinInterval = 60.0f / MaxBpm;
	
	SkinHitValidation = new SupuriousHitDetector(MinInterval);
	HandHitValidation = new SupuriousHitDetector(MinInterval);

	for (auto* child : SkinZonesRoot->GetAttachChildren())
	{
		const auto zone = Cast<UDrumZone>(child);
		if (zone)
		{
			SkinZones.Add(zone);
		}
	}

	// level.player = Cast<USimpleSamplePlayer>(duplicated);
	// level.player->SetSoundWave(level.soundWave);
	// level.player->SetAutoActivate(true);
	// level.player->Initialize();
	// level.player->Activate(true);
	// level.player->InitAudio();
	// level.player->SetupAttachment(this);

	CymbalsSamplePlayer->SetAutoActivate(true);
	CymbalsSamplePlayer->Initialize();
	CymbalsSamplePlayer->Activate();
	CymbalsSamplePlayer->InitAudio();
	

	DEBUG_LOG("Found %d zones.", SkinZones.Num());
}

void ADrum::Destroyed()
{
	delete SkinHitValidation;
	delete HandHitValidation;
}

// Called every frame
void ADrum::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EdgeVelocityTracker.Tick(DeltaTime, EdgeComponent->GetComponentLocation());
}

void ADrum::Pickup_Implementation(USceneComponent* AttachTo)
{
	pickupHandler.Pickup(AttachTo);
}

void ADrum::Drop_Implementation()
{
	pickupHandler.Drop();
}

void ADrum::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Stick = Cast<ADrumStick>(OtherActor);

	if (Stick)
	{
		if (!ShowDebugInfo)
		{
			DrawDebugSphere(GetWorld(), Stick->GetActorLocation(), 5.0f, 10, FColor::Blue, false, 0.8f);
		}

		const auto CollisionType = OtherComp->GetCollisionObjectType();
		switch (CollisionType)
		{
			case COLLISION_HAND:
				CymbalsHit(Stick);
				break;
			case COLLISION_STICKHEAD:
				SkinHit(Stick);
				break;
			default: break;
		}
	}
}

void ADrum::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void ADrum::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit"));
}
