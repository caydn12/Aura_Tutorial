// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"


UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectilesInSpread(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget);

protected:

	UPROPERTY(EditDefaultsOnly, Category="Firebolt")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "Firebolt")
	float HomingAccelerationMin = 3200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Firebolt")
	float HomingAccelerationMax = 9600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Firebolt")
	bool bLaunchHomingProjectiles = true;
};
