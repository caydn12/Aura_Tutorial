// Copyright Kickback Studio


#include "AbilitySystem/Abilities/AuraSummonAbility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/AuraCharacterBase.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations(bool bShowDebug)
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = MaxMinions > 6 ? SpawnSpread / MaxMinions : SpawnSpread / 6;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.0f, FVector::UpVector);

	TArray<FVector> SpawnLocations;
	int32 LoopMax = MaxMinions >= 6 ? MaxMinions : 6;
	for (int32 i = 0; i < LoopMax; ++i)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit,
			ChosenSpawnLocation + FVector(0.0f, 0.0f, 400.0f), // Start above the ground
			ChosenSpawnLocation - FVector(0.0f, 0.0f, 400.0f), // End below the ground
			ECC_Visibility // Use visibility channel for the trace
		);

		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}

		if (bShowDebug)
		{
			UKismetSystemLibrary::DrawDebugArrow(
				GetAvatarActorFromActorInfo(),
				Location + (Direction * MinSpawnDistance),
				Location + (Direction * MaxSpawnDistance),
				4.0f,
				FLinearColor::Green,
				3.0f
			);

			DrawDebugSphere(
				GetWorld(),
				Location + (Direction * MinSpawnDistance),
				10.0f,
				12,
				FColor::Red,
				false,
				3.0f
			);

			DrawDebugSphere(
				GetWorld(),
				Location + (Direction * MaxSpawnDistance),
				10.0f,
				12,
				FColor::Red,
				false,
				3.0f
			);

			if (i > 0)
			{
				const FVector LastDirection = LeftOfSpread.RotateAngleAxis(DeltaSpread * (i - 1), FVector::UpVector);
				DrawDebugLine(
					GetWorld(),
					Location + (LastDirection * MinSpawnDistance),
					Location + (Direction * MinSpawnDistance),
					FColor::Blue,
					false,
					3.0f,
					0,
					2.0f
				);

				DrawDebugLine(
					GetWorld(),
					Location + (LastDirection * MaxSpawnDistance),
					Location + (Direction * MaxSpawnDistance),
					FColor::Blue,
					false,
					3.0f,
					0,
					2.0f
				);
			}

			if (i == LoopMax - 1) // for full circle
			{
				const FVector FirstDirection = LeftOfSpread.RotateAngleAxis(0, FVector::UpVector);
				DrawDebugLine(
					GetWorld(),
					Location + (FirstDirection * MinSpawnDistance),
					Location + (Direction * MinSpawnDistance),
					FColor::Blue,
					false,
					3.0f,
					0,
					2.0f
				);

				DrawDebugLine(
					GetWorld(),
					Location + (FirstDirection * MaxSpawnDistance),
					Location + (Direction * MaxSpawnDistance),
					FColor::Blue,
					false,
					3.0f,
					0,
					2.0f
				);
			}

			DrawDebugSphere(
				GetWorld(),
				ChosenSpawnLocation,
				10.0f,
				12,
				FColor::Cyan,
				false,
				3.0f
			);
		}
		SpawnLocations.Add(ChosenSpawnLocation);
	}

	return SpawnLocations;
}

TSubclassOf<AAuraCharacterBase> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}

AAuraCharacterBase* UAuraSummonAbility::GetRandomMinionClassCDO() const
{
	AAuraCharacterBase* Minion = nullptr;
	if (MinionClasses.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
		Minion = MinionClasses[Selection]->GetDefaultObject<AAuraCharacterBase>();
	}
	return Minion;
}

