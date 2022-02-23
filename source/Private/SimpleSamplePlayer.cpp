// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleSamplePlayer.h"
#include <Sound/SoundClass.h>

#include <vector>

#define DEBUG_LOG(__text__, ...) UE_LOG(LogTemp, Display, TEXT(__text__), __VA_ARGS__)

USimpleSamplePlayer::USimpleSamplePlayer(const FObjectInitializer& Initializer)
	: Super(Initializer)
	  , SoundWave(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool USimpleSamplePlayer::Init(int32& SampleRate)
{
	InitAudio();
	return true;
}

void USimpleSamplePlayer::InitAudio()
{
	NumChannels = 2;

	if (SoundWave)
	{
		LoadSoundWaveInternal();
	}

	if (!SoundClass)
	{
		static ConstructorHelpers::FObjectFinder<USoundClass> MasterMixFinder(TEXT("/Engine/EngineSounds/Master.Master"));
		if (MasterMixFinder.Succeeded())
		{
			SoundClass = MasterMixFinder.Object;
		}
	}
}

int32 USimpleSamplePlayer::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	if (bIsLoaded)
	{
		const int32 NumFrames = NumSamples / NumChannels;
		std::vector<float> readBuffer;
		readBuffer.resize(NumSamples);

		for (int i = 0; i < NumSamples; ++i)
		{
			OutAudio[i] = 0.0f;
		}

		for (uint32_t looperIndex = 0; looperIndex < AudioPlayers.size(); ++looperIndex)
		{
			auto audioPlayer = AudioPlayers.begin() + looperIndex;
			if (!audioPlayer->Playing())
			{
				continue;
			}

			audioPlayer->Read(readBuffer.data(), NumFrames, NumChannels);

			for (int i = 0; i < NumSamples; ++i)
			{
				OutAudio[i] += Volumes[looperIndex] * readBuffer[i];
			}
		}
	}
	else
	{
		for (int32 Sample = 0; Sample < NumSamples; ++Sample)
		{
			OutAudio[Sample] = 0.0f;
		}
	}
	return NumSamples;
}

void USimpleSamplePlayer::PlayOne(float volumeMult)
{
	const int looperIndex = FirstFreeLooper();
	Volumes[looperIndex] = volumeMult;
	AudioPlayers[looperIndex].Reset();
}

void USimpleSamplePlayer::LoadSoundWaveInternal()
{
	bIsLoaded = false;

	if (SoundWave)
	{
		TFunction<void(const USoundWave* OutSoundWave, const Audio::FSampleBuffer& OutSampleBuffer)> OnLoaded
			= [this](const USoundWave* OutSoundWave, const Audio::FSampleBuffer& OutSampleBuffer)
		{
			if (OutSoundWave == SoundWave)
			{
				SynthCommand([this, OutSampleBuffer]()
				{
					SampleBuffer = OutSampleBuffer;

					const int16* BufferData = SampleBuffer.GetData();
					const int32 BufferNumSamples = SampleBuffer.GetNumSamples();
					const int32 BufferNumFrames = SampleBuffer.GetNumFrames();
					const int32 BufferNumChannels = SampleBuffer.GetNumChannels();
					const int32 BufferSampleRate = SampleBuffer.GetSampleRate();

					for (auto& looper : AudioPlayers)
					{
						looper.SetSampleRate(BufferSampleRate);
					}

					OneShotAudioPlayer::DataInfo info;
					info.d = BufferData;
					info.nChannels = BufferNumChannels;
					info.sampleCount = BufferNumSamples;

					for (auto& looper : AudioPlayers)
					{
						looper.SetData(info);
					}

					bIsLoaded = true;
				});
			}

			NumChannels = OutSoundWave->NumChannels;
		};

		SoundWaveLoader.LoadSoundWave(SoundWave, MoveTemp(OnLoaded), true);
	}
}

int USimpleSamplePlayer::FirstFreeLooper() const
{
	for (int i = 0; i < AudioPlayers.size(); ++i)
	{
		if (!AudioPlayers[i].Playing())
		{
			return i;
		}
	}

	return 0; // TODO: make better fallback
}
