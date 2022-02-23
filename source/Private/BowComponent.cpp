// Fill out your copyright notice in the Description page of Project Settings.


#include "BowComponent.h"

#include "DrawDebugHelpers.h"

UBowComponent::UBowComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBowComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const auto& HandleLoc = BowHandle->GetComponentLocation();
	const auto& StringLoc = StringContactPoint->GetComponentLocation();
	// DrawDebugDirectionalArrow(GetWorld(), HandleLoc, StringLoc, 1.0f, FColor::Blue);
	
	const auto HandleToString = StringLoc - HandleLoc;
	
	// const auto RotMatrix = FRotationMatrix::MakeFromXY(HandleToString, StringContactPoint->GetForwardVector());
	const auto RotMatrix = FRotationMatrix::MakeFromZX(HandleToString, StringContactPoint->GetRightVector());
	Bow->SetWorldRotation(RotMatrix.Rotator());
	
	auto newLocation = FMath::LerpStable(Bow->GetComponentLocation(), BowHandle->GetComponentLocation(), 0.4f);
	Bow->SetWorldLocation(newLocation);

	auto dbgStart = BowHandle->GetComponentLocation();
	
	// DrawDebugLine(GetWorld(), dbgStart, dbgStart + Bow->GetForwardVector() * 10.0f, FColor::Blue);

	const float stringAngleCos = FVector::DotProduct(Bow->GetForwardVector(), StringContactPoint->GetForwardVector());
	if (stringAngleCos < 0)
	{
		// DrawDebugLine(GetWorld(), dbgStart, dbgStart + StringContactPoint->GetForwardVector() * 10.0f, FColor::Red);
	}
	else
	{
		// DrawDebugLine(GetWorld(), dbgStart, dbgStart + StringContactPoint->GetForwardVector() * 10.0f, FColor::Green);
	}

	VTracker.Tick(DeltaTime, Bow->GetComponentLocation());

	CalculateBowingSpeed();
}

void UBowComponent::CalculateBowingSpeed()
{
	const auto Velocity = VTracker.GetVelocity();
	const auto BowingDir = BowHandle->GetUpVector();
	BowingSpeed = FMath::Abs(FVector::DotProduct(Velocity, BowingDir));
}


