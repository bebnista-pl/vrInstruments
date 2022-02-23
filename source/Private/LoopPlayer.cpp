#include "LoopPlayer.h"

#define DEBUG_LOG(__text__, ...) UE_LOG(LogTemp, Display, TEXT(__text__), __VA_ARGS__)

ULoopPlayer::ULoopPlayer(const FObjectInitializer &Initializer)
	: Super(Initializer)
	, SoundWave(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

ULoopPlayer::~ULoopPlayer() { }

bool ULoopPlayer::Init(int32& SampleRate)
{
	NumChannels = 2;
	SampleBufferReader.Init(SampleRate);

	if (SoundWave)
	{
		LoadSoundWaveInternal();
	}
	
	return true;
}

void ULoopPlayer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SoundWaveLoader.Update();
}

void ULoopPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void ULoopPlayer::LoadSoundWaveInternal()
{
	bIsLoaded = false;

	if (SoundWave)
	{
		TFunction<void(const USoundWave * OutSoundWave, const Audio::FSampleBuffer & OutSampleBuffer)> OnLoaded
            = [this](const USoundWave * OutSoundWave, const Audio::FSampleBuffer & OutSampleBuffer)
            {
            	if (OutSoundWave == SoundWave)
            	{
            		SynthCommand([this, OutSampleBuffer]()
                    {
                        SampleBuffer = OutSampleBuffer;
            			Looper.ClearBuffer();
                        SampleBufferReader.ClearBuffer();
                    });

            		bIsLoaded = true;
            	}

            	NumChannels = OutSoundWave->NumChannels;
            };

		SoundWaveLoader.LoadSoundWave(SoundWave, MoveTemp(OnLoaded), true);
	}
}

int32 ULoopPlayer::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	if (SampleBuffer.GetData() && !SampleBufferReader.HasBuffer())
    {
    	const int16* BufferData       = SampleBuffer.GetData();
    	const int32 BufferNumSamples  = SampleBuffer.GetNumSamples();
    	const int32 BufferNumFrames = SampleBuffer.GetNumFrames();
    	const int32 BufferNumChannels = SampleBuffer.GetNumChannels();
    	const int32 BufferSampleRate  = SampleBuffer.GetSampleRate();
    	SampleBufferReader.SetPitch(1.0f);

    	SampleBufferReader.SetBuffer(BufferData, BufferNumSamples, BufferNumChannels, BufferSampleRate);
    	Looper.SetSampleRate(BufferSampleRate);
    	
    	AudioLooper::DataInfo info;
    	info.d = BufferData;
    	info.overlapIndex = 1.13f * 44100;
    	info.nChannels = BufferNumChannels;
    	info.sampleCount = BufferNumSamples;
    	info.loop = true;
		
    	Looper.SetData(info);
    }

    if (SampleBufferReader.HasBuffer())
    {
    	const int32 NumFrames = NumSamples / NumChannels;
    	Looper.Read(OutAudio, NumFrames, NumChannels);

    	const float invNumSamples = 1.0f / NumSamples;
    	for (int i = 0; i < NumSamples; ++i)
    	{
    		const float t = static_cast<float>(i) * invNumSamples;
    		OutAudio[i] *= FMath::Lerp(PrevVolume, Volume, t);
    	}

		PrevVolume = Volume;
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
