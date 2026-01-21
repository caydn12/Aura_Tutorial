// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/AuraEnemySpawnPoint.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawner.generated.h"

UCLASS()
class AURA_API AAuraEnemySpawner : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	AAuraEnemySpawner();

	void StartSpawning();
	void StopSpawning();
	void SpawnEnemy();

	void HandleLevelClearedChanged(bool bCleared);

	// Save Interface
	virtual void LoadActor_Implementation() override;
	// End Save Interface

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bLevelCleared = false;
	
	UPROPERTY(EditAnywhere, SaveGame)
	ELevelID LevelID = ELevelID::Level_None;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;

private:
	UPROPERTY(EditAnywhere)
	float SpawnInterval = 5.0f;

	FTimerHandle SpawnTimerHandle;
};
