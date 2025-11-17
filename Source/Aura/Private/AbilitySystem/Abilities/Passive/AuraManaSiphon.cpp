// Copyright Kickback Studio


#include "AbilitySystem/Abilities/Passive/AuraManaSiphon.h"

FString UAuraManaSiphon::GetDescription(const int32 Level)
{
	const int32 ManaSiphonPercentage = ManaSiphonCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>MANA SIPHON</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		"<Default>Generates an aura of widosm "
		"siphoning life by </>"
		"<Damage>%d </><Default>percent of damage dealt</>"),
		Level,
		ManaCost,
		Cooldown,
		ManaSiphonPercentage
	);
}

FString UAuraManaSiphon::GetNextLevelDescription(const int32 Level)
{
	const int32 ManaSiphonPercentage = ManaSiphonCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>MANA SIPHON</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		"<Default>Generates an aura of widosm "
		"siphoning life by </>"
		"<Damage>%d </><Default>percent of damage dealt</>"),
		Level,
		ManaCost,
		Cooldown,
		ManaSiphonPercentage
	);
}
