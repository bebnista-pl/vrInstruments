// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <array>

#include "Components/SynthComponent.h"
#include "AudioLooper.h"
#include "DSP/SampleBufferReader.h"
#include "Sound/SampleBufferIO.h"

#include "SimpleSamplePlayer.generated.h"

UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class QUESTTEST_API USimpleSamplePlayer : public USynthComponent
{
	GENERATED_BODY()

public:
	explicit USimpleSamplePlayer(const FObjectInitializer&);
	
	virtual bool Init(int32& SampleRate) override;
	void InitAudio();
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

	void PlayOne(float volumeMult = 1.0f);
	void SetSoundWave(USoundWave* wave) { this->SoundWave = wave; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Synth")
	USoundWave* SoundWave;

private:
	void LoadSoundWaveInternal();

	int FirstFreeLooper() const;

protected:
	constexpr static uint32_t NumPlayers = 8;
	std::array<OneShotAudioPlayer, NumPlayers> AudioPlayers;
	std::array<float, NumPlayers> Volumes{ 0.0f };

	Audio::TSampleBuffer<int16> SampleBuffer;
	Audio::FSoundWavePCMLoader SoundWaveLoader;

	bool bIsLoaded = false;
};
