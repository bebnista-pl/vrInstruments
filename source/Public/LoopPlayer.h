// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SynthComponent.h"
#include "DSP/SampleBufferReader.h"
#include "Sound/SampleBufferIO.h"

#include "CoreMinimal.h"

#include <vector>

#include "SampleBuffer.h"
#include "AudioLooper.h"

#include "LoopPlayer.generated.h"

// ========================================================================
// ULoopPlayer
// Synth component class which implements USynthComponent
// This is a simple hello-world type example which generates a sine-wave
// tone using a DSP oscillator class and implements a single function to set
// the frequency. To enable example:
// 1. Ensure "SignalProcessing" is added to project's .Build.cs in PrivateDependencyModuleNames
// 2. Enable macro below that includes code utilizing SignalProcessing Oscilator
// ========================================================================

UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class QUESTTEST_API ULoopPlayer : public USynthComponent
{
	GENERATED_BODY()

public:
	explicit ULoopPlayer(const FObjectInitializer&);
	~ULoopPlayer();

	virtual bool Init(int32& SampleRate) override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;

	void LoadSoundWaveInternal();
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;
	
public:
	void SetVolume(float v) { Volume = v; }
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Synth")
    USoundWave* SoundWave;

private:
    Audio::FSampleBufferReader SampleBufferReader;
    AudioLooper Looper;
    Audio::TSampleBuffer<int16> SampleBuffer;
    Audio::FSoundWavePCMLoader SoundWaveLoader;

	float Volume = 1.0f;
	float PrevVolume = 1.0f;
	bool bIsLoaded = false;
};
