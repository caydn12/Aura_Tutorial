// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IEnemyInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetCombatTarget() const;
};
