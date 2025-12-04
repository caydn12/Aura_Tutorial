// Copyright Kickback Studio


#include "AbilitySystem/Abilities/AuraFireBlast.h"

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
		"and exploding upon impact when returning.</>"),
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
		"and exploding upon impact when returning.</>"),
		Level,
		ManaCost,
		Cooldown,
		ScaledDamage,
		NumFireBalls
	);
}
