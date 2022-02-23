#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DrumZone.generated.h"

class USimpleSamplePlayer;

USTRUCT(BlueprintType)
struct FDrumZoneLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundWave* soundWave;

	USimpleSamplePlayer* player;;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUESTTEST_API UDrumZone : public USceneComponent
{
	GENERATED_BODY()

public:	
	UDrumZone();

	void PlayOne(float vol);

protected:
	void PlayLevel(float vol);
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USimpleSamplePlayer* SamplePlayer;

	UPROPERTY(EditAnywhere)
	TArray<FDrumZoneLevel> _levels;
};
