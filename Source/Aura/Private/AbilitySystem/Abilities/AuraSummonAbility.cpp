// Copyright Kickback Studio


#include "AbilitySystem/Abilities/AuraSummonAbility.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = MaxMinions > 3 ? SpawnSpread / MaxMinions : SpawnSpread / 3;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.0f, FVector::UpVector);

	TArray<FVector> SpawnLocations;
	for (int32 i = 0; i < MaxMinions; ++i)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		
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

		const FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		SpawnLocations.Add(ChosenSpawnLocation);
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

	return SpawnLocations;
}
