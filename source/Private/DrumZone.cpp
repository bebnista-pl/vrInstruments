#include "DrumZone.h"

#include "SimpleSamplePlayer.h"

UDrumZone::UDrumZone()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SamplePlayer = CreateDefaultSubobject<USimpleSamplePlayer>(TEXT("SamplePlayer"));
	SamplePlayer->SetupAttachment(this);
}

void UDrumZone::PlayOne(float vol)
{
	if (_levels.Num() > 0)
	{
		PlayLevel(vol);
	} else
	{
		SamplePlayer->PlayOne(vol);
	}
}

void UDrumZone::PlayLevel(float vol)
{
	for (auto &lvl : _levels)
	{
		if (vol <= lvl.maxVolume)
		{
			lvl.player->PlayOne(vol);
			return;
		}
	}

	_levels.Last().player->PlayOne(vol); // play the loudest if failed to find a match
}

void UDrumZone::BeginPlay()
{
	Super::BeginPlay();

	for (auto& level : _levels)
	{
		FObjectDuplicationParameters duplicateParams(SamplePlayer, this);
		auto *duplicated = StaticDuplicateObjectEx(duplicateParams);
			
		level.player = Cast<USimpleSamplePlayer>(duplicated);
		level.player->SetSoundWave(level.soundWave);
		level.player->SetAutoActivate(true);
		level.player->Initialize();
		level.player->Activate(true);
		level.player->InitAudio();
		level.player->SetupAttachment(this);
	}

	if (_levels.Num() > 0)
	{
		// SamplePlayer->UnregisterComponent();
		SamplePlayer->DestroyComponent(true);
	}
}
