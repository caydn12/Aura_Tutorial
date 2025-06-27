// Copyright Kickback Studio


#include "AbilitySystem/Data/AbilityInfo.h"
#include "AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	FAuraAbilityInfo FoundInfo = FAuraAbilityInfo();
	bool bFound = false;

	for (const FAuraAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			FoundInfo = Info;
			bFound = true;
		}
	}

	if (bLogNotFound && !bFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}

	return FoundInfo;
}
