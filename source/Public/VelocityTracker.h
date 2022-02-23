#pragma once

#include "CoreMinimal.h"
#include "Containers/CircularQueue.h"

class QUESTTEST_API VelocityTracker
{
public:
	VelocityTracker() {}
	explicit VelocityTracker(int n) : n(n) {}
	
	void Tick(float dt, FVector position);
	
	FVector GetVelocity() const;

private:
	FVector PrevPos;
	TArray<FVector> Velocities;
	FVector Velocity;
	int n = 1;
};
