// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/AuraCheckpoint.h"
#include "AuraWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraWaypoint : public AAuraCheckpoint
{
	GENERATED_BODY()

public:

	AAuraWaypoint(const FObjectInitializer& ObjectInitializer);

	// Save Interface
	virtual void LoadActor_Implementation() override;
	// End Save Interface

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
