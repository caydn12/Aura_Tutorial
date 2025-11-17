// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraPassiveAbility.h"
#include "AuraLifeSiphon.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraLifeSiphon : public UAuraPassiveAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(const int32 Level) override;
	virtual FString GetNextLevelDescription(const int32 Level) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat LifeSiphonCurve;
};
