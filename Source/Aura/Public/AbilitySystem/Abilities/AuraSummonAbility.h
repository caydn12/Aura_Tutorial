// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"


UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations(bool bShowDebug);

	UPROPERTY(EditDefaultsOnly, Category = "Summoning", meta = (ClampMin = "0", ClampMax="31"))
	int32 MaxMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MinSpawnDistance = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MaxSpawnDistance = 400.0f;

	// Angle of AOE
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float SpawnSpread = 360.0f;
};