// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class QUESTTEST_API AudioLooper
{
public:
	struct DataInfo
	{
		const int16_t *d;
		uint32_t overlapIndex;
		uint32_t sampleCount;
		uint8_t nChannels;
		bool loop;
	};

	void SetData(const DataInfo&);
	void SetSampleRate(uint32_t rate);
	void Read(float *outData, uint32_t n, int8_t outChannels) const;
	void ClearBuffer();
	bool Finished() const { return _finished; }
	void Reset();
	
private:
	void RefreshNumFrames();
	float ReadNext() const;

	uint32_t _sampleCount = 0;
	uint32_t _numFrames = 0;
	uint32_t _sampleRate = 0;
	uint32_t _overlapSamples = 0;
	uint32_t _overlapStartIndex = 0;
	mutable uint32_t _overlapPosition = 0;
	mutable uint32_t _position = 0;
	const int16_t *_data = nullptr;
	uint8_t _nChannels = 0;

	bool _loop = false;
	mutable bool _finished = true;
};

class QUESTTEST_API OneShotAudioPlayer
{
public:
	struct DataInfo
	{
		const int16_t *d;
		uint32_t sampleCount;
		uint8_t nChannels;
	};

	void SetData(const DataInfo&);
	void SetSampleRate(uint32_t rate);

	void Read(float *outData, uint32_t n, int8_t outChannels) const;
	void Reset();
	bool Playing() const { return _playing; }
	
private:
	float ReadNext() const;

	uint32_t _sampleCount = 0;
	uint32_t _numFrames = 0;
	uint32_t _sampleRate = 0;
	uint8_t _nChannels = 0;
	mutable uint32_t _position = 0;

	const int16_t *_data = nullptr;
	mutable bool _playing = true;
};
