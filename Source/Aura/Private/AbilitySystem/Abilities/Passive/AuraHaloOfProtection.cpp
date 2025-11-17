// Copyright Kickback Studio


#include "AbilitySystem/Abilities/Passive/AuraHaloOfProtection.h"

FString UAuraHaloOfProtection::GetDescription(const int32 Level)
{
	const int32 DamageReduction = DamageReductionCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>HALO OF PROTECTION</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		"<Default>Generates a protective halo "
		"reducing all types of incoming damage by </>"
		"<Damage>%d</>"),
		Level,
		ManaCost,
		Cooldown,
		DamageReduction
	);
}

FString UAuraHaloOfProtection::GetNextLevelDescription(const int32 Level)
{
	const int32 DamageReduction = DamageReductionCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>HALO OF PROTECTION</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		"<Default>Generates a protective halo "
		"reducing all types of incoming damage by </>"
		"<Damage>%d</>"),
		Level,
		ManaCost,
		Cooldown,
		DamageReduction
	);
}
