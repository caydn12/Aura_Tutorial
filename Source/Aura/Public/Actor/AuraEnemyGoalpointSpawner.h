// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "Game/AuraGameInstance.h"
#include "AuraEnemyGoalpointSpawner.generated.h"

UENUM(BlueprintType)
enum class EGoalpointState : uint8
{
	Idle,        // rotating red decal, waiting for overlap
	Active,      // red static decal, enemies alive
	Cleared      // blue decal, disabled forever
};

class USphereComponent;
class AAuraEnemySpawnPoint;
class AAuraEnemy;

UCLASS()
class AURA_API AAuraEnemyGoalpointSpawner : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	AAuraEnemyGoalpointSpawner();

protected:
	virtual void BeginPlay() override;

public:

	// Save Interface
	virtual bool ShouldLoadTransform_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;
	// End Save Interface
private:

	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void StartEncounter();

	UFUNCTION()
	void OnEnemyDied(AActor* Enemy);

	void CheckEncounterCleared();

	void UpdateCompletedGoalpoints();

	void SetState(EGoalpointState NewState);

	// Components
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> Decal;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	// Config

	UPROPERTY(EditDefaultsOnly, Category = "Config|Decals")
	TObjectPtr<UMaterialInterface> RedRotatingDecal;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Decals")
	TObjectPtr<UMaterialInterface> RedStaticDecal;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Decals")
	TObjectPtr<UMaterialInterface> BlueClearedDecal;

	UPROPERTY(EditInstanceOnly, Category = "Config|Spawning")
	TArray<TObjectPtr<AAuraEnemySpawnPoint>> SpawnPoints;

	// Runtime

	UPROPERTY()
	TArray<TObjectPtr<AAuraEnemy>> AliveEnemies;

	UPROPERTY(VisibleInstanceOnly, SaveGame)
	EGoalpointState GoalpointState = EGoalpointState::Idle;

	UPROPERTY(SaveGame)
	bool bHasBeenCleared = false;

	UPROPERTY(EditInstanceOnly, Category = "Config")
	ELevelID LevelID;
};
