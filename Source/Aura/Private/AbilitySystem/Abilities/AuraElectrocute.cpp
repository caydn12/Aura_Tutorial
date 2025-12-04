// Copyright Kickback Studio


#include "AbilitySystem/Abilities/AuraElectrocute.h"

FString UAuraElectrocute::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>ELECTROCUTE</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Channels a beam of electricity, "
			"shocking on impact, and dealing </>"
			"<Damage>%d</><Default> lightning damage with a chance to stun.</>"),
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage
		);
	}
	else if (Level == 2)
	{
		return FString::Printf(TEXT(
			"<Title>ELECTROCUTE</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Channels a beam of electricity, "
			"shocking on impact, "
			"and spreading to </><Level>%d </><Default>target, "
			"dealing </><Damage>%d</><Default> lightning damage "
			"with a chance to stun.</>"),
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level - 1, MaxNumShockTargets),
			ScaledDamage
		);
	}
	else
	{
		return FString::Printf(TEXT(
			"<Title>ELECTROCUTE</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Channels a beam of electricity, "
			"shocking on impact, "
			"and spreading to </><Level>%d </><Default>targets, "
			"dealing </><Damage>%d</><Default> lightning damage "
			"with a chance to stun.</>"),
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level - 1, MaxNumShockTargets),
			ScaledDamage
		);
	}
}

FString UAuraElectrocute::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 2)
	{
		return FString::Printf(TEXT(
			"<Title>NEXT LEVEL</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Channels a beam of electricity, "
			"shocking on impact, "
			"and spreading to </><Level>%d </><Default>target, "
			"dealing </><Damage>%d</><Default> lightning damage "
			"with a chance to stun.</>"),
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level - 1, MaxNumShockTargets),
			ScaledDamage
		);
	}
	else
	{
		return FString::Printf(TEXT(
			"<Title>NEXT LEVEL</>\n"
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			"<Default>Channels a beam of electricity, "
			"shocking on impact, "
			"and spreading to </><Level>%d </><Default>targets, "
			"dealing </><Damage>%d</><Default> lightning damage "
			"with a chance to stun.</>"),
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level - 1, MaxNumShockTargets),
			ScaledDamage
		);
	}
}
