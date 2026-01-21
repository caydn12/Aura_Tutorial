// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

class AAuraEnemy;

UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	TSubclassOf<AAuraEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	int32 EnemyLevel = 1;
};
