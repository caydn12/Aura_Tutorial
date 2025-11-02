// Copyright Kickback Studio


#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>FIRE BOLT</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Launches a bolt of Fire, "
			"exploding on impact, and dealing </>"
			"<Damage>%d</><Default> fire damage with a chance to burn.</>"),
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage
		);
	}
	else
	{
		return FString::Printf(TEXT(
			"<Title>FIRE BOLT</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Launches %d bolts of Fire, "
			"exploding on impact, and dealing </>"
			"<Damage>%d</><Default> fire damage with a chance to burn.</>"),
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, MaxNumProjectiles),
			ScaledDamage
		);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Launches %d bolts of Fire, "
		"exploding on impact, and dealing </>"
		"<Damage>%d</><Default> fire damage with a chance to burn.</>"),
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumProjectiles),
		ScaledDamage
	);
}

void UAuraFireBolt::SpawnProjectilesInSpread(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();

	if (bIsServer)
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
			GetAvatarActorFromActorInfo(),
			SocketTag
		);

		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}

		const FVector Forward = Rotation.Vector();
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-ProjectileSpread / 2, FVector::UpVector);
		const FVector RightOfSpread = Forward.RotateAngleAxis(ProjectileSpread / 2, FVector::UpVector);

		const int32 NumProjectilesToSpawn = FMath::Min(GetAbilityLevel(), MaxNumProjectiles);

		// Vector from socket to target
		UKismetSystemLibrary::DrawDebugArrow(
			GetAvatarActorFromActorInfo(),
			SocketLocation,
			SocketLocation + (Forward * 100),
			2.f,
			FLinearColor::White,
			120.f,
			2.f
		);

		// Left spread boundary
		UKismetSystemLibrary::DrawDebugArrow(
			GetAvatarActorFromActorInfo(),
			SocketLocation,
			SocketLocation + (LeftOfSpread * 100),
			2.f,
			FLinearColor::Gray,
			120.f,
			2.f
		);

		// Right spread boundary
		UKismetSystemLibrary::DrawDebugArrow(
			GetAvatarActorFromActorInfo(),
			SocketLocation,
			SocketLocation + (RightOfSpread * 100),
			2.f,
			FLinearColor::Gray,
			120.f,
			2.f
		);

		// Calculate Spread
		if (NumProjectilesToSpawn > 1)
		{
			const float DeltaSpread = ProjectileSpread / (NumProjectilesToSpawn - 1);
			for (int32 i = 0; i < NumProjectilesToSpawn; ++i)
			{
				const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);

				UKismetSystemLibrary::DrawDebugArrow(
					GetAvatarActorFromActorInfo(),
					SocketLocation,
					SocketLocation + (Direction * 75),
					2.f,
					FLinearColor::Red,
					120.f,
					2.f
				);

				SpawnProjectile(SocketLocation + (Direction * 1000), SocketTag, bOverridePitch, PitchOverride);
			}
		}
		else // Do a single spawn
		{
			SpawnProjectile(ProjectileTargetLocation, SocketTag, bOverridePitch, PitchOverride);
		}
	}
}
