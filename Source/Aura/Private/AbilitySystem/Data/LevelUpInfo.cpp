// Copyright Kickback Studio


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 FoundLevel = 1;
	// Start checking against level 1 requirements
	for (int i = 1; i < LevelUpInformation.Num(); ++i)
	{
		if (XP >= LevelUpInformation[i].LevelUpRequirement)
		{
			FoundLevel = i + 1;
		}
		else
		{
			// Exit loop early if XP doesn't meet a levelup requirement
			i = LevelUpInformation.Num();
		}
	}

	return FoundLevel;
}