// Copyright Kickback Studio


#include "AbilitySystem/Abilities/AuraFireBlast.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		"<Title>FIRE BLAST</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Fire Blast causes </><Damage>%d</><Default> "
		"fire damage with a chance to burn.</>\n\n"
		"<Default>Launches %d fire balls in all directions, "
		"each returning toward the caster, "
		"and exploding upon impact with the caster.</>"),
		Level,
		ManaCost,
		Cooldown,
		ScaledDamage,
		NumFireBalls
	);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL:</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Fire Blast causes </><Damage>%d</><Default> "
		"fire damage with a chance to burn.</>\n\n"
		"<Default>Launches %d fire balls in all directions, "
		"each returning toward the caster, "
		"and exploding upon impact with the caster.</>"),
		Level,
		ManaCost,
		Cooldown,
		ScaledDamage,
		NumFireBalls
	);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;

	const FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::GetEvenlySpreadRotators(Forward, FVector::UpVector, 360, NumFireBalls);

	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform = FTransform(Rotator, AvatarLocation);

		if (AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(FireBallClass, SpawnTransform, GetAvatarActorFromActorInfo(), CurrentActorInfo->PlayerController->GetPawn(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

			FireBall->DamageEffectParams = DamageEffectParams;

			FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();

			FireBalls.Add(FireBall);

			FireBall->FinishSpawning(SpawnTransform);
		}
	}

	return FireBalls;
}
