// Copyright Kickback Studio


#include "Actor/AuraEnemySpawnPoint.h"
#include "Character/AuraEnemy.h"
#include "AuraLogChannels.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	AAuraEnemy* AuraEnemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(
		EnemyClass,
		GetActorTransform(),
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	AuraEnemy->SetLevel(EnemyLevel);

	AuraEnemy->FinishSpawning(GetActorTransform());
	
	AuraEnemy->SpawnDefaultController();
}
