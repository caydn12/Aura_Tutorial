// Copyright Kickback Studio


#include "Actor/AuraEnemySpawner.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"

AAuraEnemySpawner::AAuraEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);
}

void AAuraEnemySpawner::StartSpawning()
{
	bLevelCleared = false;

	if (SpawnTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AAuraEnemySpawner::SpawnEnemy,
		SpawnInterval,
		true
	);
}

void AAuraEnemySpawner::StopSpawning()
{
	bLevelCleared = true;
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void AAuraEnemySpawner::SpawnEnemy()
{
	int32 SpawnIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);

	if (IsValid(SpawnPoints[SpawnIndex]))
	{
		SpawnPoints[SpawnIndex]->SpawnEnemy();
	}
}

void AAuraEnemySpawner::HandleLevelClearedChanged(bool bCleared)
{
	if (bCleared)
	{
		StopSpawning();
	}
	else
	{
		StartSpawning();
	}
}

void AAuraEnemySpawner::LoadActor_Implementation()
{
	HandleLevelClearedChanged(bLevelCleared);
}

void AAuraEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (!bIsBoundToLevelCleared)
		{
			GameInstance->OnLevelClearedChangedDelegate.AddUObject(this, &AAuraEnemySpawner::HandleLevelClearedChanged);
			bIsBoundToLevelCleared = true;
		}
		HandleLevelClearedChanged(bLevelCleared);
	}
}

