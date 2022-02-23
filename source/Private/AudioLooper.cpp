// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioLooper.h"

#define DEBUG_LOG(__text__, ...) UE_LOG(LogTemp, Display, TEXT(__text__), __VA_ARGS__)

namespace
{
	uint32_t CalculateNumFrames(uint32_t sampleRate, uint32_t sampleCount)
	{
		if (sampleRate == 0)
		{
			return 0;
		}

		return sampleCount / sampleRate;
	}
}

void AudioLooper::SetData(const DataInfo& info)
{
	_data = info.d;
	_sampleCount = info.sampleCount;
	_nChannels = info.nChannels;
	_position = 0;
	_overlapSamples = _sampleCount - info.overlapIndex - 1;
	_overlapStartIndex = info.overlapIndex;
	_loop = info.loop;
	
	// DEBUG_LOG("Overlap index: %d (%d), start: %f", _overlapStartIndex, _sampleCount, info.overlapIndex);
	RefreshNumFrames();
}

void AudioLooper::SetSampleRate(uint32_t rate)
{
	_sampleRate = rate;
	_position = 0;
	RefreshNumFrames();
}

void AudioLooper::Read(float* outData, uint32_t n, int8_t outChannels) const
{
	if (_nChannels == outChannels)
	{
		for (uint32_t i = 0; i < n; ++i)
		{
			outData[i] = ReadNext();
		}
	}
	else if (outChannels == 1)
	{
		const float invCh = 1.0f / static_cast<float>(_nChannels);
		for (uint32_t i = 0; i < n; ++i)
		{
			int32_t sum = 0;
			for (uint8_t ch = 0; ch < _nChannels; ++ch)
			{
				sum += ReadNext();
			}

			outData[i] = invCh * sum;
		}
	}
	else if (_nChannels == 1)
	{
		for (uint32_t i = 0; i < n; ++i)
		{
			const float val = ReadNext();
			for (uint8_t ch = 0; ch < outChannels; ++ch, ++i)
			{
				outData[i] = val;
			}
		}
	}
	else
	{
		checkNoEntry();		
	}
}

void AudioLooper::ClearBuffer()
{
	_data = nullptr;
	_numFrames = 0;
	_sampleRate = 0;
	_sampleCount = 0;
	_nChannels = 0;
}

void AudioLooper::Reset()
{
	_finished = false;
	_position = 0;
}

void AudioLooper::RefreshNumFrames()
{
	_numFrames = CalculateNumFrames(_sampleRate, _sampleCount);
}

float AudioLooper::ReadNext() const
{
	if (_position == _sampleCount)
	{
		if (_loop)
		{
			_position = _overlapPosition;
			_overlapPosition = 0;
		}
		else
		{
			_finished = true;
			return 0.0f;			
		}
	}
	
	constexpr float convToFloat = 1.0f / static_cast<float>(TNumericLimits<int16_t>::Max());

	float overlapValue = 0.0f;
	float alpha = 0.0f;
	
	if (_position >= _overlapStartIndex)
	{
		const float p = static_cast<float>(_overlapPosition) / _overlapSamples;
		// alpha = FMath::Sin(p * HALF_PI);
		alpha = p;
		overlapValue = static_cast<float>(_data[_overlapPosition]) * convToFloat;
		++_overlapPosition;
	}

	const float val = static_cast<float>(_data[_position]) * convToFloat;
	++_position;
	
	const auto oneMinusAlpha = 1.0f - alpha;

	const auto gainOverlap = FMath::Pow(alpha, 0.35f);
	const auto gainMain = FMath::Pow(oneMinusAlpha, 0.35f);

	const float result = val * oneMinusAlpha + overlapValue * alpha; 

	return result;
}

void OneShotAudioPlayer::SetData(const DataInfo &info)
{
	_data = info.d;
	_nChannels = info.nChannels;
	_sampleCount = info.sampleCount;
	_position = 0;
	_playing = true;
	_numFrames = CalculateNumFrames(_sampleRate, _sampleCount);
}

void OneShotAudioPlayer::SetSampleRate(uint32_t rate)
{
	_sampleRate = rate;
	_position = 0;
	_numFrames = CalculateNumFrames(_sampleRate, _sampleCount);
}

void OneShotAudioPlayer::Read(float* outData, uint32_t n, int8_t outChannels) const
{
	if (_nChannels == outChannels)
	{
		for (uint32_t i = 0; i < n; ++i)
		{
			outData[i] = ReadNext();
		}
	}
	else if (outChannels == 1)
	{
		const float invCh = 1.0f / static_cast<float>(_nChannels);
		for (uint32_t i = 0; i < n; ++i)
		{
			int32_t sum = 0;
			for (uint8_t ch = 0; ch < _nChannels; ++ch)
			{
				sum += ReadNext();
			}

			outData[i] = invCh * sum;
		}
	}
	else if (_nChannels == 1)
	{
		for (uint32_t i = 0; i < n; ++i)
		{
			const float val = ReadNext();
			for (uint8_t ch = 0; ch < outChannels; ++ch, ++i)
			{
				outData[i] = val;
			}
		}
	}
	else
	{
		checkNoEntry();		
	}
}

void OneShotAudioPlayer::Reset()
{
	_position = 0;
	_playing = true;
}

float OneShotAudioPlayer::ReadNext() const
{
	if (_position == _sampleCount)
	{
		_playing = false;
		return 0.0f;			
	}
	
	constexpr float convToFloat = 1.0f / static_cast<float>(TNumericLimits<int16_t>::Max());

	const float val = static_cast<float>(_data[_position]) * convToFloat;
	++_position;

	return val;
}
