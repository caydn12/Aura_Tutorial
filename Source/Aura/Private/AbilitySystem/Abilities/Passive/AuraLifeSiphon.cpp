// Copyright Kickback Studio


#include "AbilitySystem/Abilities/Passive/AuraLifeSiphon.h"

FString UAuraLifeSiphon::GetDescription(const int32 Level)
{
	const int32 LifeSiphonPercentage = LifeSiphonCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>LIFE SIPHON</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		"<Default>Generates a bloodthirst aura "
		"siphoning life by </>"
		"<Damage>%d </><Default>percent of damage dealt</>"),
		Level,
		ManaCost,
		Cooldown,
		LifeSiphonPercentage
	);
}

FString UAuraLifeSiphon::GetNextLevelDescription(const int32 Level)
{
	const int32 LifeSiphonPercentage = LifeSiphonCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>LIFE SIPHON</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		"<Default>Generates a bloodthirst aura "
		"siphoning life by </>"
		"<Damage>%d </><Default>percent of damage dealt</>"),
		Level,
		ManaCost,
		Cooldown,
		LifeSiphonPercentage
	);
}
