// Copyright Kickback Studio


#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	FAuraAttributeInfo FoundInfo = FAuraAttributeInfo();
	bool bFound = false;
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			FoundInfo = Info;
			bFound = true;
		}
	}

	if (bLogNotFound && !bFound)
	{
		UE_LOG(LogAura, Error,
			TEXT("Cant find Info for AttributeTag [%s] on AttributeInfo [%s]."),
			*AttributeTag.ToString(),
			*GetNameSafe(this));
	}

	return FoundInfo;
}
