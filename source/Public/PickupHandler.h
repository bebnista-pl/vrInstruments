#pragma once

#include "CoreMinimal.h"

class QUESTTEST_API PickupHandler final
{
public:
	void Setup(UStaticMeshComponent*);
	void Pickup(USceneComponent *AttachTo) const;
	void Drop() const;

private:
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	USceneComponent *RootComponent = nullptr;
	bool bSimulatePhyscis = false;
};
