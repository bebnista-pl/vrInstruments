#include "BassInstrument.h"

UBassInstrument::UBassInstrument()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBassInstrument::BeginPlay()
{
	Super::BeginPlay();
}

void UBassInstrument::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

