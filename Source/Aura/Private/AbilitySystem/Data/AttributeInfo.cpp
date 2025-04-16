// Copyright Kickback Studio


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLog) const
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

	if (bLog && !bFound)
	{
		UE_LOG(LogTemp, Error,
			TEXT("Cant find Info for AttributeTag [%s] on AttributeInfo [%s]."),
			*AttributeTag.ToString(),
			*GetNameSafe(this));
	}

	return FoundInfo;
}
