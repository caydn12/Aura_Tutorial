// Copyright Kickback Studio


#include "Actor/AuraEnemyGoalpointSpawner.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Actor/AuraEnemySpawnPoint.h"
#include "Character/AuraEnemy.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

AAuraEnemyGoalpointSpawner::AAuraEnemyGoalpointSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorHiddenInGame(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	Decal->SetupAttachment(Root);
	Decal->SetRelativeRotation(FRotator(-90.f, 180.f, 0.f));

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(Root);
}

void AAuraEnemyGoalpointSpawner::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(
		this, &AAuraEnemyGoalpointSpawner::OnSphereOverlap
	);

	if (GoalpointState == EGoalpointState::Active)
	{
		GoalpointState = EGoalpointState::Idle;
	}

	SetState(GoalpointState);
}

void AAuraEnemyGoalpointSpawner::LoadActor_Implementation()
{
	SetState(GoalpointState);
}

void AAuraEnemyGoalpointSpawner::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GoalpointState == EGoalpointState::Idle)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			StartEncounter();
		}
	}
}

void AAuraEnemyGoalpointSpawner::StartEncounter()
{
	SetState(EGoalpointState::Active);

	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	for (AAuraEnemySpawnPoint* SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint)
		{
			AAuraEnemy* AuraEnemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(
				SpawnPoint->EnemyClass,
				SpawnPoint->GetActorTransform(),
				nullptr,
				nullptr,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			);

			AuraEnemy->SetLevel(SpawnPoint->EnemyLevel);

			if (AuraEnemy)
			{
				AliveEnemies.Add(AuraEnemy);

				if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(AuraEnemy))
				{
					if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &AAuraEnemyGoalpointSpawner::OnEnemyDied))
					CombatInterface->GetOnDeathDelegate().AddDynamic(
						this,
						&AAuraEnemyGoalpointSpawner::OnEnemyDied
					);
				}
			}

			AuraEnemy->FinishSpawning(AuraEnemy->GetActorTransform());
			AuraEnemy->SpawnDefaultController();
		}
	}
}

void AAuraEnemyGoalpointSpawner::OnEnemyDied(AActor* Enemy)
{
	if (AAuraEnemy* AuraEnemy = Cast<AAuraEnemy>(Enemy))
	{
		AliveEnemies.Remove(AuraEnemy);
		CheckEncounterCleared();
	}
}

void AAuraEnemyGoalpointSpawner::CheckEncounterCleared()
{
	if (AliveEnemies.Num() == 0)
	{
		SetState(EGoalpointState::Cleared);
		
		if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
		{
			AuraGameInstance->SetLevelCleared(LevelID, true);
		}
	}
}

void AAuraEnemyGoalpointSpawner::UpdateCompletedGoalpoints()
{
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (!bHasBeenCleared)
		{
			AuraGameInstance->SetCompletedGoalpoints(AuraGameInstance->GetCompletedGoalpoints() + 1);

			bHasBeenCleared = true;
		}
	}
}

void AAuraEnemyGoalpointSpawner::SetState(EGoalpointState NewState)
{
	GoalpointState = NewState;

	switch (GoalpointState)
	{
	case EGoalpointState::Idle:
		Decal->SetDecalMaterial(RedRotatingDecal);
		break;
	case EGoalpointState::Active:
		Decal->SetDecalMaterial(RedStaticDecal);
		break;
	case EGoalpointState::Cleared:
		UpdateCompletedGoalpoints();
		Decal->SetDecalMaterial(BlueClearedDecal);
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

