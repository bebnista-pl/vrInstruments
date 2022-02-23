#include "VelocityTracker.h"

#define DEBUG_LOG(__text__, ...) UE_LOG(LogTemp, Display, TEXT(__text__), __VA_ARGS__)

void VelocityTracker::Tick(float dt, FVector position)
{
	auto v = (position - PrevPos) / dt;
	Velocity = v;
	
	Velocities.Add(v);
	if (Velocities.Num() > n)
	{
		Velocities.RemoveAt(0);
	}
	
	PrevPos = position;
}

FVector VelocityTracker::GetVelocity() const
{
	FVector result = FVector::ZeroVector;
	for (const auto &v : Velocities)
	{
		result += v;
	}

	result /= static_cast<float>(n);
	
	// if (FMath::IsNaN(result.Size()))
	// {
	// 	FVector sum;
	// 	for (const auto &v : Velocities)
	// 	{
	// 		sum += v;
	// 	}
	// 	
	// 	DEBUG_LOG("Nan velocity: %s, sum: %s", &result.ToString()[0], &sum.ToString()[0]);
	// }
	return result;
}
