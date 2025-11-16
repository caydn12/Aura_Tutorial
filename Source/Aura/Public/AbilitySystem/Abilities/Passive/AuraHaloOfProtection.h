// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraPassiveAbility.h"
#include "AuraHaloOfProtection.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraHaloOfProtection : public UAuraPassiveAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(const int32 Level) override;
	virtual FString GetNextLevelDescription(const int32 Level) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat DamageReductionCurve;
};
