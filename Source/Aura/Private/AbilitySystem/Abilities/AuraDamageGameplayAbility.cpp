// Copyright Kickback Studio


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.0f);

	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);


	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DebuffDuration = DebuffDuration;

	if (IsValid(TargetActor))
	{
		Params.DeathImpulse = GetImpulse(TargetActor) * DeathImpulseMagnitude;

		const bool bShouldKnockback = FMath::RandRange(1, 100) <= KnockbackChance;
		if (bShouldKnockback)
		{
			Params.KnockbackForce = GetImpulse(TargetActor) * KnockbackForceMagnitude;
		}
	}

	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Params.RadialDamageOrigin = RadialDamageOrigin;
	}

	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtCurrentLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FVector UAuraDamageGameplayAbility::GetImpulse(const AActor* TargetActor, const FVector& OverrideDirection, const bool bOverrideDirection, const float PitchOverride) const
{
	FVector Direction = TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation();
	Direction = bOverrideDirection ? OverrideDirection : Direction;

	Direction.Z = 0.f;
	Direction.Normalize();
	const FVector ToTarget = Direction + FVector::UpVector * FMath::Sin(FMath::DegreesToRadians(PitchOverride));

	/* DEBUG */
	// const FVector Start = TargetActor->GetActorLocation();
	// const FVector End = Start + ToTarget.GetSafeNormal() * 300.f;
	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3, false, 2);

	return ToTarget;
}
