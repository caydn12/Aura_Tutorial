// Copyright Kickback Studio


#include "AbilitySystem/Abilities/AuraArcaneShards.h"

FString UAuraArcaneShards::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>ARCANE SHARDS</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Summons </><Level>1</><Default> shard of arcane energy "
			"causing radial arcane damage of </>"
			"<Cooldown>%d</><Default> at the shard origin.</>"),
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage
		);
	}
	else
	{
		return FString::Printf(TEXT(
			"<Title>ARCANE SHARDS</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Summons </><Level>%d</><Default> shards of arcane energy "
			"causing radial arcane damage of </>"
			"<Cooldown>%d</><Default> at the shard origin.</>"),
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, MaxNumShards),
			ScaledDamage
		);
	}
}

FString UAuraArcaneShards::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Summons </><Level>%d</><Default> shards of arcane energy "
		"causing radial arcane damage of </>"
		"<Cooldown>%d</><Default> at the shard origin.</>"),
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumShards),
		ScaledDamage
	);
}
